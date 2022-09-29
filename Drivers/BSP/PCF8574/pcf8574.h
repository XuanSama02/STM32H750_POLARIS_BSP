#ifndef __PCF8574_H
#define __PCF8574_H

#include "sys.h"
#include "myiic.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/19
 * @brif:    �����ǿ�����PCF8574����
 * @version:
 * 2022/08/19: ʵ���˱����ǿ�����PCF8574��������IO
 * 
 * 2022/09/06: �Ż��궨��,��Ϊ������ʽ
 */

#define PCF8574_INT() HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_10)  //PCF8574 INT����

#define PCF8574_ADDR  0X40  //PCF8574��ַ(������һλ)

//PCF8574����IO�Ĺ���
#define BEEP_IO       0  //��������������      P0
#define AP_INT_IO     1  //AP3216C�ж�����    P1
#define DCMI_PWDN_IO  2  //DCMI�ĵ�Դ��������  P2
#define USB_PWR_IO    3  //USB��Դ��������     P3
#define MD_PD         4  //MD_PD              P4
#define MPU_INT_IO    5  //MPU9250�ж�����     P5
#define RS485_RE_IO   6  //RS485_RE����       P6
#define ETH_RESET_IO  7  //��̫����λ����      P7

u8 pcf8574_init(void);
u8 pcf8574_read_byte(void);
void pcf8574_write_byte(u8 data);
u8 pcf8574_read_bit(u8 bit);
void pcf8574_write_bit(u8 bit, u8 sta);

#endif
