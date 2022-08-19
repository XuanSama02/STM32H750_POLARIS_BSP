#ifndef __LED_H
#define __LED_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ�����LED����
 * @version:
 * 1.0��ʵ���˱����ǿ�����LED����
 */

/*
LED0 -> PB1 ��ɫ
LED1 -> PB0 ��ɫ
*/

#define LED_GPIO_PORT GPIOB       //LED�˿�
#define LED0_PIN      GPIO_PIN_1  //LED0����
#define LED1_PIN      GPIO_PIN_0  //LED1����

#define LED0(n)		(n ? HAL_GPIO_WritePin(LED_GPIO_PORT, LED0_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED_GPIO_PORT, LED0_PIN, GPIO_PIN_RESET))
#define LED0_Toggle (HAL_GPIO_TogglePin(LED_GPIO_PORT, LED0_PIN))
#define LED1(n)		(n ? HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET))
#define LED1_Toggle (HAL_GPIO_TogglePin(LED_GPIO_PORT, LED1_PIN))

void led_init(void);  //LED��ʼ������

#endif
