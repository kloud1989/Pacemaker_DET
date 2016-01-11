#ifndef __UTIL_H__
#define __UTIL_H__


#include <stdio.h>
#include <xdc/std.h>
/* Platform utilities include */
#include "ti/platform/platform.h"
#include "ti/platform/resource_mgr.h"

#include "KeyStone_UART_Init_drv.h"
#include "KeyStone_common.h"

#define UART_NUM 	0
#define	MAX_WRITE_LEN	256


extern void board_write(const char *fmt, ... );


#endif
