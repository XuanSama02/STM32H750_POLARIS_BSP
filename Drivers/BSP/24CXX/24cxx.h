#ifndef __24CXX_H
#define __24CXX_H

#include "sys.h"
#include "myiic.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板AT24C02驱动
 * @version:
 * 1.0：实现了北极星开发板AT24C02的读写
 */

//AT24Cxx系列EEPROM定义

#define AT24C01  127
#define AT24C02  255
#define AT24C04  511
#define AT24C08  1023
#define AT24C16  2047
#define AT24C32  4095
#define AT24C64  8191
#define AT24C128 16383
#define AT24C256 32767

//北极星开发板板载AT24C02
#define EEPROM_TYPE AT24C02

void at24cxx_init(void);  //初始化I2C
u8 at24cxx_check(void);   //检查器件

u8 ac24cxx_read_one_byte(u16 addr);                       //指定地址读取一个字节
u32 at24cxx_read_len_byte(u16 addr, u8 len);              //指定地址开始读取指定长度数据
void at24cxx_write_one_byte(u16 addr, u8 data);           //指定地址写入一个字节
void at24cxx_write_len_byte(u16 addr, u32 data, u8 len);  //指定地址开始写入指定长度的数据
void at24cxx_write(u16 addr, u8 *bBuffer, u16 num);       //从指定地址开始写入指定长度的数据
void at24cxx_read(u16 addr, u8 *pbuffer, u16 num);        //从指定地址开始读出指定长度的数据

#endif
