/*
 * tcpHello.c
 *
 *
 * Copyright (C) 2007 Texas Instruments Incorporated - http://www.ti.com/ 
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
#include "ti/platform/platform.h"
#include <ti/sysbios/BIOS.h>
#include <xdc/cfg/global.h>
#include <ti/ndk/inc/netmain.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Event.h>
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
volatile STATE FSM_State = STATE_INIT;

// 发送100组采样数据
#define NUM2SEND 100

#define MAIL_TIMEOUT 256	//需要在NUM2SEND*MAX_1298TIMEOUT的时间内处理完数据
#define CMD_LENGTH 16

Uint8 DataFromMail[NUM2MAIL][27];
float ChannelData[NUM2SEND * CHANNEL_NUM];
float Data2Send[NUM2SEND];	//暂时只发送单通道数据
float *CurrentData = ChannelData;
// 上位机请求发送通道编号
volatile Uint8 Channel_No = 0;
volatile SOCKET SockInst = NULL;

void trans_state(STATE state);

// 通过3字节标志位，检查1298工作状态
Int8 check_status(Uint8 *input_data)
{
	return 0;
}

// 将1298通道数据转为单精度浮点型
inline void convert_data(Uint8 *input_data, Uint16 length)
{
	Uint16 i;
	float Vref = 2.4;

	for(i=0; i<length; i++)
	{
		if((input_data[3*i] >> 7) == 0x01)	//即符号位为1，是负数
		{
			CurrentData[i] = -Vref*(float)(16777216-(input_data[0+3*i]*65536+input_data[1+3*i]*256+input_data[2+3*i]))/(8388608-1);
		}
		else
		{
			CurrentData[i] = Vref*(float)(input_data[0+3*i]*65536+input_data[1+3*i]*256+input_data[2+3*i])/(8388608-1);
		}
	}
	CurrentData += length;

	return;
}

// 将每次ISR mail过来的数据存储、转换
void retrieve_data()
{
	Int i, j;
	Int status1298;
	Uint8 channelDataSingle[CHANNEL_NUM * 3];

	for(i=0; i<NUM2MAIL; i++)
	{
		status1298 = check_status(DataFromMail[i]);
		if(status1298 != 0)
		{
			System_printf(" ADS1298 is in an invalid status! \n");
		}

		for(j=0; j<CHANNEL_NUM; j++)
		{
			// 读取某通道的3个字节数据
			channelDataSingle[0+3*j] = DataFromMail[i][3+3*j];
			channelDataSingle[1+3*j] = DataFromMail[i][4+3*j];
			channelDataSingle[2+3*j] = DataFromMail[i][5+3*j];
		}
		convert_data(channelDataSingle, CHANNEL_NUM);
	}

	return;
}

void signal_process()
{
	return;
}

// 获取一次数据以供发送
void getMailData()
{
    Int16 i;

	while(TRUE)
	{
		CurrentData = ChannelData;
//		for(i=0; i<(Int16)(NUM2SEND/NUM2MAIL); i++)
		for(i=0; i<10; i++)
		{
			/* Wait for data to be mailed by ads1298_ISR(). */
			if(Mailbox_pend(mailbox1298, DataFromMail, BIOS_WAIT_FOREVER ) == 0)
			{
				platform_write("One message wasn't delivered!\n");
			}
			retrieve_data();
		}

		for(i=0; i<NUM2SEND; i++)
		{
			// 物理连接上8通道接的其实是1通道，1通道接的是8通道
			Data2Send[i] = ChannelData[i*CHANNEL_NUM+(8-Channel_No)];	//发送对应通道数据
		}

		signal_process();

		//发送单通道数据，触发发送事件
		Event_post(eventServer, Event_Id_01);
	}
}

// 大写字母的ASCII代码是65-90，小写字母的代码是97-112
void to_upper_case(char *input, Uint16 length)
{
	Uint16 i;

	for(i=0; i<length; i++)
	{
		if((input[i]>='a')&&(input[i]<='z'))
		{
			input[i]-= 32;
		}
	}
}

Uint8 getChannelNo(char *data)
{
	char *startPtr = 0;
	Uint8 channelNo;

	startPtr = strstr(data, "CHANNEL=");
	if(NULL == startPtr)
	{
		return 0;
	}
	else
	{
		channelNo = (Uint8)(*(startPtr+8)-'0');
		if(channelNo>8 || channelNo<0)
		{
			platform_write("Bad channel number setting, using default!\n");
			channelNo = 1;
		}
		return channelNo;
	}

}

COMMAND get_command()
{
	int ret =0;
	char dataReceived[CMD_LENGTH] = "";

//	recv(SockInst, (void *)dataReceived, CMD_LENGTH, MSG_WAITALL);
	ret = recv(SockInst, (void *)dataReceived, CMD_LENGTH, 0);
	if(ret == 0)
	{
		platform_write("Client has been closed!\n");
		FSM_State = STATE_EXIT;
	}

	to_upper_case(dataReceived, CMD_LENGTH);

	if(NULL != strstr(dataReceived, "STOP"))
	{
		platform_write("STOP command received!\n");
		return COMMAND_STOP;
	}
	else if(NULL != strstr(dataReceived, "START"))
	{
		platform_write("START command received!\n");
		Channel_No = getChannelNo((char *)dataReceived);
		platform_write("Channel_No = %d \n", Channel_No);
		return COMMAND_START;
	}
	else if(NULL != strstr(dataReceived, "EXIT"))
	{
		platform_write("EXIT command received!\n");
		return COMMAND_EXIT;
	}
	else
	{
		return COMMAND_INVALID;
	}
}

// 检查是否收到指令
void check_command()
{
	COMMAND cmdFromClient = COMMAND_INVALID;

	cmdFromClient = get_command();
	switch(cmdFromClient)
	{
		case COMMAND_START:
			switch(FSM_State)
			{
				case STATE_STOP:
				case STATE_INIT:
					FSM_State = STATE_TRANS;
					Hwi_enableInterrupt(5);
					break;
				default:
					break;
			}
			break;
		case COMMAND_STOP:
			switch(FSM_State)
			{
				case STATE_TRANS:
					FSM_State = STATE_STOP;
					Hwi_disableInterrupt(5);
				default:
					break;
			}
			break;
		case COMMAND_EXIT:
			FSM_State = STATE_EXIT;
		default:
			break;
	}
}

// 定时器每200ms触发
void post_check()
{
	Event_post(eventServer, Event_Id_00);
}

int dtask_tcp_hello( SOCKET sock, UINT32 unused )
{
    struct timeval to;
    UInt serverEvents;
    int ret;

    (void)unused;
    SockInst = sock;

    platform_write("A connection detected! \n");
    // Configure our socket send timeout to be 1 seconds
    to.tv_sec  = 1;
    to.tv_usec = 0;
    setsockopt( SockInst, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof( to ) );

    // Configure our socket recv timeout to be 10 microseconds
    to.tv_sec  = 0;
    to.tv_usec = 10000;
    setsockopt( SockInst, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof( to ) );

    FSM_State = STATE_INIT;
    while(TRUE)
    {
    	serverEvents = Event_pend(eventServer, Event_Id_NONE, Event_Id_00 + Event_Id_01, BIOS_WAIT_FOREVER);

    	// 定时器事件
    	if(serverEvents & Event_Id_00)
    	{
    		check_command();
    	}

    	// 发送就绪事件
    	if(serverEvents & Event_Id_01)
    	{
    		ret = send(SockInst, (void *)Data2Send, NUM2SEND * sizeof(float), 0);
    		if(-1 == ret)
    		{
    			platform_write("A send operation failed, %d\n", fdError());
    			break;
    		}
    		else
    		{
    			// stdio操作耗时长，尽量避免使用
//    			printf(" %d bytes were sent to client successfully! \n", ret);
    		}
    	}

    	if(FSM_State == STATE_EXIT)
    	{
    		platform_write("Going to close the socket!\n");
    		break;
    	}
    }
    // NDK函数不能被高于NDK task的task，以及swi、hwi调用
    // NDK中调用的高优先级task的优先级会被强制转为ndk task的优先级

    Hwi_disableInterrupt(5);

    // 上位机断开连接，因此撤销本次socket连接，等待下一次连接，return 0
	// Returns "1" if socket 's' is still open, and "0" if its been closed
    return(0);
}


