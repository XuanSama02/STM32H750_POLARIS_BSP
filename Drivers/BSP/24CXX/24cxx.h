#ifndef __24CXX_H
#define __24CXX_H

#include "sys.h"
#include "myiic.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ�����AT24C02����
 * @version:
 * 1.0��ʵ���˱����ǿ�����AT24C02�Ķ�д
 */

//AT24Cxxϵ��EEPROM����

#define AT24C01  127
#define AT24C02  255
#define AT24C04  511
#define AT24C08  1023
#define AT24C16  2047
#define AT24C32  4095
#define AT24C64  8191
#define AT24C128 16383
#define AT24C256 32767

//�����ǿ��������AT24C02
#define EEPROM_TYPE AT24C02

void at24cxx_init(void);  //��ʼ��I2C
u8 at24cxx_check(void);   //�������

u8 ac24cxx_read_one_byte(u16 addr);                       //ָ����ַ��ȡһ���ֽ�
u32 at24cxx_read_len_byte(u16 addr, u8 len);              //ָ����ַ��ʼ��ȡָ����������
void at24cxx_write_one_byte(u16 addr, u8 data);           //ָ����ַд��һ���ֽ�
void at24cxx_write_len_byte(u16 addr, u32 data, u8 len);  //ָ����ַ��ʼд��ָ�����ȵ�����
void at24cxx_write(u16 addr, u8 *bBuffer, u16 num);       //��ָ����ַ��ʼд��ָ�����ȵ�����
void at24cxx_read(u16 addr, u8 *pbuffer, u16 num);        //��ָ����ַ��ʼ����ָ�����ȵ�����

#endif
