#ifndef __PCF8574_H
#define __PCF8574_H

#include "sys.h"
#include "myiic.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/19
 * @brif:    北极星开发板PCF8574驱动
 * @version:
 * 2022/08/19: 实现了北极星开发板PCF8574驱动控制IO
 * 
 * 2022/09/06: 优化宏定义,改为函数形式
 */

#define PCF8574_INT() HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_10)  //PCF8574 INT引脚

#define PCF8574_ADDR  0X40  //PCF8574地址(左移了一位)

//PCF8574各个IO的功能
#define BEEP_IO       0  //蜂鸣器控制引脚      P0
#define AP_INT_IO     1  //AP3216C中断引脚    P1
#define DCMI_PWDN_IO  2  //DCMI的电源控制引脚  P2
#define USB_PWR_IO    3  //USB电源控制引脚     P3
#define MD_PD         4  //MD_PD              P4
#define MPU_INT_IO    5  //MPU9250中断引脚     P5
#define RS485_RE_IO   6  //RS485_RE引脚       P6
#define ETH_RESET_IO  7  //以太网复位引脚      P7

u8 pcf8574_init(void);
u8 pcf8574_read_byte(void);
void pcf8574_write_byte(u8 data);
u8 pcf8574_read_bit(u8 bit);
void pcf8574_write_bit(u8 bit, u8 sta);

#endif
