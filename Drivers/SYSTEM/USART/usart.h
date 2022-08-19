#ifndef __USART_H
#define __USART_H

#include "sys.h"
#include "stdio.h"	

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ����崮������
 * @version:
 * 1.0��ʵ���˱����ǿ����崮������,���ڴ�ӡ
 */

#define USART_REC_LEN 200  //�����������ֽ���:200
#define EN_USART1_RX  1    //�Ƿ�ʹ�ܴ���1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN];  //���ջ���,���USART_REC_LEN���ֽ�,ĩ�ֽ�Ϊ���з�
extern u16 USART_RX_STA;                 //����״̬���	
extern UART_HandleTypeDef UART1_Handler; //UART���

#define RXBUFFERSIZE 1              //�����С
extern u8 aRxBuffer[RXBUFFERSIZE];  //HAL��USART����Buffer

void uart_init(u32 baud_rate);

#endif
