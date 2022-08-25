#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板定时器驱动
 * @version:
 * 2022/08/18: 实现了北极星开发板定时器驱动,定时器中断配置,用于实现视频播放器功能
 */

extern TIM_HandleTypeDef TIM3_Handler;  //定时器3句柄
extern TIM_HandleTypeDef TIM6_Handler;  //定时器6句柄

void tim3_init(u16 arr, u16 psc);
void tim6_init(u16 arr, u16 psc);

void tim3_start(void);
void tim3_stop(void);

u32 get_ticknum(void);

#endif
