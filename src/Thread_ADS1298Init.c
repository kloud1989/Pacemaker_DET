/*
 * Thread_ADS1298Init.c
 *
 *	Configuration for ADS1298
 *
 *  Created on: 2016-1-12
 *      Author: Wayne
 */


#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/Error.h>
#include "ti/platform/platform.h"
#include "ADS1298_drv.h"
#include "SPI_Intc.h"
#include "GPIO_drv.h"
#include "Global.h"
#include "KeyStone_common.h"
#include "Services.h"

extern void ADS1298_ISR();

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
