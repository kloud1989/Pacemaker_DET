#include "GPIO_drv.h"

#define GPIOBANKNUM     (0)

/******************************************************************************
 *
 * Function:		gpioInit
 *
 * Description:		Initializes the GPIO peripheral
 *
 * Parameters:		void
 *
 * Return Value: 	void
 *
 *****************************************************************************/
void gpioInit(void)
{
	Int32 pinNum;
	CSL_GpioHandle hGpio;

	// Open the CSL GPIO Module 0
	hGpio = CSL_GPIO_open(0);

	// Disable the GPIO global interrupts
	CSL_GPIO_bankInterruptDisable(hGpio, GPIOBANKNUM);

	// Clear all falling edge trigger and rising edge trigger
	for (pinNum = GPIO_0; pinNum <= GPIO_15; pinNum++)
	{
		CSL_GPIO_clearFallingEdgeDetect(hGpio, pinNum);
		CSL_GPIO_clearRisingEdgeDetect(hGpio, pinNum);
	}
}

/******************************************************************************
 *
 * Function:		gpioSetDirection
 *
 * Description:		This function configures the specified GPIO's direction
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 * 					direction - GPIO_OUT or GPIO_IN
 *
 * Return Value: 	void
 *
 *****************************************************************************/
void gpioSetDirection(uint32_t uiNumber, GpioDirection direction)
{
	CSL_GpioHandle hGpio;

	// Open the CSL GPIO Module 0
	hGpio = CSL_GPIO_open(0);

	if (direction == GPIO_OUT)
	{
		CSL_GPIO_setPinDirOutput(hGpio, uiNumber); // Set to OUTPUT
	}
	else
	{
		CSL_GPIO_setPinDirInput(hGpio, uiNumber); // Set to INPUT

	}
}

/******************************************************************************
 *
 * Function:		gpioSetOutput
 *
 * Description:		This function sets the specified GPIO's pin state to 1
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 *
 * Pre-Condition:	The specified GPIO should be configured as output
 *
 *****************************************************************************/
void gpioSetOutput(uint32_t uiNumber)
{
	CSL_GpioHandle hGpio;

	// Open the CSL GPIO Module 0
	hGpio = CSL_GPIO_open(0);

	CSL_GPIO_setOutputData(hGpio, uiNumber);
}

/******************************************************************************
 *
 * Function:		gpioClearOutput
 *
 * Description:		This function Clears the specified GPIO's pin state to 0
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 *
 * Pre-Condition:	The specified GPIO should be configured as output
 *
 *****************************************************************************/
void gpioClearOutput(uint32_t uiNumber)
{
	CSL_GpioHandle hGpio;

	// Open the CSL GPIO Module 0
	hGpio = CSL_GPIO_open(0);

	CSL_GPIO_clearOutputData(hGpio, uiNumber);
}

/******************************************************************************
 *
 * Function:		gpioReadInput
 *
 * Description:		This function gets the specified GPIO's pin state
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	uint32_t - Input state of GPIO if success
 * 							  	- else GPIO status
 *
 * Pre-Condition:	The specified GPIO should be configured as input
 *
 *****************************************************************************/
uint32_t gpioReadInput(uint32_t uiNumber)
{
	uint8_t inData = 0;
	CSL_GpioHandle hGpio;

	// Open the CSL GPIO Module 0
	hGpio = CSL_GPIO_open(0);

	if (uiNumber > GPIO_MAX_NUMBER)
		return INVALID_GPIO_NUMBER;

	CSL_GPIO_getInputData(hGpio, uiNumber, &inData);

	if ((inData & GPIO_HIGH) == GPIO_HIGH)
		return GPIO_HIGH;
	else
		return GPIO_LOW;
}

/******************************************************************************
 *
 * Function:		gpioEnableGlobalInterrupt
 *
 * Description:		This function Enables GPIO interrupts to CPU
 *
 * Parameters:		void
 *
 * Return Value: 	void
 *
 *****************************************************************************/
void gpioEnableGlobalInterrupt( void )
{
    CSL_GpioHandle  hGpio;

    // Open the CSL GPIO Module 0
    hGpio = CSL_GPIO_open (0);

    CSL_GPIO_bankInterruptEnable(hGpio, GPIOBANKNUM); //GPIOREGS->BINTEN |= 0x01;
}

/******************************************************************************
 *
 * Function:		gpioDisableGlobalInterrupt
 *
 * Description:		This function Disables GPIO interrupts to CPU
 *
 * Parameters:		void
 *
 * Return Value: 	void
 *
 *****************************************************************************/
void gpioDisableGlobalInterrupt( void )
{
    CSL_GpioHandle  hGpio;

    // Open the CSL GPIO Module 0
    hGpio = CSL_GPIO_open (0);

    CSL_GPIO_bankInterruptDisable(hGpio, GPIOBANKNUM); //GPIOREGS->BINTEN = 0x00;
}

/******************************************************************************
 *
 * Function:		gpioSetRisingEdgeInterrupt
 *
 * Description:		This function sets specified GPIO's rising edge interrupt
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 *
 *****************************************************************************/
void gpioSetRisingEdgeInterrupt( uint32_t uiNumber )
{
    CSL_GpioHandle  hGpio;

    // Open the CSL GPIO Module 0
    hGpio = CSL_GPIO_open (0);

    CSL_GPIO_setRisingEdgeDetect(hGpio, uiNumber); //GPIOREGS->SET_RIS_TRIG |= (1 << uiNumber);
}

/******************************************************************************
 *
 * Function:		gpioClearRisingEdgeInterrupt
 *
 * Description:		This function clears specified GPIO's rising edge interrupt
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 *
 *****************************************************************************/
void gpioClearRisingEdgeInterrupt( uint32_t uiNumber )
{
    CSL_GpioHandle  hGpio;

    // Open the CSL GPIO Module 0
    hGpio = CSL_GPIO_open (0);

    CSL_GPIO_clearRisingEdgeDetect(hGpio, uiNumber); //GPIOREGS->CLR_RIS_TRIG |= (1 << uiNumber);
}

/******************************************************************************
 *
 * Function:		gpioSetFallingEdgeInterrupt
 *
 * Description:		This function sets specified GPIO's falling edge interrupt
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 *
 *****************************************************************************/
void gpioSetFallingEdgeInterrupt( uint32_t uiNumber )
{
    CSL_GpioHandle  hGpio;

    // Open the CSL GPIO Module 0
    hGpio = CSL_GPIO_open (0);

    CSL_GPIO_setFallingEdgeDetect(hGpio, uiNumber); //GPIOREGS->SET_FAL_TRIG |= (1 << uiNumber);
}

/******************************************************************************
 *
 * Function:		gpioClearFallingEdgeInterrupt
 *
 * Description:		This function clears specified GPIO's falling edge interrupt
 *
 * Parameters:		uiNumber - 	GPIO number to configure
 *
 * Return Value: 	void
 *
 *****************************************************************************/
void gpioClearFallingEdgeInterrupt( uint32_t uiNumber )
{
    CSL_GpioHandle  hGpio;

    // Open the CSL GPIO Module 0
    hGpio = CSL_GPIO_open (0);

    CSL_GPIO_clearFallingEdgeDetect(hGpio, uiNumber); //GPIOREGS->CLR_FAL_TRIG |= (1 << uiNumber);
}

