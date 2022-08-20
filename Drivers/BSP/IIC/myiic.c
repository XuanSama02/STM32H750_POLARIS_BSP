#include "myiic.h"
#include "delay.h"

/**
 * @brief I2C��ʼ��
 * 
 */
void i2c_init(void)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //ʹ��ʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();   //ʹ��GPIOHʱ��
    //����GPIO
    ymx_gpio_init.Pin   = GPIO_PIN_4|GPIO_PIN_5;      //����PH4,PH5
    ymx_gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;        //�������
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //����
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(GPIOH, &ymx_gpio_init);
    //I2C����״̬:SDA��SCLͬʱΪ�ߵ�ƽ
    IIC_SDA(1);
    IIC_SCL(1);  
}

/**
 * @brief ����I2C��ʼ�ź�
 * 
 */
void i2c_start(void)
{
    SDA_OUT();   //sad�����
    IIC_SDA(1);
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(0);  //START:SCL�ߵ�ƽ,SDA�½���
    delay_us(4);
    IIC_SCL(0);  //ǯסI2C���ߣ�׼�����ͻ��������
}

/**
 * @brief ����I2Cֹͣ�ź�
 * 
 */
void i2c_stop(void)
{
    SDA_OUT();   //sda�����
    IIC_SCL(0);
    IIC_SDA(0);  //STOP:SCL�ߵ�ƽ,SDA������
    delay_us(4);
    IIC_SCL(1);
    IIC_SDA(1);  //����I2C���߽����ź�
    delay_us(4);
}

/**
 * @brief I2C����һ���ֽ�
 * 
 * @param txd ��������
 */
void i2c_send_byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL(0);  //����ʱ�ӿ�ʼ���ݴ���
    for(t=0; t<8; t++)  //���η���1���ֽڵ�8��λ
    {
        IIC_SDA((txd&0x80) >> 7);
        txd <<= 1;
        delay_us(2);    //��TEA5767��������ʱ���Ǳ����
        IIC_SCL(1);
        delay_us(2);
        IIC_SCL(0);
        delay_us(2);
    }
}

/**
 * @brief I2C��ȡһ���ֽ�
 * 
 * @param ack 1:����ack 0:����nack
 * @return u8 ��ȡ������
 */
u8 i2c_read_byte(unsigned char ack)
{
    unsigned char i;
    unsigned char receive = 0;
    SDA_IN();  //SDA����Ϊ����
    for(i=0; i<8; i++)
    {
        IIC_SCL(0);
        delay_us(2);
        IIC_SCL(1);
        receive <<= 1;
        if(READ_SDA)
            receive++;
        delay_us(1);
    }
    if(!ack)
        i2c_nack();  //����nACK
    else
        i2c_ack();   //����ACK
    return receive;  //���ط��͵�����
}

/**
 * @brief I2C�ȴ�Ӧ���ź�
 * 
 * @return u8 1:����Ӧ��ʧ�� 0:����Ӧ��ɹ�
 */
u8 i2c_wait_ack(void)
{
    u8 ucErrTime = 0;
    SDA_IN();  //SDA����Ϊ����
    IIC_SDA(1);
    delay_us(1);
    IIC_SCL(1);
    delay_us(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)  //��ʱ
        {
            i2c_stop();      //����I2Cֹͣ�ź�
            return 1;        //����ʧ��
        }
    }
    IIC_SCL(0);  //ʱ�����0
    return 0;    //���ճɹ�
}

/**
 * @brief ����ackӦ��
 * 
 */
void i2c_ack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(0);
    delay_us(2);
    IIC_SCL(1);
    delay_us(2);
    IIC_SCL(0);
}

/**
 * @brief ������ackӦ��
 * 
 */
void i2c_nack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(1);
    delay_us(2);
    IIC_SCL(1);
    delay_us(2);
    IIC_SCL(0);
}

/**
 * @brief I2Cд��һ�ֽ�
 * 
 * @param daddr �豸��ַ
 * @param addr  ���ݵ�ַ
 * @param data  д������
 */
void i2c_write_one_byte(u8 daddr, u8 addr, u8 data)
{
    delay_ms(1);
}

/**
 * @brief I2C��ȡһ�ֽ�
 * 
 * @param daddr �豸��ַ
 * @param addr  ���ݵ�ַ
 * @return u8   ��������
 */
u8 i2c_read_one_byte(u8 daddr, u8 addr)
{
    delay_ms(1);
    return 0;
}
