/*  ============================================================================
 *     Copyright (C) 2013 Texas Instruments Incorporated.       *
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *   ===========================================================================
 */
/** ============================================================================
Interrupts configuration for SPI
 * =============================================================================
 *  Revision History
 *  ===============
 *  23-July-2013 cheng  file created
 *  19-October-2013 Brighton  update to support multiple ports
 * =============================================================================
 */

#include "SPI_Intc.h"

int SPI_event_num = 33;

char * spi_int_str[]=
{
	"Error",
	"receive buffer full",
	"receive buffer overrun",
	"transmit buffer empty"
};

void SPI_Interrupt_Handler()
{
	Uint32 intVector;


	intVector=(spiRegs->INTVEC[0]&CSL_SPI_INTVEC_INTVECT_MASK)>>
		CSL_SPI_INTVEC_INTVECT_SHIFT;
	while((0x11<=intVector)&&(intVector<=0x14))
	{
	    System_printf("SPI %s interrupt happened at %d\n",
	    	spi_int_str[intVector-0x11], TSCL);

		intVector=(spiRegs->INTVEC[0]&CSL_SPI_INTVEC_INTVECT_MASK)>>
			CSL_SPI_INTVEC_INTVECT_SHIFT;
	}
}

void SPI_ISR()
{
	/* Disable the host interrupt */
	CpIntc_disableHostInt(0, SPI_event_num);

	/* Clear SPI Interrupt events in CIC*/
	CpIntc_clearSysInt(0, CSL_INTC0_SPIINT0);

	SPI_Interrupt_Handler();

	/* Enable the host interrupt */
	CpIntc_enableHostInt(0, SPI_event_num);
}

void SPI_Interrupts_Init(void)
{
	Int eventId;
	Hwi_Params params;
	Hwi_Handle spiHwi;
	Error_Block eb;

	/* Disable CIC0 Global host interrupts. */
	CpIntc_disableAllHostInts(0);

	/* Map the System Interrupt i.e. the SPI Interrupt to the SPI_ISR Handler. */
	CpIntc_dispatchPlug(CSL_INTC0_SPIINT0, (CpIntc_FuncPtr)SPI_ISR, (UArg)CSL_INTC0_SPIINT0, TRUE);

	/* The configuration is for CPINTC0. We map system interrupt 54 to Host Interrupt SPI_event_num. */
	CpIntc_mapSysIntToHostInt(0, CSL_INTC0_SPIINT0, SPI_event_num);

	/* Enable the Host Interrupt. */
	CpIntc_enableHostInt(0, SPI_event_num);

	/* Enable Global host interrupts. */
	CpIntc_enableAllHostInts(0);

	/* Enable the System Interrupt */
	CpIntc_enableSysInt(0, CSL_INTC0_SPIINT0);

	/* Get the event id associated with the host interrupt. */
	eventId = CpIntc_getEventId(SPI_event_num);

	Error_init(&eb);
	Hwi_Params_init(&params);

	/* Host interrupt value*/
	params.arg = SPI_event_num;

	/* Event id for your host interrupt */
	params.eventId = eventId;

	/* Enable the Hwi */
	params.enableInt = TRUE;

	/* This plugs the interrupt vector 6 and the ISR function. */
	/* When using CpIntc, you must plug the Hwi fxn with CpIntc_dispatch */
	/* so it knows how to process the CpIntc interrupts.*/
	spiHwi = Hwi_create(6, &CpIntc_dispatch, &params, &eb);
	if (spiHwi == NULL) {
		System_abort("Hwi create failed");
	}
}




