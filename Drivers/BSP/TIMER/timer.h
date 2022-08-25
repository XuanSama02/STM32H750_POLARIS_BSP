#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ����嶨ʱ������
 * @version:
 * 2022/08/18: ʵ���˱����ǿ����嶨ʱ������,��ʱ���ж�����,����ʵ����Ƶ����������
 */

extern TIM_HandleTypeDef TIM3_Handler;  //��ʱ��3���
extern TIM_HandleTypeDef TIM6_Handler;  //��ʱ��6���

void tim3_init(u16 arr, u16 psc);
void tim6_init(u16 arr, u16 psc);

void tim3_start(void);
void tim3_stop(void);

u32 get_ticknum(void);

#endif
