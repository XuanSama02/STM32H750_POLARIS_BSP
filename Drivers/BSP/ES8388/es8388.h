#ifndef __ES8388_H
#define __ES8388_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//ES8388驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2019/4/24
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define ES8388_ADDR     0x10	//ES8388的器件地址,固定为0x10 

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


