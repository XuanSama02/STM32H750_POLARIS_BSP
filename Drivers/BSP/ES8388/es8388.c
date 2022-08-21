#include "es8388.h"
#include "myiic.h"
#include "delay.h"
#include "pcf8574.h"

/**
 * @brief ES8388��ʼ��
 * 
 * @return u8 0:�ɹ� ����:�������
 */
u8 es8388_init(void)
{
    i2c_init();  //��ʼ��I2C�ӿ�
    pcf8574_write_bit(MD_PD, 0);  //�򿪹���
    //��λES8388
    es8388_write_reg(0, 0x80);
    es8388_write_reg(0, 0x00);
    delay_ms(100);  //�ȴ���λ
    //��ʼ��ES8388
    es8388_write_reg(0x01, 0x58);
    es8388_write_reg(0x01, 0x50);
    es8388_write_reg(0x02, 0xF3);
    es8388_write_reg(0x02, 0xF0);
    //����ES8388
    es8388_write_reg(0x03, 0x09);  //��˷�ƫ�õ�Դ�ر�
    es8388_write_reg(0x00, 0x06);  //ʹ�ܲο� 500K����ʹ��
    es8388_write_reg(0x04, 0x00);  //DAC��Դ����,�����κ�ͨ��
    es8388_write_reg(0x08, 0x00);  //MCLK����Ƶ
    es8388_write_reg(0x2B, 0x80);  //DAC���� DACLRC��ADCLRC��ͬ
    //����ADC
    es8388_write_reg(0x09, 0x88);  //ADC L/R PGA��������Ϊ+24dB
    es8388_write_reg(0x0C, 0x4C);  //ADC ����ѡ��Ϊleft data = left ADC, right data = left ADC ��Ƶ����Ϊ16bit
    es8388_write_reg(0x0D, 0x02);  //ADC���� MCLK/������=256
    es8388_write_reg(0x10, 0x00);  //ADC�����������ƽ��ź�˥�� L ����Ϊ��С
    es8388_write_reg(0x11, 0x00);  //ADC�����������ƽ��ź�˥�� R ����Ϊ��С
    //����DAC
    es8388_write_reg(0x17, 0x18);  //DAC ��Ƶ����Ϊ16bit
    es8388_write_reg(0x18, 0x02);  //DAC ���� MCLK/������=256
    es8388_write_reg(0x1A, 0x00);  //DAC�����������ƽ��ź�˥�� L ����Ϊ��С
    es8388_write_reg(0x1B, 0x00);  //DAC�����������ƽ��ź�˥�� R ����Ϊ��С
    es8388_write_reg(0x27, 0xB8);  //L��Ƶ��
    es8388_write_reg(0x2A, 0xB8);  //R��Ƶ��
    delay_ms(100);
    return 0;
}

/**
 * @brief ES8388д�Ĵ���
 * 
 * @param addr �Ĵ�����ַ
 * @param txd  �Ĵ���ֵ
 * @return u8 0:�ɹ� ����:�������
 */
u8 es8388_write_reg(u8 addr, u8 txd)
{
    i2c_start();
    i2c_send_byte((ES8388_ADDR<<1)|0); 	//����������ַ+д����
    if(i2c_wait_ack())  //�ȴ�Ӧ��
        return 1;       //1:�豸��Ӧ��
    i2c_send_byte(addr);        //д�Ĵ�����ַ
    if(i2c_wait_ack())  //�ȴ�Ӧ��
        return 2;       //2:�Ĵ�����ַ��Ӧ��
    i2c_send_byte(txd & 0XFF);  //��������
    if(i2c_wait_ack())  //�ȴ�Ӧ��
        return 3;       //3:����д����Ӧ��
    i2c_stop();
    return 0;
}

/**
 * @brief ES8388���Ĵ���
 * 
 * @param addr �Ĵ�����ַ
 * @return u8 ����������
 */
u8 es8388_read_reg(u8 addr)
{
    u8 temp = 0;
    i2c_start();
    i2c_send_byte((ES8388_ADDR<<1)|0);  //����������ַ+д����
    if(i2c_wait_ack())  //�ȴ�Ӧ��
        return 1;       //1:��Ӧ��
    i2c_send_byte(addr);                //д�Ĵ�����ַ
    if(i2c_wait_ack())  //�ȴ�Ӧ��
        return 1;       //1:��Ӧ��
    i2c_start();
    i2c_send_byte((ES8388_ADDR<<1)|1);  //����������ַ+������
    if(i2c_wait_ack())  //�ȴ�Ӧ��
        return 1;       //1:��Ӧ��
    temp = i2c_read_byte(0);            //��ȡ����
    i2c_stop();
    return temp;                        //���ض�ȡ������
}

/**
 * @brief ����I2C����ģʽ
 * 
 * @param fmt 0:�����ֱ�׼I2S 1:MSB(�����) 2:LSB(�Ҷ���) 3:PCM/DSP
 * @param len 0:24bit 1:20bit 2:18bit 3:16bit 4:32bit
 */
void es8388_i2s_config(u8 fmt, u8 len)
{
    fmt &= 0X03;
    len &= 0X07;  //�޶���Χ
    es8388_write_reg(23, (fmt << 1)|(len << 3));  //Reg23,ES8388����ģʽ����
}

/**
 * @brief ���ö�������
 * 
 * @param volume ����(0~30)
 */
void es8388_headphone_vol_config(u8 volume)
{
    u8 i=0;
    if(volume > 30)
        volume = 30;
    for(i=0; i<volume; i++)
    {
        es8388_write_reg(0x2E, i);
        es8388_write_reg(0x2F, i);
    }
}

/**
 * @brief ��������������
 * 
 * @param volume ����(0~30)
 */
void es8388_speaker_vol_config(u8 volume)
{
    u8 i = 0;
    if(volume > 30)
        volume = 30;
    for(i=0; i<volume; i++)
    {
        es8388_write_reg(0x30, i);
        es8388_write_reg(0x31, i);
    }
}

/**
 * @brief ����3D������
 * 
 * @param depth ǿ��(0~7)
 */
void es8388_3d_config(u8 depth)
{ 
    depth &= 0X7;  //�޶���Χ 
    es8388_write_reg(0X1D, depth<<2);  //Reg7,3D��������
}

/**
 * @brief ES8388 ADC/DAC����
 * 
 * @param dac_en DACʹ�� 1:���� 0:�ر�
 * @param adc_en ADCʹ�� 1:���� 0:�ر�
 */
void es8388_adda_config(u8 dac_en, u8 adc_en)
{
    u8 tempreg = 0;
    tempreg |= ((!dac_en)<<0);
    tempreg |= ((!adc_en)<<1);
    tempreg |= ((!dac_en)<<2);
    tempreg |= ((!adc_en)<<3);
    es8388_write_reg(0x02, tempreg);
}

/**
 * @brief ES8388 DAC���ͨ������
 * 
 * @param o1_en ͨ��1 1:���� 0:�ر�
 * @param o2_en ͨ��2 1:���� 0:�ر�
 */
void es8388_output_config(u8 o1_en, u8 o2_en)
{
    u8 tempreg = 0;
    tempreg |= o1_en*(3<<4);
    tempreg |= o2_en*(3<<2);
    es8388_write_reg(0x04, tempreg);
}

/**
 * @brief ES8388 MIC��������(PGA����)
 * 
 * @param gain 0~8 ��Ӧ 0~24dB
 */
void es8388_mic_gain_config(u8 gain)
{
    gain &= 0X0F;
    gain |= gain<<4;
    es8388_write_reg(0X09, gain);  //Reg9,����ͨ��PGA��������
}

/**
 * @brief ES8388 ALC����
 * 
 * @param sel      0:�ر�ALC 1:��ͨ��ALC 2:��ͨ��ALC 3:������ALC
 * @param gain_max ������� 0~7 ��Ӧ -6.5dB ~ +35.5dB
 * @param gain_min ��С���� 0~7 ��Ӧ  -12dB ~ +30dB
 */
void es8388_alc_control(u8 sel, u8 gain_max, u8 gain_min)
{
    u8 tempreg = 0;
    tempreg  = sel<<6;
    tempreg |= (gain_max&0X07)<<3;
    tempreg |= gain_min&0X07;
    es8388_write_reg(0X12, tempreg);  //Reg18,ALC����
}

/**
 * @brief ES8388 ADC���ͨ������
 * 
 * @param in 0:ͨ��1���� 1:ͨ��2����
 */
void es8388_input_config(u8 in)
{
    es8388_write_reg(0x0A, (5*in)<<4);  //ADC1 ����ͨ��ѡ��L/R INPUT1
}
