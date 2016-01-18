/*
 * Services.h
 *
 *  Created on: 2016-1-12
 *      Author: Wayne
 */

#ifndef SERVICES_H_
#define SERVICES_H_

#include <stdio.h>
#include <string.h>

#include <ti/ndk/inc/netmain.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/hal/Hwi.h>

#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>

#include "ADS1298_drv.h"
#include "Global.h"



// 客户端接收指令
typedef enum COMMAND{
	COMMAND_START = 0,
	COMMAND_STOP,
	COMMAND_EXIT,
	COMMAND_INVALID
} COMMAND;

// 程序执行状态
typedef enum STATE{
	STATE_INIT = 0,
	STATE_TRANS,
	STATE_STOP,
	STATE_EXIT
} STATE;

#define CMD_LENGTH 16

extern void ADS1298_ISR(UArg arg);
extern int dtask_tcp(SOCKET sock, UINT32 unused);

#endif /* SERVICES_H_ */
