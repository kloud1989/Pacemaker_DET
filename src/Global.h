/*
 * Global.h
 *	
 *  Created on: 2015-4-30
 *  Global modified
 *      Author: Wayne
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

//default baud rate used for tests except for loopback
#define DEFAULT_BAUD_RATE 115200
//the number of the UART be tested
#define UART_NUM 	0

// GPIO definition:
// GPIO_13: ADS1298_RESET
// GPIO_14: ADS1298_DRDY
// GPIO_15: ADS1298_START
#define ADS1298_RESETz 	GPIO_13
#define ADS1298_DRDY 	GPIO_14
#define ADS1298_START 	GPIO_15

// 每组1298ISR mail的数据组数
#define NUM2MAIL 10

// 1298通道数
#define CHANNEL_NUM 8
#endif /* GLOBAL_H_ */
