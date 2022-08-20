#ifndef __MYIIC_H
#define __MYIIC_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板软件I2C驱动
 * @version:
 * 1.0：实现了北极星开发板软件I2C驱动
 */

//IO方向设置

#define SDA_IN()  {GPIOH->MODER &= ~(3<<(5*2)); GPIOH->MODER |= 0<<5*2;}  //PH5输入模式
#define SDA_OUT() {GPIOH->MODER &= ~(3<<(5*2)); GPIOH->MODER |= 1<<5*2;}  //PH5输出模式

//IO操作

#define IIC_SCL(n)  (n ? HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_RESET))  //SCL
#define IIC_SDA(n)  (n ? HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_RESET))  //SDA

#define READ_SDA HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_5)  //输入SDA

//IIC所有操作函数

void i2c_init(void);                  //初始化IIC的IO口
void i2c_start(void);                 //发送IIC开始信号
void i2c_stop(void);                  //发送IIC停止信号
void i2c_send_byte(u8 txd);           //IIC发送一个字节
u8 i2c_read_byte(unsigned char ack);  //IIC读取一个字节
u8 i2c_wait_ack(void);                //IIC等待ACK信号
void i2c_ack(void);                   //IIC发送ACK信号
void i2c_nack(void);                  //IIC不发送ACK信号

//I2C通用操作函数(尚未定义)

void i2c_write_one_byte(u8 daddr, u8 addr, u8 data);
u8 i2c_read_one_byte(u8 daddr, u8 addr);

#endif
