#include "24cxx.h"
#include "delay.h"

/**
 * @brief ��ʼ��I2C
 * 
 */
void at24cxx_init(void)
{
	i2c_init();  //I2C��ʼ��
}

/**
 * @brief ���AT24C02�Ƿ�����,����ʹ�������һ����ַ��Ϊ��־λ
 * 
 * @return u8 1:���ʧ�� 0:���ɹ�
 */
u8 at24cxx_check(void)
{
	u8 temp;
	temp = ac24cxx_read_one_byte(EEPROM_TYPE);  //����ÿ�ο�����д��
	if(temp == 0X55)  //��ȡ��־λΪ0x55,���ɹ�
		return 0;
	else              //���ʧ��,����д��,�ų��״γ�ʼ��
	{
		at24cxx_write_one_byte(EEPROM_TYPE, 0X55);  //��־λд��0x55
	    temp = ac24cxx_read_one_byte(EEPROM_TYPE);  //��־λ��ȡ
		if(temp == 0X55)  //��ȡ��־λΪ0x55,���ɹ�
			return 0;
	}
	return 1;  //���ʧ��
}

/**
 * @brief ��AT24Cxxָ����ַ��ȡһ�ֽ�����
 * 
 * @param addr ��ȡ��ַ
 * @return u8  ����
 */
u8 ac24cxx_read_one_byte(u16 addr)
{
	u8 temp = 0;
    i2c_start();
	if(EEPROM_TYPE > AT24C16)
	{
		i2c_send_byte(0XA0);  //����д����
		i2c_wait_ack();
		i2c_send_byte(addr>>8);  //���͸ߵ�ַ
	}
	else  //EEPROM_TYPE <= AT24C16
		i2c_send_byte(0XA0 + ((addr/256)<<1));  //����������ַ0XA0,д����
	i2c_wait_ack();
    i2c_send_byte(addr%256);  //���͵͵�ַ
	i2c_wait_ack();
	i2c_start();
	i2c_send_byte(0XA1);  //�������ģʽ
	i2c_wait_ack();
    temp = i2c_read_byte(0);
    i2c_stop();  //����һ��ֹͣ����
	return temp;
}

/**
 * @brief ��AT24Cxx�����ָ����ַ��ʼ��������Ϊlen������
 * 
 * @param addr ��ʼ�����ĵ�ַ
 * @param len  Ҫ�������ݵĳ���2,4
 * @return u32 ����
 */
u32 at24cxx_read_len_byte(u16 addr, u8 len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0; t<len; t++)  //���ζ�ȡlen���ȵ�����
	{
		temp <<= 8;
		temp  += ac24cxx_read_one_byte(addr+len-t-1);
	}
	return temp;
}

/**
 * @brief ��AT24Cxxָ����ַд��һ������
 * 
 * @param addr д�����ݵ�Ŀ�ĵ�ַ
 * @param data Ҫд�������
 */
void at24cxx_write_one_byte(u16 addr, u8 data)
{
    i2c_start();
	if(EEPROM_TYPE > AT24C16)
	{
		i2c_send_byte(0XA0);     //����д����
		i2c_wait_ack();
		i2c_send_byte(addr>>8);  //���͸ߵ�ַ
	}
	else  //EEPROM_TYPE <= AT24C16
		i2c_send_byte(0XA0 + ((addr/256)<<1));  //����������ַ0XA0,д����
	i2c_wait_ack();
    i2c_send_byte(addr%256);     //���͵͵�ַ
	i2c_wait_ack();
	i2c_send_byte(data);         //�����ֽ�
	i2c_wait_ack();
    i2c_stop();                  //����һ��ֹͣ����
	delay_ms(10);
}

/**
 * @brief ��AT24Cxx�����ָ����ַ��ʼд�볤��Ϊlen������,����д��16bit����32bit������
 * 
 * @param addr ��ʼд��ĵ�ַ
 * @param data ���������׵�ַ
 * @param len  Ҫд�����ݵĳ���2,4
 */
void at24cxx_write_len_byte(u16 addr, u32 data, u8 len)
{  	
	u8 t;
	for(t=0; t<len; t++)
		at24cxx_write_one_byte(addr+t, (data>>(8*t))&0xff);
}

/**
 * @brief ��AT24Cxx�����ָ����ַ��ʼд��ָ������������
 * 
 * @param addr    ��ʼд��ĵ�ַ,��24c02Ϊ0~255
 * @param pbuffer ���������׵�ַ
 * @param num     Ҫд�����ݵĸ���
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
 * @brief ��AT24Cxx�����ָ����ַ��ʼ����ָ������������
 * 
 * @param addr    ��ʼ�����ĵ�ַ,��24c02Ϊ0~255
 * @param pbuffer ���������׵�ַ
 * @param num     Ҫ�������ݵĸ���
 */
void at24cxx_read(u16 addr, u8 *pbuffer, u16 num)
{
	while(num)
	{
		*pbuffer++=ac24cxx_read_one_byte(addr++);
		num--;
	}
}
