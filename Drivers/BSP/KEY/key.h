#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ����尴������
 * @version:
 * 1.0��ʵ���˱����ǿ����尴������
 */

/*
GND -> KEY0 -> PH3
GND -> KEY1 -> PH2
GND -> KEY2 -> PC13
VCC -> WKUP -> PA0
*/

#define KEY0_GPIO_PORT GPIOH        //KEY0�˿�
#define KEY0_PIN       GPIO_PIN_3   //KEY0����

#define KEY1_GPIO_PORT GPIOH        //KEY1�˿�
#define KEY1_PIN       GPIO_PIN_2   //KEY1����

#define KEY2_GPIO_PORT GPIOC        //KEY2�˿�
#define KEY2_PIN       GPIO_PIN_13  //KEY2����

#define WKUP_GPIO_PORT GPIOA        //WKUP�˿�
#define WKUP_PIN       GPIO_PIN_0   //WKUP����

#define KEY0_PRES 1  //KEY0���·���ֵ
#define KEY1_PRES 2  //KEY1���·���ֵ
#define KEY2_PRES 3  //KEY2���·���ֵ
#define WKUP_PRES 4  //WKUP���·���ֵ

#define KEY0_READ HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_PIN)  //KEY0����PH3
#define KEY1_READ HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_PIN)  //KEY1����PH2
#define KEY2_READ HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_PIN)  //KEY2����PC13
#define WKUP_READ HAL_GPIO_ReadPin(WKUP_GPIO_PORT, WKUP_PIN)  //WKUP����PA0

void key_init(void);   //����IO��ʼ������
u8 key_scan(u8 mode);  //����ɨ�躯��

#endif
