#include "24cxx.h"
#include "delay.h"

/**
 * @brief 初始化I2C
 * 
 */
void at24cxx_init(void)
{
	i2c_init();  //I2C初始化
}

/**
 * @brief 检查AT24C02是否正常,这里使用了最后一个地址作为标志位
 * 
 * @return u8 1:检测失败 0:检测成功
 */
u8 at24cxx_check(void)
{
	u8 temp;
	temp = ac24cxx_read_one_byte(EEPROM_TYPE);  //避免每次开机都写入
	if(temp == 0X55)  //读取标志位为0x55,检查成功
		return 0;
	else              //检查失败,尝试写入,排除首次初始化
	{
		at24cxx_write_one_byte(EEPROM_TYPE, 0X55);  //标志位写入0x55
	    temp = ac24cxx_read_one_byte(EEPROM_TYPE);  //标志位读取
		if(temp == 0X55)  //读取标志位为0x55,检查成功
			return 0;
	}
	return 1;  //检查失败
}

/**
 * @brief 在AT24Cxx指定地址读取一字节数据
 * 
 * @param addr 读取地址
 * @return u8  数据
 */
u8 ac24cxx_read_one_byte(u16 addr)
{
	u8 temp = 0;
    i2c_start();
	if(EEPROM_TYPE > AT24C16)
	{
		i2c_send_byte(0XA0);  //发送写命令
		i2c_wait_ack();
		i2c_send_byte(addr>>8);  //发送高地址
	}
	else  //EEPROM_TYPE <= AT24C16
		i2c_send_byte(0XA0 + ((addr/256)<<1));  //发送器件地址0XA0,写数据
	i2c_wait_ack();
    i2c_send_byte(addr%256);  //发送低地址
	i2c_wait_ack();
	i2c_start();
	i2c_send_byte(0XA1);  //进入接收模式
	i2c_wait_ack();
    temp = i2c_read_byte(0);
    i2c_stop();  //产生一个停止条件
	return temp;
}

/**
 * @brief 在AT24Cxx里面的指定地址开始读出长度为len的数据
 * 
 * @param addr 开始读出的地址
 * @param len  要读出数据的长度2,4
 * @return u32 数据
 */
u32 at24cxx_read_len_byte(u16 addr, u8 len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0; t<len; t++)  //依次读取len长度的数据
	{
		temp <<= 8;
		temp  += ac24cxx_read_one_byte(addr+len-t-1);
	}
	return temp;
}

/**
 * @brief 在AT24Cxx指定地址写入一个数据
 * 
 * @param addr 写入数据的目的地址
 * @param data 要写入的数据
 */
void at24cxx_write_one_byte(u16 addr, u8 data)
{
    i2c_start();
	if(EEPROM_TYPE > AT24C16)
	{
		i2c_send_byte(0XA0);     //发送写命令
		i2c_wait_ack();
		i2c_send_byte(addr>>8);  //发送高地址
	}
	else  //EEPROM_TYPE <= AT24C16
		i2c_send_byte(0XA0 + ((addr/256)<<1));  //发送器件地址0XA0,写数据
	i2c_wait_ack();
    i2c_send_byte(addr%256);     //发送低地址
	i2c_wait_ack();
	i2c_send_byte(data);         //发送字节
	i2c_wait_ack();
    i2c_stop();                  //产生一个停止条件
	delay_ms(10);
}

/**
 * @brief 在AT24Cxx里面的指定地址开始写入长度为len的数据,用于写入16bit或者32bit的数据
 * 
 * @param addr 开始写入的地址
 * @param data 数据数组首地址
 * @param len  要写入数据的长度2,4
 */
void at24cxx_write_len_byte(u16 addr, u32 data, u8 len)
{  	
	u8 t;
	for(t=0; t<len; t++)
		at24cxx_write_one_byte(addr+t, (data>>(8*t))&0xff);
}

/**
 * @brief 在AT24Cxx里面的指定地址开始写入指定个数的数据
 * 
 * @param addr    开始写入的地址,对24c02为0~255
 * @param pbuffer 数据数组首地址
 * @param num     要写入数据的个数
 */
void at24cxx_write(u16 addr, u8 *pbuffer, u16 num)
{
	while(num--)
	{
		at24cxx_write_one_byte(addr, *pbuffer);
		addr++;
		pbuffer++;
	}
}

/**
 * @brief 在AT24Cxx里面的指定地址开始读出指定个数的数据
 * 
 * @param addr    开始读出的地址,对24c02为0~255
 * @param pbuffer 数据数组首地址
 * @param num     要读出数据的个数
 */
void at24cxx_read(u16 addr, u8 *pbuffer, u16 num)
{
	while(num)
	{
		*pbuffer++=ac24cxx_read_one_byte(addr++);
		num--;
	}
}
