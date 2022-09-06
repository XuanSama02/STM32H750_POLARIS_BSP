#ifndef __DELAY_H
#define __DELAY_H

#include <sys.h>

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ�����delay����ʵ��
 * @version:
 * 2022/08/18: ʵ���˱����ǿ����������΢��������ʱ������֧��UCOSII��UCOSIII��FreeRTOS��Ҫ���н�һ����ֲ
 *             û��ʹ��HAL_Delay,����ж����ȼ����ò�����,HAL_Delay���׽�����ѭ��
 * 
 * 2022/09/05: �Ƴ�һ����OS�йصĴ���,�������ʹ��
 */

void delay_init(u16 sysclk);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
