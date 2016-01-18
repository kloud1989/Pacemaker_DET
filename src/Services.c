/*
 * Services.c
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

#include "Services.h"
#include "dsp.h"

static void retrieve_data(Uint8 *data);

volatile Uint16 DRDYCount = 0;
// TCP发送 NUM2SEND 组采样数据，足够的数据才能够进行算法处理

#define NUM2SEND 100
// #define MAX_1298TIMEOUT 1	//1298DRDY ISR所能容忍的最大超时(ms),2.048M/4/512 = 1KHz
// #define MAIL_TIMEOUT 256	//需要在NUM2SEND*MAX_1298TIMEOUT的时间内处理完数据

volatile STATE FSM_State = STATE_INIT;

// 双缓冲设计
Uint8 DataPool1[NUM2SEND][27];
Uint8 DataPool2[NUM2SEND][27];
Uint8 *dataPool = *DataPool1;

float AllData[CHANNEL_NUM][NUM2SEND];
float *Data2Send;	//暂时只发送单通道数据

// 上位机请求发送通道编号
volatile Uint8 Channel_No = 1;

void ADS1298_ISR(UArg arg)
{
	Uint8 i;
	Uint8 data[27];

	// 读取一次1298SPI数据
	KeyStone_SPI_TxRx(NULL, 0, 0, data, 0, 27, &ADS1298TransferParam);

	for(i=0; i<27; i++)
	{
		dataPool[DRDYCount*27+i] = data[i];
	}

	DRDYCount++;
	if(DRDYCount >= NUM2SEND)
	{
		DRDYCount = 0;
		Semaphore_post(semDRDY);
	}
}

// 获取一次数据以供发送
void GetData()
{
	Uint8 *data;
	while(TRUE)
	{
		/* Wait 10 sets of data from ADS1298_ISR(). */
		Semaphore_pend(semDRDY, BIOS_WAIT_FOREVER);

		// 获取缓冲区地址
		data = dataPool;

		// 切换数据池
		if(dataPool == *DataPool1)
		{
			dataPool = *DataPool2;
		}
		else
		{
			dataPool = *DataPool2;
		}

		retrieve_data(data);

		Data2Send = AllData[8-Channel_No];

		signal_process();

		//发送单通道数据，触发发送事件
		Event_post(eventServer, Event_Id_01);
	}
}

// 通过3字节标志位，检查1298工作状态
Int8 check_status(Uint8 *data)
{
	return 0;
}

// 将1298通道数据转为单精度浮点型
inline float convert_data(Uint8 *inputData)
{
	float ret = 0;
	float Vref = 2.4;

	if((inputData[3] >> 7) == 0x01)	//即符号位为1，是负数
	{
		ret = -Vref*(float)(16777216-(inputData[0]*65536+inputData[1]*256+inputData[2]))/(8388608-1);
	}
	else
	{
		ret = Vref*(float)(inputData[0]*65536+inputData[1]*256+inputData[2])/(8388608-1);
	}

	return ret;
}

// 解析、转换AD数据
void retrieve_data(Uint8 *data)
{
	Int i, j;
	Int status1298;
	Uint8 dataOnce[3];

	status1298 = check_status(data);
	if(status1298 != 0)
	{
		System_printf(" ADS1298 is in an invalid status! \n");
	}

	for(i=0; i<CHANNEL_NUM; i++)
	{
		// 得到某通道的 NUM2SEND 次采样数据
		for(j=0; j<NUM2SEND; j++)
		{
			// 读取某通道的3个字节数据
			dataOnce[0] = data[27*j+3+3*i];
			dataOnce[1] = data[27*j+4+3*i];
			dataOnce[2] = data[27*j+5+3*i];
			AllData[i][j] = convert_data(dataOnce);
		}
	}

	return;
}

void signal_process()
{
	return;
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
		if(channelNo>8 || channelNo<=0)
		{
			platform_write("Bad channel number setting, using default!\n");
			channelNo = 1;
		}
		return channelNo;
	}

}

COMMAND get_command(SOCKET sock)
{
	int ret =0;
	char dataReceived[CMD_LENGTH] = "";

//	recv(sock, (void *)dataReceived, CMD_LENGTH, MSG_WAITALL);
	ret = recv(sock, (void *)dataReceived, CMD_LENGTH, 0);
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
void check_command(SOCKET sock)
{
	COMMAND cmdFromClient = COMMAND_INVALID;

	cmdFromClient = get_command(sock);
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

int dtask_tcp( SOCKET sock, UINT32 unused )
{
	Task_Params taskParams;
	Task_Handle taskGetData;
	Error_Block eb;

	Error_init(&eb);
	Task_Params_init(&taskParams);
	taskParams.stackSize = 1024;
	taskParams.priority = 7;
	taskGetData = Task_create((Task_FuncPtr)GetData, &taskParams, &eb);
	if (taskGetData == NULL)
	{
		System_abort("Task create failed");
	}

    struct timeval to;
    UInt serverEvents;
    int ret;

    (void)unused;

    platform_write("A connection detected! \n");
    // Configure our socket send timeout to be 1 seconds
    to.tv_sec  = 1;
    to.tv_usec = 0;
    setsockopt( sock, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof( to ) );

    // Configure our socket recv timeout to be 10 microseconds
    to.tv_sec  = 0;
    to.tv_usec = 10000;
    setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof( to ) );

    FSM_State = STATE_INIT;
    while(TRUE)
    {
    	serverEvents = Event_pend(eventServer, Event_Id_NONE, Event_Id_00 + Event_Id_01, BIOS_WAIT_FOREVER);

    	// 定时器事件
    	if(serverEvents & Event_Id_00)
    	{
    		check_command(sock);
    	}

    	// 发送就绪事件
    	if(serverEvents & Event_Id_01)
    	{
    		ret = send(sock, (void *)Data2Send, NUM2SEND * sizeof(float), 0);
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


