#ifndef __DELAY_H
#define __DELAY_H
#include <sys.h>

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ�����delay����ʵ��
 * @version:
 * 1.0��ʵ���˱����ǿ����������΢��������ʱ������֧��UCOSII��UCOSIII��FreeRTOS��Ҫ���н�һ����ֲ
 *      û��ʹ��HAL_Delay,����ж����ȼ����ò�����,HAL_Delay���׽�����ѭ��
 */

void delay_init(u16 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
