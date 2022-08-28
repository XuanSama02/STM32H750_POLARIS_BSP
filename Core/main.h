#ifndef __MAIN_H
#define __MAIN_H

#include "stm32h7xx_hal.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/16
 * @brif:    北极星开发板主函数头文件库
 * @version:
 * 2022/08/16: 包含了北极星开发板头文件
 * 
 * 2022/08/29: 绘制了时钟树图
 */

/***************************************400MHz***************************************/
/*
时钟树配置(400MHz):
HSE(25MHz) ->|-> PLL1M=5  -> PLL1N=160 ->|-> PLL1P=2  -> 400MHz -> SYSCLK(核心时钟树)
             |                           |-> PLL1Q=4  -> 200MHz -> QSPI,SPI2,SDMMC
             |                           |-> PLL1R=x  -> xxxMHz -> 可以使用
             |
             |-> PLL2M=25 -> PLL2N=X1  ->|-> PLL2P=X2 -> fffMHz -> SAI1
             |                           |-> PLL2Q=x  -> xxxMHz -> 不能使用(PLL2N不确定)
             |                           |-> PLL2R=x  -> xxxMHz -> 不能使用(PLL2N不确定)
             |
             |-> PLL2M=5  -> PLL2N=160 ->|-> PLL3P=2  -> 400MHz -> 可以使用
                                         |-> PLL3Q=2  -> 400MHz -> 可以使用
                                         |-> PLL3R=X3 -> fffMHz -> LTDC
xxx: 未启用
fff: 不确定

X1: SAI1初始化函数决定
X2: SAI1初始化函数决定
X3: LTDC初始化函数决定

note: 因PLL2N的参数由SAI1的采样率设置函数确定,所以PLL2仅能供SAI1使用
      其他还能使用的: PLL1R, PLL3P, PLL3Q
*/

/*
核心时钟树配置(400MHz):
SYSCLK(400MHz) ->|-> 400MHz -> CPU Clock
                 |-> 400MHz -> CPU Systick
                 |-> 200MHz -> AXI 
                 |-> 200MHz -> HCLK3
                 |
                 |-> 100MHz -> APB1
                 |-> 200MHz -> APB1 TIM
                 |-> 100MHz -> APB2
                 |-> 200MHz -> APB2 TIM
                 |-> 100MHz -> APB3
                 |->  50MHz -> APB4
                 |
                 |-> 200MHz -> AHB1,2
                 |-> 200MHz -> ABB4

note: 所有TIM频率为200MHz
*/
/***************************************400MHz***************************************/



/***************************************480MHz***************************************/
/*
时钟树配置(480MHz):
HSE(25MHz) ->|-> PLL1M=5  -> PLL1N=192 ->|-> PLL1P=2  -> 480MHz -> SYSCLK(核心时钟树)
             |                           |-> PLL1Q=4  -> 240MHz -> QSPI,SPI2,SDMMC
             |                           |-> PLL1R=x  -> xxxMHz -> 可以使用
             |
             |-> PLL2M=25 -> PLL2N=X1  ->|-> PLL2P=X2 -> fffMHz -> SAI1
             |                           |-> PLL2Q=x  -> xxxMHz -> 不能使用(PLL2N不确定)
             |                           |-> PLL2R=x  -> xxxMHz -> 不能使用(PLL2N不确定)
             |
             |-> PLL2M=5  -> PLL2N=160 ->|-> PLL3P=2  -> 400MHz -> 可以使用
                                         |-> PLL3Q=2  -> 400MHz -> 可以使用
                                         |-> PLL3R=X3 -> fffMHz -> LTDC
xxx: 未启用
fff: 不确定

X1: SAI1初始化函数决定
X2: SAI1初始化函数决定
X3: LTDC初始化函数决定

note: 因PLL2N的参数由SAI1的采样率设置函数确定,所以PLL2仅能供SAI1使用
      其他还能使用的: PLL1R, PLL3P, PLL3Q
*/

/*
核心时钟树配置(480MHz):
SYSCLK(480MHz) ->|-> 480MHz -> CPU Clock
                 |-> 480MHz -> CPU Systick
                 |-> 240MHz -> AXI 
                 |-> 240MHz -> HCLK3
                 |
                 |-> 120MHz -> APB1
                 |-> 240MHz -> APB1 TIM
                 |-> 120MHz -> APB2
                 |-> 240MHz -> APB2 TIM
                 |-> 120MHz -> APB3
                 |->  60MHz -> APB4
                 |
                 |-> 240MHz -> AHB1,2
                 |-> 240MHz -> ABB4

note: 所有TIM频率为240MHz
*/
/***************************************480MHz***************************************/

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "ltdc.h"
#include "lcd.h"
#include "sdram.h"
#include "usmart.h"
#include "pcf8574.h"
#include "mpu.h"
#include "malloc.h"
#include "w25qxx.h"
#include "sdmmc_sdcard.h"
//#include "nand.h"
//#include "ftl.h"
#include "ff.h"
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"
#include "es8388.h"
#include "timer.h"
#include "jpegcodec.h"
#include "audioplay.h"
#include "videoplayer.h"
#include "string.h"

#endif
