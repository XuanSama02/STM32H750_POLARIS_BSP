#include "pcf8574.h"
#include "delay.h"

/**
 * @brief ��ʼ��PCF8574
 * 
 * @return u8 1:��ʼ��ʧ�� 0:��ʼ���ɹ�
 */
u8 pcf8574_init(void)
{
    u8 temp = 0;
    GPIO_InitTypeDef ymx_gpio_init;
    //����ʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();  //ʹ��GPIOBʱ��
    //����GPIO
    ymx_gpio_init.Pin   = GPIO_PIN_10;                //PG10
    ymx_gpio_init.Mode  = GPIO_MODE_INPUT;            //����
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //����
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(GPIOG, &ymx_gpio_init);             //��ʼ��
    i2c_init();                                       //I2C��ʼ��
    //���PCF8574�Ƿ���λ
    i2c_start();
    i2c_send_byte(PCF8574_ADDR);  //д��ַ
    temp = i2c_wait_ack();        //�ȴ�Ӧ��,�����Ƿ���ACKӦ��,���ж�PCF8574��״̬
    i2c_stop();                   //����һ��ֹͣ����
    pcf8574_write_byte(0XFF);     //Ĭ�����������IO����ߵ�ƽ
    return temp;
}

/**
 * @brief ��ȡPCF8574��8λIOֵ
 * 
 * @return u8 ����������
 */
u8 pcf8574_read_byte(void)
{				  
    u8 temp = 0;
    i2c_start();
    i2c_send_byte(PCF8574_ADDR|0X01);  //�������ģʽ
    i2c_wait_ack();
    temp = i2c_read_byte(0);
    i2c_stop();  //����һ��ֹͣ����
    return temp;
}

/**
 * @brief ��PCF8574д��8λIOֵ
 * 
 * @param data Ҫд�������
 */
void pcf8574_write_byte(u8 data)
{
    i2c_start();
    i2c_send_byte(PCF8574_ADDR|0X00);  //����������ַ0X40,д����
    i2c_wait_ack();
    i2c_send_byte(data);  //�����ֽ�
    i2c_wait_ack();
    i2c_stop();  //����һ��ֹͣ����
    delay_ms(10);
}

/**
 * @brief ��ȡPCF8574��ĳ��IO��ֵ
 * 
 * @param bit Ҫ��ȡ��IO���,0~7
 * @return u8 ��IO��ֵ,0��1
 */
u8 pcf8574_read_bit(u8 bit)
{
    u8 data;
    data=pcf8574_read_byte();  //�ȶ�ȡ���8λIO��ֵ
    if(data & (1<<bit))
        return 1;
    else
        return 0;
}  

/**
 * @brief ����PCF8574ĳ��IO�ĸߵ͵�ƽ
 * 
 * @param bit Ҫ���õ�IO���,0~7
 * @param sta IO��״̬;0��1
 */
void pcf8574_write_bit(u8 bit, u8 sta)
{
    u8 data;
    data = pcf8574_read_byte();  //�ȶ���ԭ��������
    if(sta == 0)
        data &= ~(1<<bit);
    else
        data |= 1<<bit;
    pcf8574_write_byte(data);    //д���µ�����
}
