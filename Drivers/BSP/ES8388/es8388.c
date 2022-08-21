#include "es8388.h"
#include "myiic.h"
#include "delay.h"
#include "pcf8574.h"

/**
 * @brief ES8388初始化
 * 
 * @return u8 0:成功 其他:错误代码
 */
u8 es8388_init(void)
{
    i2c_init();  //初始化I2C接口
    pcf8574_write_bit(MD_PD, 0);  //打开功放
    //软复位ES8388
    es8388_write_reg(0, 0x80);
    es8388_write_reg(0, 0x00);
    delay_ms(100);  //等待复位
    //初始化ES8388
    es8388_write_reg(0x01, 0x58);
    es8388_write_reg(0x01, 0x50);
    es8388_write_reg(0x02, 0xF3);
    es8388_write_reg(0x02, 0xF0);
    //配置ES8388
    es8388_write_reg(0x03, 0x09);  //麦克风偏置电源关闭
    es8388_write_reg(0x00, 0x06);  //使能参考 500K驱动使能
    es8388_write_reg(0x04, 0x00);  //DAC电源管理,不打开任何通道
    es8388_write_reg(0x08, 0x00);  //MCLK不分频
    es8388_write_reg(0x2B, 0x80);  //DAC控制 DACLRC与ADCLRC相同
    //配置ADC
    es8388_write_reg(0x09, 0x88);  //ADC L/R PGA增益配置为+24dB
    es8388_write_reg(0x0C, 0x4C);  //ADC 数据选择为left data = left ADC, right data = left ADC 音频数据为16bit
    es8388_write_reg(0x0D, 0x02);  //ADC配置 MCLK/采样率=256
    es8388_write_reg(0x10, 0x00);  //ADC数字音量控制将信号衰减 L 设置为最小
    es8388_write_reg(0x11, 0x00);  //ADC数字音量控制将信号衰减 R 设置为最小
    //配置DAC
    es8388_write_reg(0x17, 0x18);  //DAC 音频数据为16bit
    es8388_write_reg(0x18, 0x02);  //DAC 配置 MCLK/采样率=256
    es8388_write_reg(0x1A, 0x00);  //DAC数字音量控制将信号衰减 L 设置为最小
    es8388_write_reg(0x1B, 0x00);  //DAC数字音量控制将信号衰减 R 设置为最小
    es8388_write_reg(0x27, 0xB8);  //L混频器
    es8388_write_reg(0x2A, 0xB8);  //R混频器
    delay_ms(100);
    return 0;
}

/**
 * @brief ES8388写寄存器
 * 
 * @param addr 寄存器地址
 * @param txd  寄存器值
 * @return u8 0:成功 其他:错误代码
 */
u8 es8388_write_reg(u8 addr, u8 txd)
{
    i2c_start();
    i2c_send_byte((ES8388_ADDR<<1)|0); 	//发送器件地址+写命令
    if(i2c_wait_ack())  //等待应答
        return 1;       //1:设备无应答
    i2c_send_byte(addr);        //写寄存器地址
    if(i2c_wait_ack())  //等待应答
        return 2;       //2:寄存器地址无应答
    i2c_send_byte(txd & 0XFF);  //发送数据
    if(i2c_wait_ack())  //等待应答
        return 3;       //3:数据写入无应答
    i2c_stop();
    return 0;
}

/**
 * @brief ES8388读寄存器
 * 
 * @param addr 寄存器地址
 * @return u8 读到的数据
 */
u8 es8388_read_reg(u8 addr)
{
    u8 temp = 0;
    i2c_start();
    i2c_send_byte((ES8388_ADDR<<1)|0);  //发送器件地址+写命令
    if(i2c_wait_ack())  //等待应答
        return 1;       //1:无应答
    i2c_send_byte(addr);                //写寄存器地址
    if(i2c_wait_ack())  //等待应答
        return 1;       //1:无应答
    i2c_start();
    i2c_send_byte((ES8388_ADDR<<1)|1);  //发送器件地址+读命令
    if(i2c_wait_ack())  //等待应答
        return 1;       //1:无应答
    temp = i2c_read_byte(0);            //读取数据
    i2c_stop();
    return temp;                        //返回读取的数据
}

/**
 * @brief 设置I2C工作模式
 * 
 * @param fmt 0:飞利浦标准I2S 1:MSB(左对齐) 2:LSB(右对齐) 3:PCM/DSP
 * @param len 0:24bit 1:20bit 2:18bit 3:16bit 4:32bit
 */
void es8388_i2s_config(u8 fmt, u8 len)
{
    fmt &= 0X03;
    len &= 0X07;  //限定范围
    es8388_write_reg(23, (fmt << 1)|(len << 3));  //Reg23,ES8388工作模式设置
}

/**
 * @brief 设置耳机音量
 * 
 * @param volume 音量(0~30)
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
 * @brief 设置扬声器音量
 * 
 * @param volume 音量(0~30)
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
 * @brief 设置3D环绕声
 * 
 * @param depth 强度(0~7)
 */
void es8388_3d_config(u8 depth)
{ 
    depth &= 0X7;  //限定范围 
    es8388_write_reg(0X1D, depth<<2);  //Reg7,3D环绕设置
}

/**
 * @brief ES8388 ADC/DAC配置
 * 
 * @param dac_en DAC使能 1:开启 0:关闭
 * @param adc_en ADC使能 1:开启 0:关闭
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
 * @brief ES8388 DAC输出通道配置
 * 
 * @param o1_en 通道1 1:开启 0:关闭
 * @param o2_en 通道2 1:开启 0:关闭
 */
void es8388_output_config(u8 o1_en, u8 o2_en)
{
    u8 tempreg = 0;
    tempreg |= o1_en*(3<<4);
    tempreg |= o2_en*(3<<2);
    es8388_write_reg(0x04, tempreg);
}

/**
 * @brief ES8388 MIC增益设置(PGA增益)
 * 
 * @param gain 0~8 对应 0~24dB
 */
void es8388_mic_gain_config(u8 gain)
{
    gain &= 0X0F;
    gain |= gain<<4;
    es8388_write_reg(0X09, gain);  //Reg9,左右通道PGA增益设置
}

/**
 * @brief ES8388 ALC配置
 * 
 * @param sel      0:关闭ALC 1:右通道ALC 2:左通道ALC 3:立体声ALC
 * @param gain_max 最大增益 0~7 对应 -6.5dB ~ +35.5dB
 * @param gain_min 最小增益 0~7 对应  -12dB ~ +30dB
 */
void es8388_alc_control(u8 sel, u8 gain_max, u8 gain_min)
{
    u8 tempreg = 0;
    tempreg  = sel<<6;
    tempreg |= (gain_max&0X07)<<3;
    tempreg |= gain_min&0X07;
    es8388_write_reg(0X12, tempreg);  //Reg18,ALC设置
}

/**
 * @brief ES8388 ADC输出通道配置
 * 
 * @param in 0:通道1输入 1:通道2输入
 */
void es8388_input_config(u8 in)
{
    es8388_write_reg(0x0A, (5*in)<<4);  //ADC1 输入通道选择L/R INPUT1
}
