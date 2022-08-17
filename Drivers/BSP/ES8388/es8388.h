#ifndef __ES8388_H
#define __ES8388_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//ES8388��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/4/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define ES8388_ADDR     0x10	//ES8388��������ַ,�̶�Ϊ0x10 

u8 ES8388_Init(void);
u8 ES8388_Write_Reg(u8 reg, u8 val);
u8 ES8388_Read_Reg(u8 reg);
void ES8388_I2S_Cfg(u8 fmt, u8 len);
void ES8388_HPvol_Set(u8 volume);
void ES8388_SPKvol_Set(u8 volume);
void ES8388_3D_Set(u8 depth);
void ES8388_ADDA_Cfg(u8 dacen,u8 adcen);
void ES8388_Output_Cfg(u8 o1en,u8 o2en);
void ES8388_MIC_Gain(u8 gain);
void ES8388_ALC_Ctrl(u8 sel,u8 maxgain,u8 mingain);
void ES8388_Input_Cfg(u8 in);
#endif


