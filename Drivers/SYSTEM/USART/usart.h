#ifndef __USART_H
#define __USART_H

#include "sys.h"
#include "stdio.h"	

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板串口驱动
 * @version:
 * 1.0：实现了北极星开发板串口驱动,串口打印
 */

#define USART_REC_LEN 200  //定义最大接收字节数:200
#define EN_USART1_RX  1    //是否使能串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN];  //接收缓冲,最大USART_REC_LEN个字节,末字节为换行符
extern u16 USART_RX_STA;                 //接收状态标记	
extern UART_HandleTypeDef UART1_Handler; //UART句柄

#define RXBUFFERSIZE 1              //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];  //HAL库USART接收Buffer

void uart_init(u32 baud_rate);

#endif
