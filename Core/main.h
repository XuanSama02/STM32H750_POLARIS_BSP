#ifndef __MAIN_H
#define __MAIN_H

#include "stm32h7xx_hal.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/16
 * @brif:    �����ǿ�����������ͷ�ļ���
 * @version:
 * 2022/08/16: �����˱����ǿ�����ͷ�ļ�
 * 
 * 2022/08/29: ������ʱ����ͼ
 */

/***************************************400MHz***************************************/
/*
ʱ��������(400MHz):
HSE(25MHz) ->|-> PLL1M=5  -> PLL1N=160 ->|-> PLL1P=2  -> 400MHz -> SYSCLK(����ʱ����)
             |                           |-> PLL1Q=4  -> 200MHz -> QSPI,SPI2,SDMMC
             |                           |-> PLL1R=x  -> xxxMHz -> ����ʹ��
             |
             |-> PLL2M=25 -> PLL2N=X1  ->|-> PLL2P=X2 -> fffMHz -> SAI1
             |                           |-> PLL2Q=x  -> xxxMHz -> ����ʹ��(PLL2N��ȷ��)
             |                           |-> PLL2R=x  -> xxxMHz -> ����ʹ��(PLL2N��ȷ��)
             |
             |-> PLL2M=5  -> PLL2N=160 ->|-> PLL3P=2  -> 400MHz -> ����ʹ��
                                         |-> PLL3Q=2  -> 400MHz -> ����ʹ��
                                         |-> PLL3R=X3 -> fffMHz -> LTDC
xxx: δ����
fff: ��ȷ��

X1: SAI1��ʼ����������
X2: SAI1��ʼ����������
X3: LTDC��ʼ����������

note: ��PLL2N�Ĳ�����SAI1�Ĳ��������ú���ȷ��,����PLL2���ܹ�SAI1ʹ��
      ��������ʹ�õ�: PLL1R, PLL3P, PLL3Q
*/

/*
����ʱ��������(400MHz):
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

note: ����TIMƵ��Ϊ200MHz
*/
/***************************************400MHz***************************************/



/***************************************480MHz***************************************/
/*
ʱ��������(480MHz):
HSE(25MHz) ->|-> PLL1M=5  -> PLL1N=192 ->|-> PLL1P=2  -> 480MHz -> SYSCLK(����ʱ����)
             |                           |-> PLL1Q=4  -> 240MHz -> QSPI,SPI2,SDMMC
             |                           |-> PLL1R=x  -> xxxMHz -> ����ʹ��
             |
             |-> PLL2M=25 -> PLL2N=X1  ->|-> PLL2P=X2 -> fffMHz -> SAI1
             |                           |-> PLL2Q=x  -> xxxMHz -> ����ʹ��(PLL2N��ȷ��)
             |                           |-> PLL2R=x  -> xxxMHz -> ����ʹ��(PLL2N��ȷ��)
             |
             |-> PLL2M=5  -> PLL2N=160 ->|-> PLL3P=2  -> 400MHz -> ����ʹ��
                                         |-> PLL3Q=2  -> 400MHz -> ����ʹ��
                                         |-> PLL3R=X3 -> fffMHz -> LTDC
xxx: δ����
fff: ��ȷ��

X1: SAI1��ʼ����������
X2: SAI1��ʼ����������
X3: LTDC��ʼ����������

note: ��PLL2N�Ĳ�����SAI1�Ĳ��������ú���ȷ��,����PLL2���ܹ�SAI1ʹ��
      ��������ʹ�õ�: PLL1R, PLL3P, PLL3Q
*/

/*
����ʱ��������(480MHz):
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

note: ����TIMƵ��Ϊ240MHz
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
