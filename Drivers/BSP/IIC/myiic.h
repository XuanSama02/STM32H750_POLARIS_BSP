#ifndef __MYIIC_H
#define __MYIIC_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ��������I2C����
 * @version:
 * 1.0��ʵ���˱����ǿ��������I2C����
 */

//IO��������

#define SDA_IN()  {GPIOH->MODER &= ~(3<<(5*2)); GPIOH->MODER |= 0<<5*2;}  //PH5����ģʽ
#define SDA_OUT() {GPIOH->MODER &= ~(3<<(5*2)); GPIOH->MODER |= 1<<5*2;}  //PH5���ģʽ

//IO����

#define IIC_SCL(n)  (n ? HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_RESET))  //SCL
#define IIC_SDA(n)  (n ? HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_RESET))  //SDA

#define READ_SDA HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_5)  //����SDA

//IIC���в�������

void i2c_init(void);                  //��ʼ��IIC��IO��
void i2c_start(void);                 //����IIC��ʼ�ź�
void i2c_stop(void);                  //����IICֹͣ�ź�
void i2c_send_byte(u8 txd);           //IIC����һ���ֽ�
u8 i2c_read_byte(unsigned char ack);  //IIC��ȡһ���ֽ�
u8 i2c_wait_ack(void);                //IIC�ȴ�ACK�ź�
void i2c_ack(void);                   //IIC����ACK�ź�
void i2c_nack(void);                  //IIC������ACK�ź�

//I2Cͨ�ò�������(��δ����)

void i2c_write_one_byte(u8 daddr, u8 addr, u8 data);
u8 i2c_read_one_byte(u8 daddr, u8 addr);

#endif
