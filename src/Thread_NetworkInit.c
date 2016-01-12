/*
 * Thread_NetworkInit.c
 *
 *  Created on: 2016-1-12
 *      Author: Wayne
 */

#include <ti/ndk/inc/netmain.h>
#include <xdc/std.h>
#include "ti/platform/resource_mgr.h"
#include <xdc/runtime/System.h>
#include "Services.h"

// Our NETCTRL callback functions
static void NetworkOpen();
static void NetworkClose();
static void NetworkIPAddr(IPN IPAddr, uint IfIdx, uint fAdd);

// Fun reporting function
static void ServiceReport(uint Item, uint Status, uint Report, HANDLE hCfgEntry);

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
	hHello = DaemonNew(SOCK_STREAM, 0, 6350, dtask_tcp, OS_TASKPRINORM,
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

