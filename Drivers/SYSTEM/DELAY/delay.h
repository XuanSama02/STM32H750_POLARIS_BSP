#ifndef __DELAY_H
#define __DELAY_H
#include <sys.h>

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板delay函数实现
 * @version:
 * 1.0：实现了北极星开发板毫秒与微秒两种延时函数，支持UCOSII与UCOSIII，FreeRTOS需要进行进一步移植
 *      没有使用HAL_Delay,如果中断优先级配置不合适,HAL_Delay容易进入死循环
 */

void delay_init(u16 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
