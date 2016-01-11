#ifndef	_GPIO_DRV_H_
#define	_GPIO_DRV_H_

#include <xdc/std.h>
#include <ti/csl/csl_gpioAux.h>

#define	GPIO_0					(0)
#define	GPIO_1					(1)
#define	GPIO_2					(2)
#define	GPIO_3					(3)
#define	GPIO_4					(4)
#define	GPIO_5					(5)
#define	GPIO_6					(6)
#define	GPIO_7					(7)
#define	GPIO_8					(8)
#define	GPIO_9					(9)
#define	GPIO_10					(10)
#define	GPIO_11					(11)
#define	GPIO_12					(12)
#define	GPIO_13					(13)
#define	GPIO_14					(14)
#define	GPIO_15					(15)

#define	GPIO_MAX_NUMBER			(15)

// IO¿Ú×´Ì¬
#define	GPIO_LOW				(0)
#define	GPIO_HIGH				(1)
#define	INVALID_GPIO_NUMBER		(2)
#define	INVALID_GPIO_DIRECTION	(3)
#define	INVALID_GPIO_STATE		(4)

typedef	enum _GpioDirection
{
	GPIO_OUT = 0,
	GPIO_IN
}GpioDirection;

void gpioInit(void);
void gpioSetDirection(Uint32 uiNumber, GpioDirection direction);
void gpioSetOutput(Uint32 uiNumber);
void gpioClearOutput(Uint32 uiNumber);
Uint32 gpioReadInput(Uint32 uiNumber);
void gpioEnableGlobalInterrupt(void);
void gpioDisableGlobalInterrupt(void);
void gpioSetRisingEdgeInterrupt(Uint32 uiNumber);
void gpioClearRisingEdgeInterrupt(Uint32 uiNumber);
void gpioSetFallingEdgeInterrupt(uint32_t	uiNumber);
void gpioClearFallingEdgeInterrupt(Uint32 uiNumber);


#endif
