#ifndef __LED_H
#define __LED_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板LED驱动
 * @version:
 * 2022/08/18: 实现了北极星开发板LED驱动
 * 
 * 2022/08/29: 修改了LED宏定义函数,逻辑1点亮
 */

/*
LED0 -> PB1 红色
LED1 -> PB0 绿色
*/

#define LED_GPIO_PORT GPIOB       //LED端口
#define LED0_PIN      GPIO_PIN_1  //LED0引脚
#define LED1_PIN      GPIO_PIN_0  //LED1引脚

#define LED0(n)     (n ? HAL_GPIO_WritePin(LED_GPIO_PORT, LED0_PIN, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LED_GPIO_PORT, LED0_PIN, GPIO_PIN_SET))
#define LED0_Toggle (HAL_GPIO_TogglePin(LED_GPIO_PORT, LED0_PIN))
#define LED1(n)     (n ? HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET) : HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET))
#define LED1_Toggle (HAL_GPIO_TogglePin(LED_GPIO_PORT, LED1_PIN))

void led_init(void);  //LED初始化函数

#endif
