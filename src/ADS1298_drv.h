#ifndef _ADS1298_DRV_H_
#define _ADS1298_DRV_H_

#include <tistdtypes.h>
#include "KeyStone_SPI_Init_drv.h"
#include "Utils.h"
#include <stdio.h>

extern SPI_Data_Format ADS1298DataFormat;
extern SPI_Transfer_Param ADS1298TransferParam;

extern void ADS1298_config_init();

#endif
