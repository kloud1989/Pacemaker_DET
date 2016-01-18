/*
 * main.c
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
#include <string.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

/* BIOS6 include */
#include <ti/sysbios/BIOS.h>

#include "ti/platform/platform.h"
#include "KeyStone_UART_Init_drv.h"
#include "KeyStone_DDR_Init.h"
#include "KeyStone_common.h"
#include "Global.h"


static void KeyStone_UART_config(Uint32 baudRate, Bool bLoopBackEnable, UART_Tx_Master txMaster);


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


//---------------------------------------------------------------------
// Main Entry Point
//---------------------------------------------------------------------
int main()
{
	(void) platform_uart_init();
	(void) platform_uart_set_baudrate(115200);
	(void) platform_write_configure(PLATFORM_WRITE_ALL);

	//	KeyStone_UART_config(DEFAULT_BAUD_RATE, FALSE, UART_USE_EDMA_TO_TX);
	//	(void) platform_write_configure(PLATFORM_WRITE_ALL);

	#ifndef DEBUG
	KeyStone_DDR_reinit(50, 20, 1, NULL);
	#endif

	/* Start the BIOS 6 Scheduler */
	BIOS_start();
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

