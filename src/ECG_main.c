/*
 * ECG_main.c
 *
 * Based on TCP/IP Stack 'Hello World!' Example which is ported to use BIOS6 OS.
 *
 * Copyright (C) 2007, 2011 Texas Instruments Incorporated - http://www.ti.com/
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <ti/ndk/inc/netmain.h>

#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>

/* BIOS6 include */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Semaphore.h>
//#include <ti/sysbios/knl/Swi.h>

/* Platform utilities include */
#include "ti/platform/platform.h"
#include "ti/platform/resource_mgr.h"

#include "KeyStone_SPI_Init_drv.h"
#include "KeyStone_UART_Init_drv.h"
#include "KeyStone_DDR_Init.h"
#include "Utils.h"
#include "KeyStone_common.h"
#include "ADS1298_drv.h"
#include "SPI_Intc.h"
#include "GPIO_drv.h"
#include "Global.h"


// Our NETCTRL callback functions
static void NetworkOpen();
static void NetworkClose();
static void NetworkIPAddr(IPN IPAddr, uint IfIdx, uint fAdd);

// Fun reporting function
static void ServiceReport(uint Item, uint Status, uint Report, HANDLE hCfgEntry);

int ADS1298Init();
void KeyStone_UART_config(Uint32 baudRate, Bool bLoopBackEnable, UART_Tx_Master txMaster);
extern int dtask_tcp_hello();

char *HostName = "tidsp";
char *LocalIPAddr = "192.168.2.100";
char *LocalIPMask = "255.255.255.0"; // Not used when using DHCP
char *GatewayIP = "192.168.2.101"; // Not used when using DHCP
char *DomainName = "demo.net"; // Not used when using DHCP
char *DNSServer = "0.0.0.0"; // Used when set to anything but zero

// Simulator EMAC Switch does not handle ALE_LEARN mode, so please configure the
// MAC address of the PC where you want to launch the webpages and initiate PING to NDK */
Uint8 clientMACAddress[6] =
{ 0xA4, 0x1F, 0x72, 0x8C, 0x30, 0xB8 }; /* MAC address for my PC */


volatile Uint16 DRDYCount = 0; /* 计数到NUM2MAIL再mail*/

#define MAX_1298TIMEOUT 1	//1298DRDY ISR所能容忍的最大超时(ms),2.048M/4/512 = 1KHz
Uint8 Data2Mail[NUM2MAIL][27];


/*************************************************************************
 *  @b EVM_init()
 * 
 *  @n
 *     
 *  Initializes the platform hardware. This routine is configured to start in 
 *   the evm.cfg configuration file. It is the first routine that BIOS 
 *   calls and is executed before Main is called. If you are debugging within
 *  CCS the default option in your target configuration file may be to execute 
 *  all code up until Main as the image loads. To debug this you should disable
 *  that option. 
 *
 *  @param[in]  None
 * 
 *  @retval
 *      None
 ************************************************************************/
void EVM_init()
{
	platform_init_flags sFlags;
	platform_init_config sConfig;
	/* Status of the call to initialize the platform */
	int32_t pform_status;

//	(void) platform_uart_init();
//	(void) platform_uart_set_baudrate(115200);
//	(void) platform_write_configure(PLATFORM_WRITE_ALL);

	KeyStone_UART_config(DEFAULT_BAUD_RATE, FALSE, UART_USE_EDMA_TO_TX);
	(void) platform_write_configure(PLATFORM_WRITE_ALL);

	/*
	 * You can choose what to initialize on the platform by setting the following
	 * flags. Things like the DDR, PLL, etc should have been set by the boot loader.
	 */
	memset((void *) &sFlags, 0, sizeof(platform_init_flags));
	memset((void *) &sConfig, 0, sizeof(platform_init_config));

	sFlags.pll = 1; /* PLLs for clocking    */
	sFlags.ddr = 0; /* External memory    */
	sFlags.tcsl = 1; /* Time stamp counter   */
	sFlags.phy = 1; /* Ethernet       */
	sFlags.ecc = 0; /* Memory ECC       */
	sConfig.pllm = 0; /* Use libraries default clock divisor */

	pform_status = platform_init(&sFlags, &sConfig);

	/* If we initialized the platform okay */
	if (pform_status != Platform_EOK)
	{
		System_abort("Platform Initial Failed!");
	}

	// The DDR configuration table for C6678 doesn't contain DDR levelling configuration,
	// so it is recommended that you run the DDR at lower speeds while initializing it from the RBL
	// and then speed it up from the application initialization code.
	// KeyStone_DDR_reinit(50, 20, 1, NULL);
}

void Setup_EDMA_for_UART(Uint32 uiCC, Uint32 uiChannel, Uint32 uiTC)
{
	EDMA_channel_TC_cfg(gpEDMA_CC_regs[uiCC], uiChannel, uiTC);
	if(uiChannel<32)
	{
		//enable the EDMA channel
		gpEDMA_CC_regs[uiCC]->TPCC_EESR= 1<<(uiChannel);
		//enable the EDMA channel interrupt
		gpEDMA_CC_regs[uiCC]->TPCC_IESR= 1<<(uiChannel);
	}
	else
	{
		//enable the EDMA channel
		gpEDMA_CC_regs[uiCC]->TPCC_EESRH= 1<<(uiChannel-32);
		//enable the EDMA channel interrupt
		gpEDMA_CC_regs[uiCC]->TPCC_IESRH= 1<<(uiChannel-32);
	}
}

/*UART configure*/
void KeyStone_UART_config(Uint32 baudRate, Bool bLoopBackEnable,
	UART_Tx_Master txMaster)
{
	UART_Config    gUARTCfg;

	if(bLoopBackEnable)
		printf("\nInit UART%d at %dbps in loopback mode...\n", UART_NUM, baudRate);
	else
		printf("\nInit UART%d at %dbps...\n", UART_NUM, baudRate);

	//clear the configuration data structure
	memset(&gUARTCfg, 0, sizeof(gUARTCfg));

	gUARTCfg.baudRate = baudRate;
	gUARTCfg.DSP_Core_Speed_Hz = gDSP_Core_Speed_Hz;
	gUARTCfg.dataLen = DATA_LEN_8BIT;
	gUARTCfg.parityMode = PARITY_DISABLE;
	gUARTCfg.stopMode = ONE_STOP_BIT;
    gUARTCfg.autoFlow = AUTO_FLOW_DIS;
	gUARTCfg.osmSel = OVER_SAMPLING_16X;
    gUARTCfg.fifoRxTriBytes = TRIGGER_LEVEL_14BYTE;
    gUARTCfg.txMaster = txMaster;
	gUARTCfg.bLoopBackEnable = bLoopBackEnable;

	KeyStone_UART_init(&gUARTCfg, UART_NUM);

	if(gUARTCfg.txMaster == UART_USE_EDMA_TO_TX)
	{
		EDMA_init();
		Setup_EDMA_for_UART(2, CSL_TPCC2_UTXEVT, 0);
	}
}

Task_Handle tskADS1298Init;

//---------------------------------------------------------------------
// Main Entry Point
//---------------------------------------------------------------------
int main()
{
	Task_Params taskParams;
	Error_Block eb;

	KeyStone_DDR_reinit(50, 20, 1, NULL);

	Error_init(&eb);
	Task_Params_init(&taskParams);
	taskParams.stackSize = 2048;
	taskParams.priority = 6;
	tskADS1298Init = Task_create((Task_FuncPtr)ADS1298Init, &taskParams, &eb);
	if (tskADS1298Init == NULL) {
	    System_abort("Task create failed");
	}

	/* Start the BIOS 6 Scheduler */
	BIOS_start();
}

int NetworkInit()
{
	int rc;
	HANDLE hCfg;
	QMSS_CFG_T qmss_cfg;
	CPPI_CFG_T cppi_cfg;

	/* Initialize the components required to run this application:
	 *  (1) QMSS
	 *  (2) CPPI
	 *  (3) Packet Accelerator
	 */
	/* Initialize QMSS */
	if (platform_get_coreid() == 0)
	{
		qmss_cfg.master_core = 1;
	}
	else
	{
		qmss_cfg.master_core = 0;
	}
	qmss_cfg.max_num_desc = MAX_NUM_DESC;
	qmss_cfg.desc_size = MAX_DESC_SIZE;
	qmss_cfg.mem_region = Qmss_MemRegion_MEMORY_REGION0;
	if (res_mgr_init_qmss(&qmss_cfg) != 0)
	{
		platform_write("Failed to initialize the QMSS subsystem \n");
		goto main_exit;
	}
	else
	{
		platform_write("QMSS successfully initialized \n");
	}

	/* Initialize CPPI */
	if (platform_get_coreid() == 0)
	{
		cppi_cfg.master_core = 1;
	}
	else
	{
		cppi_cfg.master_core = 0;
	}
	cppi_cfg.dma_num = Cppi_CpDma_PASS_CPDMA;
	cppi_cfg.num_tx_queues = NUM_PA_TX_QUEUES;
	cppi_cfg.num_rx_channels = NUM_PA_RX_CHANNELS;
	if (res_mgr_init_cppi(&cppi_cfg) != 0)
	{
		platform_write("Failed to initialize CPPI subsystem \n");
		goto main_exit;
	}
	else
	{
		platform_write("CPPI successfully initialized \n");
	}

	if (res_mgr_init_pass() != 0)
	{
		platform_write("Failed to initialize the Packet Accelerator \n");
		goto main_exit;
	}
	else
	{
		platform_write("PA successfully initialized \n");
	}

	//
	// THIS MUST BE THE ABSOLUTE FIRST THING DONE IN AN APPLICATION before
	//  using the stack!!
	//
	rc = NC_SystemOpen(NC_PRIORITY_LOW, NC_OPMODE_INTERRUPT);
	if (rc)
	{
		platform_write("NC_SystemOpen Failed (%d)\n", rc);
		for (;;)
			;
	}


	//
	// Create and build the system configuration from scratch.
	//

	// Create a new configuration
	hCfg = CfgNew();
	if (!hCfg)
	{
		platform_write("Unable to create configuration\n");
		goto main_exit;
	}

	//
	// THIS MUST BE THE ABSOLUTE FIRST THING DONE IN AN APPLICATION!!
	//
	rc = NC_SystemOpen(NC_PRIORITY_LOW, NC_OPMODE_INTERRUPT);
	if (rc)
	{
		platform_write("NC_SystemOpen Failed (%d)\n", rc);
		for (;;)
			;
	}

	//
	// Create and build the system configuration from scratch.
	//

	// Create a new configuration
	hCfg = CfgNew();
	if (!hCfg)
	{
		platform_write("Unable to create configuration\n");
		goto main_exit;
	}

	// We better validate the length of the supplied names
	if (strlen(DomainName) >= CFG_DOMAIN_MAX
			|| strlen(HostName) >= CFG_HOSTNAME_MAX)
	{
		platform_write("Names too long\n");
		goto main_exit;
	}

	// Add our global hostname to hCfg (to be claimed in all connected domains)
	CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_HOSTNAME, 0,
			strlen(HostName), (UINT8 *) HostName, 0);

	// If the IP address is specified, manually configure IP and Gateway
	if (1)
	{
		CI_IPNET NA;
		CI_ROUTE RT;
		IPN IPTmp;

		// Setup manual IP address
		bzero( &NA, sizeof(NA));
		NA.IPAddr = inet_addr(LocalIPAddr);
		NA.IPMask = inet_addr(LocalIPMask);
		strcpy(NA.Domain, DomainName);
		NA.NetType = 0;

		// Add the address to interface 1
		CfgAddEntry(hCfg, CFGTAG_IPNET, 1, 0, sizeof(CI_IPNET), (UINT8 *) &NA,
				0);

		// Add the default gateway. Since it is the default, the
		// destination address and mask are both zero (we go ahead
		// and show the assignment for clarity).
		bzero( &RT, sizeof(RT));
		RT.IPDestAddr = 0;
		RT.IPDestMask = 0;
		RT.IPGateAddr = inet_addr(GatewayIP);

		// Add the route
		CfgAddEntry(hCfg, CFGTAG_ROUTE, 0, 0, sizeof(CI_ROUTE), (UINT8 *) &RT,
				0);

		// Manually add the DNS server when specified
		IPTmp = inet_addr(DNSServer);
		if (IPTmp)
			CfgAddEntry(hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER, 0,
					sizeof(IPTmp), (UINT8 *) &IPTmp, 0);
	}
	// Else we specify DHCP
	else
	{
		CI_SERVICE_DHCPC dhcpc;

		// Specify DHCP Service on IF-1
		bzero( &dhcpc, sizeof(dhcpc));
		dhcpc.cisargs.Mode = CIS_FLG_IFIDXVALID;
		dhcpc.cisargs.IfIdx = 1;
		dhcpc.cisargs.pCbSrv = &ServiceReport;
		CfgAddEntry(hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, 0,
				sizeof(dhcpc), (UINT8 *) &dhcpc, 0);
	}

	//
	// Configure IPStack/OS Options
	//

	// We don't want to see debug messages less than WARNINGS
	rc = DBG_WARN;
	CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_DBGPRINTLEVEL, CFG_ADDMODE_UNIQUE,
			sizeof(uint), (UINT8 *) &rc, 0);

	//
	// This code sets up the TCP and UDP buffer sizes
	// (Note 8192 is actually the default. This code is here to
	// illustrate how the buffer and limit sizes are configured.)
	//

	// TCP Receive limit
	rc = 8192;
	CfgAddEntry(hCfg, CFGTAG_IP, CFGITEM_IP_SOCKTCPRXLIMIT, CFG_ADDMODE_UNIQUE,
			sizeof(uint), (UINT8 *) &rc, 0);

	//
	// Boot the system using this configuration
	//
	// We keep booting until the function returns 0. This allows
	// us to have a "reboot" command.
	//
	do
	{
		rc = NC_NetStart(hCfg, NetworkOpen, NetworkClose, NetworkIPAddr);
	} while (rc > 0);

	// Delete Configuration
	CfgFree(hCfg);

	// Close the OS
	main_exit: NC_SystemClose();
	return (0);
}

void SPI_Init()
{
	SPI_Config spiCfg;
	SPI_Interrupt_Config spiIntCfg;

	//On Shannon, SPI belong to clock domain 3 which need be enabled
	KeyStone_enable_PSC_module(CSL_PSC_PD_ALWAYSON, 3);

	/*clear configuration structure to default values*/
	memset(&spiCfg, 0, sizeof(spiCfg));
	memset(&spiIntCfg, 0, sizeof(spiIntCfg));

	spiCfg.loopBack = FALSE;
	spiCfg.DMA_requestEnable = FALSE;
	/*data format for ADS1298 test*/
	spiCfg.dataFormat[0] = &ADS1298DataFormat;

	SPI_Interrupts_Init();
	spiIntCfg.overrunInterruptEnable = TRUE;
	spiIntCfg.bitErrorInterruptEnable = TRUE;
	spiCfg.interruptCfg = &spiIntCfg;

	spiCfg.number_SPI_pins = 5;
	spiCfg.CS_polarity = SPI_CS_LOW_ACTIVE;

	KeyStone_SPI_init(&spiCfg);
}

void Mail_data()
{
	while(1)
	{
		Semaphore_pend(semDRDY, BIOS_WAIT_FOREVER);

		// mail post操作不能放在HWI、SWI中，因为该操作会用到semaphore_pend
//		if(Mailbox_post(mailbox1298, Data2Mail, MAX_1298TIMEOUT) == FALSE)
		if(Mailbox_post(mailbox1298, Data2Mail, BIOS_WAIT_FOREVER) == FALSE)
		{
			System_printf("One message missed!\n");
		}
	}
}

void ADS1298_ISR(UArg arg)
{
	Uint8 i;
	Uint8 data[27];

	// 读取一次1298SPI数据
	KeyStone_SPI_TxRx(NULL, 0, 0, data, 0, 27, &ADS1298TransferParam);

	for(i=0; i<27; i++)
	{
		Data2Mail[DRDYCount][i] = data[i];
	}

	DRDYCount++;
	if(DRDYCount >= NUM2MAIL)
	{
		DRDYCount = 0;
		Semaphore_post(semDRDY);
	}
}

void ADS1298_drdy_init()
{
	Hwi_Handle ads1298Hwi;
	Hwi_Params hwiParams;
	Error_Block eb;

	Error_init(&eb);
	Hwi_Params_init(&hwiParams);

	// set the argument you want passed to your ISR function
	hwiParams.arg = 5;

	// set the event id of the peripheral assigned to this interrupt: 88 for GPINT14
	hwiParams.eventId = 88;

	// don't allow this interrupt to nest itself
	hwiParams.maskSetting = Hwi_MaskingOption_SELF;

	hwiParams.enableInt = FALSE;

	// Configure interrupt 5 to invoke "ads1298ISR".
	// Automatically enables interrupt 5 by default
	// set params.enableInt = FALSE if you want to control
	// when the interrupt is enabled using Hwi_enableInterrupt()
	ads1298Hwi = Hwi_create(5, ADS1298_ISR, &hwiParams, &eb);
	if (ads1298Hwi == NULL) {
		System_abort("Hwi create failed");
	}
}

int ADS1298Init()
{
	SPI_Init();

	gpioInit();
	gpioSetDirection(ADS1298_RESETz, GPIO_OUT);
	gpioSetDirection(ADS1298_DRDY, GPIO_IN);
	gpioSetDirection(ADS1298_START, GPIO_OUT);
	gpioSetFallingEdgeInterrupt(ADS1298_DRDY);
	gpioClearOutput(ADS1298_START);
	gpioClearOutput(ADS1298_RESETz);


	// Delay for Power-On Reset
	// Wait after power-up until RESET: tPOR = 2^18 tCLK = 0.125s
	gpioSetOutput(ADS1298_RESETz);
	platform_delay(20);

	// Issue Reset Pulse,
	gpioClearOutput(ADS1298_RESETz);
	platform_delay(2);	// Reset low width tRST = 2tCLK
	gpioSetOutput(ADS1298_RESETz);

	// Wait for 18 tCLK	which is required to execute the RESET command.
	// Avoid sending any commands during this time.
	platform_delay(18);

	ADS1298_config_init();
	ADS1298_drdy_init();

	// Set START = 1
	gpioSetOutput(ADS1298_START);

	// Ready to capture DRDY
	gpioEnableGlobalInterrupt();

	return (0);
}

//
// System Task Code [ Server Daemon Servers ]
//
static HANDLE hHello = 0;

// NetworkOpen
//
// This function is called after the configuration has booted
//
static void NetworkOpen()
{
	// Create our local server
	hHello = DaemonNew(SOCK_STREAM, 0, 6350, dtask_tcp_hello, OS_TASKPRINORM,
			OS_TASKSTKNORM, 0, 1);
}

//
// NetworkClose
//
// This function is called when the network is shutting down,
// or when it no longer has any IP addresses assigned to it.
//
static void NetworkClose()
{
	DaemonFree(hHello);
}

//
// NetworkIPAddr
//
// This function is called whenever an IP address binding is
// added or removed from the system.
//
static void NetworkIPAddr(IPN IPAddr, uint IfIdx, uint fAdd)
{
	IPN IPTmp;

	if (fAdd)
		platform_write("Network Added: ");
	else
		platform_write("Network Removed: ");

	// Print a message
	IPTmp = ntohl( IPAddr );
	platform_write("If-%d:%d.%d.%d.%d\n", IfIdx, (UINT8) (IPTmp >> 24) & 0xFF,
			(UINT8) (IPTmp >> 16) & 0xFF, (UINT8) (IPTmp >> 8) & 0xFF,
			(UINT8) IPTmp & 0xFF);
}

//
// Service Status Reports
//
// Here's a quick example of using service status updates
//
static char *TaskName[] =
{ "Telnet", "HTTP", "NAT", "DHCPS", "DHCPC", "DNS" };
static char *ReportStr[] =
{ "", "Running", "Updated", "Complete", "Fault" };
static char *StatusStr[] =
{ "Disabled", "Waiting", "IPTerm", "Failed", "Enabled" };
static void ServiceReport(uint Item, uint Status, uint Report, HANDLE h)
{
	System_printf("Service Status: %-9s: %-9s: %-9s: %03d\n", TaskName[Item - 1],
			StatusStr[Status], ReportStr[Report / 256], Report & 0xFF);

	//
	// Example of adding to the DHCP configuration space
	//
	// When using the DHCP client, the client has full control over access
	// to the first 256 entries in the CFGTAG_SYSINFO space.
	//
	// Note that the DHCP client will erase all CFGTAG_SYSINFO tags except
	// CFGITEM_DHCP_HOSTNAME. If the application needs to keep manual
	// entries in the DHCP tag range, then the code to maintain them should
	// be placed here.
	//
	// Here, we want to manually add a DNS server to the configuration, but
	// we can only do it once DHCP has finished its programming.
	//
	if (Item == CFGITEM_SERVICE_DHCPCLIENT && Status == CIS_SRV_STATUS_ENABLED
			&& (Report == (NETTOOLS_STAT_RUNNING | DHCPCODE_IPADD)
					|| Report == (NETTOOLS_STAT_RUNNING | DHCPCODE_IPRENEW)))
	{
		IPN IPTmp;

		// Manually add the DNS server when specified
		IPTmp = inet_addr(DNSServer);
		if (IPTmp)
			CfgAddEntry(0, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER, 0,
					sizeof(IPTmp), (UINT8 *) &IPTmp, 0);
	}
}

