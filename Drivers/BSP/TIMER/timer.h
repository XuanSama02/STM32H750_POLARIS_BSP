#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32H7������
//��ʱ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
extern TIM_HandleTypeDef TIM3_Handler;      //��ʱ��3��� 
extern TIM_HandleTypeDef TIM6_Handler;      //��ʱ��6��� 

void TIM3_Init(u16 arr,u16 psc);
void TIM6_Init(u16 arr,u16 psc);

void TIM3_Start(void);
void TIM3_Stop(void);
u32 GetTicknum(void);
#endif
