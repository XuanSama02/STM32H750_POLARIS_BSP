#ifndef __LED_H
#define __LED_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ�����LED����
 * @version:
 * 2022/08/18: ʵ���˱����ǿ�����LED����
 * 
 * 2022/08/29: �޸���LED�궨�庯��,�߼�1����
 * 
 * 2022/09/06: ʹ��do while(0)��д�궨��
 */

/*
LED0 -> PB1 ��ɫ
LED1 -> PB0 ��ɫ
*/

#define LED_GPIO_PORT GPIOB       //LED�˿�
#define LED0_PIN      GPIO_PIN_1  //LED0����
#define LED1_PIN      GPIO_PIN_0  //LED1����

#define LED0(n)       do{ n ? HAL_GPIO_WritePin(LED_GPIO_PORT, LED0_PIN, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LED_GPIO_PORT, LED0_PIN, GPIO_PIN_SET); }while(0)
#define LED0_Toggle() do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, LED0_PIN); }while(0)
#define LED1(n)       do{ n ? HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET); }while(0)
#define LED1_Toggle() do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, LED1_PIN); }while(0)

void led_init(void);  //LED��ʼ������

#endif
