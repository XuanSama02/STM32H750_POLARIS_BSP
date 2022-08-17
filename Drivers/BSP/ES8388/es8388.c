#include "es8388.h"
#include "myiic.h"
#include "delay.h"
#include "pcf8574.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//ES8388��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/4/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//ES8388��ʼ��
//����ֵ:0,��ʼ������
//    ����,�������
u8 ES8388_Init(void)
{
    IIC_Init();                 	//��ʼ��IIC�ӿ�
    PCF8574_WriteBit(MD_PD, 0);		//�򿪹���

    //��λES8388
    ES8388_Write_Reg(0, 0x80);
    ES8388_Write_Reg(0, 0x00);
    delay_ms(100);					//�ȴ���λ

    ES8388_Write_Reg(0x01, 0x58);
    ES8388_Write_Reg(0x01, 0x50);
    ES8388_Write_Reg(0x02, 0xF3);
    ES8388_Write_Reg(0x02, 0xF0);

    ES8388_Write_Reg(0x03, 0x09);	//��˷�ƫ�õ�Դ�ر�
    ES8388_Write_Reg(0x00, 0x06);	//ʹ�ܲο�		500K����ʹ��
    ES8388_Write_Reg(0x04, 0x00);	//DAC��Դ���������κ�ͨ��
    ES8388_Write_Reg(0x08, 0x00);	//MCLK����Ƶ
    ES8388_Write_Reg(0x2B, 0x80);	//DAC����	DACLRC��ADCLRC��ͬ

    ES8388_Write_Reg(0x09, 0x88);	//ADC L/R PGA��������Ϊ+24dB
    ES8388_Write_Reg(0x0C, 0x4C);	//ADC	����ѡ��Ϊleft data = left ADC, right data = left ADC 	��Ƶ����Ϊ16bit
    ES8388_Write_Reg(0x0D, 0x02);	//ADC���� MCLK/������=256
    ES8388_Write_Reg(0x10, 0x00);	//ADC�����������ƽ��ź�˥�� L	����Ϊ��С������
    ES8388_Write_Reg(0x11, 0x00);	//ADC�����������ƽ��ź�˥�� R	����Ϊ��С������

    ES8388_Write_Reg(0x17, 0x18);	//DAC ��Ƶ����Ϊ16bit
    ES8388_Write_Reg(0x18, 0x02);	//DAC	���� MCLK/������=256
    ES8388_Write_Reg(0x1A, 0x00);	//DAC�����������ƽ��ź�˥�� L	����Ϊ��С������
    ES8388_Write_Reg(0x1B, 0x00);	//DAC�����������ƽ��ź�˥�� R	����Ϊ��С������
    ES8388_Write_Reg(0x27, 0xB8);	//L��Ƶ��
    ES8388_Write_Reg(0x2A, 0xB8);	//R��Ƶ��
    delay_ms(100);
    return 0;
}

//ES8388д�Ĵ���
//reg:�Ĵ�����ַ
//val:Ҫд��Ĵ�����ֵ
//����ֵ:0,�ɹ�;
//    ����,�������
u8 ES8388_Write_Reg(u8 reg, u8 val)
{
    IIC_Start();
    IIC_Send_Byte((ES8388_ADDR << 1)|0); 	//����������ַ+д����
    if(IIC_Wait_Ack())return 1;				//�ȴ�Ӧ��(�ɹ�?/ʧ��?)
    IIC_Send_Byte(reg);						//д�Ĵ�����ַ
    if(IIC_Wait_Ack())return 2;				//�ȴ�Ӧ��(�ɹ�?/ʧ��?)
    IIC_Send_Byte(val & 0XFF);				//��������
    if(IIC_Wait_Ack())return 3;				//�ȴ�Ӧ��(�ɹ�?/ʧ��?)
    IIC_Stop();
    return 0;
}

//ES8388���Ĵ���
//reg:�Ĵ�����ַ
//����ֵ:����������
u8 ES8388_Read_Reg(u8 reg)
{
    u8 temp = 0;

    IIC_Start();
    IIC_Send_Byte((ES8388_ADDR << 1)|0); 	//����������ַ+д����
    if(IIC_Wait_Ack())return 1;				//�ȴ�Ӧ��(�ɹ�?/ʧ��?)
    IIC_Send_Byte(reg);						//д�Ĵ�����ַ
    if(IIC_Wait_Ack())return 1;				//�ȴ�Ӧ��(�ɹ�?/ʧ��?)
    IIC_Start();
    IIC_Send_Byte((ES8388_ADDR << 1)|1); 	//����������ַ+������
    if(IIC_Wait_Ack())return 1;				//�ȴ�Ӧ��(�ɹ�?/ʧ��?)
    temp = IIC_Read_Byte(0);
    IIC_Stop();

    return temp;
}

//����I2S����ģʽ
//fmt:0,�����ֱ�׼I2S;1,MSB(�����);2,LSB(�Ҷ���);3,PCM/DSP
//len:0,24bit;1,20bit;2,18bit;3,16bit;4,32bit
void ES8388_I2S_Cfg(u8 fmt, u8 len)
{
    fmt&=0X03;
    len&=0X07; //�޶���Χ
    ES8388_Write_Reg(23,(fmt << 1)|(len << 3));	//R23,ES8388����ģʽ����
}

//���ö�������
//volume:������С(0-33)
void ES8388_HPvol_Set(u8 volume)
{
    u8 i=0;
    if(volume > 30)
        volume=30;
    for(i=0; i<volume; i++)
    {
        ES8388_Write_Reg(0x2E,i);
        ES8388_Write_Reg(0x2F,i);
    }
}

//������������
//volume:������С(0-33)
void ES8388_SPKvol_Set(u8 volume)
{
    u8 i=0;
    if(volume > 30)
        volume=30;
    for(i=0; i<volume; i++)
    {
        ES8388_Write_Reg(0x30,i);
        ES8388_Write_Reg(0x31,i);
    }
}

//����3D������
//depth:0~7(3Dǿ��,0�ر�,7��ǿ)
void ES8388_3D_Set(u8 depth)
{ 
	depth&=0X7;//�޶���Χ 
 	ES8388_Write_Reg(0X1D,depth<<2);	//R7,3D�������� 	
}

//ES8388 DAC/ADC����
//dacen: dacʹ��(1)/�ر�(0)
//adcen: adcʹ��(1)/�ر�(0)
void ES8388_ADDA_Cfg(u8 dacen,u8 adcen)
{
	u8 tempreg = 0;
	tempreg|=((!dacen)<<0);
	tempreg|=((!adcen)<<1);
	tempreg|=((!dacen)<<2);
	tempreg|=((!adcen)<<3);
	ES8388_Write_Reg(0x02,tempreg);
}

//ES8388 DAC���ͨ������
//o1en: ͨ��1ʹ��(1)/��ֹ(0)
//o2en: ͨ��2ʹ��(1)/��ֹ(0)
void ES8388_Output_Cfg(u8 o1en,u8 o2en)
{
	u8 tempreg=0;
	tempreg|=o1en*(3<<4);
	tempreg|=o2en*(3<<2);	
	ES8388_Write_Reg(0x04,tempreg);
}

//ES8388 MIC��������(MIC PGA����)
//gain:0~8,��Ӧ0~24dB  3dB/Step
void ES8388_MIC_Gain(u8 gain)
{
	gain&=0X0F; 
	gain|=gain<<4;
	ES8388_Write_Reg(0X09,gain);		//R9,����ͨ��PGA��������
}

//ES8388 ALC����
//sel:0,�ر�ALC;1,��ͨ��ALC;2,��ͨ��ALC;3,������ALC
//maxgain:0~7,��Ӧ-6.5~+35.5dB  
//minigain:0~7,��Ӧ-12~+30dB  	6dB/STEP
void ES8388_ALC_Ctrl(u8 sel,u8 maxgain,u8 mingain)
{
	u8 tempreg=0;
	tempreg=sel<<6;
	tempreg|=(maxgain&0X07)<<3;
	tempreg|=mingain&0X07;
	ES8388_Write_Reg(0X12,tempreg);		//R18,ALC����
}

//ES8388 ADC���ͨ������
//in: 0 ͨ��1����; 1 ͨ��2����
void ES8388_Input_Cfg(u8 in)
{
	ES8388_Write_Reg(0x0A,(5*in)<<4);	//ADC1 ����ͨ��ѡ��L/R	INPUT1
}
