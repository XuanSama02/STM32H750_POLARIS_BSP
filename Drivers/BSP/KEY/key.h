#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板按键驱动
 * @version:
 * 1.0：实现了北极星开发板按键驱动
 */

/*
GND -> KEY0 -> PH3
GND -> KEY1 -> PH2
GND -> KEY2 -> PC13
VCC -> WKUP -> PA0
*/

#define KEY0_GPIO_PORT GPIOH        //KEY0端口
#define KEY0_PIN       GPIO_PIN_3   //KEY0引脚

#define KEY1_GPIO_PORT GPIOH        //KEY1端口
#define KEY1_PIN       GPIO_PIN_2   //KEY1引脚

#define KEY2_GPIO_PORT GPIOC        //KEY2端口
#define KEY2_PIN       GPIO_PIN_13  //KEY2引脚

#define WKUP_GPIO_PORT GPIOA        //WKUP端口
#define WKUP_PIN       GPIO_PIN_0   //WKUP引脚

#define KEY0_PRES 1  //KEY0按下返回值
#define KEY1_PRES 2  //KEY1按下返回值
#define KEY2_PRES 3  //KEY2按下返回值
#define WKUP_PRES 4  //WKUP按下返回值

#define KEY0_READ HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_PIN)  //KEY0按键PH3
#define KEY1_READ HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_PIN)  //KEY1按键PH2
#define KEY2_READ HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_PIN)  //KEY2按键PC13
#define WKUP_READ HAL_GPIO_ReadPin(WKUP_GPIO_PORT, WKUP_PIN)  //WKUP按键PA0

void key_init(void);   //按键IO初始化函数
u8 key_scan(u8 mode);  //按键扫描函数

#endif
