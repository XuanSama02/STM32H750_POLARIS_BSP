#include "myiic.h"
#include "delay.h"

/**
 * @brief I2C初始化
 * 
 */
void i2c_init(void)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //使能时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();   //使能GPIOH时钟
    //配置GPIO
    ymx_gpio_init.Pin   = GPIO_PIN_4|GPIO_PIN_5;      //设置PH4,PH5
    ymx_gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;        //推挽输出
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //上拉
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //快速
    HAL_GPIO_Init(GPIOH, &ymx_gpio_init);
    //I2C空闲状态:SDA与SCL同时为高电平
    IIC_SDA(1);
    IIC_SCL(1);  
}

/**
 * @brief 产生I2C起始信号
 * 
 */
void i2c_start(void)
{
    SDA_OUT();   //sad线输出
    IIC_SDA(1);
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(0);  //START:SCL高电平,SDA下降沿
    delay_us(4);
    IIC_SCL(0);  //钳住I2C总线，准备发送或接收数据
}

/**
 * @brief 产生I2C停止信号
 * 
 */
void i2c_stop(void)
{
    SDA_OUT();   //sda线输出
    IIC_SCL(0);
    IIC_SDA(0);  //STOP:SCL高电平,SDA上升沿
    delay_us(4);
    IIC_SCL(1);
    IIC_SDA(1);  //发送I2C总线结束信号
    delay_us(4);
}

/**
 * @brief I2C发送一个字节
 * 
 * @param txd 发送数据
 */
void i2c_send_byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL(0);  //拉低时钟开始数据传输
    for(t=0; t<8; t++)  //依次发送1个字节的8个位
    {
        IIC_SDA((txd&0x80) >> 7);
        txd <<= 1;
        delay_us(2);    //对TEA5767这三个延时都是必须的
        IIC_SCL(1);
        delay_us(2);
        IIC_SCL(0);
        delay_us(2);
    }
}

/**
 * @brief I2C读取一个字节
 * 
 * @param ack 1:发送ack 0:发送nack
 * @return u8 读取的数据
 */
u8 i2c_read_byte(unsigned char ack)
{
    unsigned char i;
    unsigned char receive = 0;
    SDA_IN();  //SDA设置为输入
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
        i2c_nack();  //发送nACK
    else
        i2c_ack();   //发送ACK
    return receive;  //返回发送的数据
}

/**
 * @brief I2C等待应答信号
 * 
 * @return u8 1:接收应答失败 0:接收应答成功
 */
u8 i2c_wait_ack(void)
{
    u8 ucErrTime = 0;
    SDA_IN();  //SDA设置为输入
    IIC_SDA(1);
    delay_us(1);
    IIC_SCL(1);
    delay_us(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)  //超时
        {
            i2c_stop();      //产生I2C停止信号
            return 1;        //接收失败
        }
    }
    IIC_SCL(0);  //时钟输出0
    return 0;    //接收成功
}

/**
 * @brief 产生ack应答
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
 * @brief 不产生ack应答
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
 * @brief I2C写入一字节
 * 
 * @param daddr 设备地址
 * @param addr  数据地址
 * @param data  写入数据
 */
void i2c_write_one_byte(u8 daddr, u8 addr, u8 data)
{
    delay_ms(1);
}

/**
 * @brief I2C读取一字节
 * 
 * @param daddr 设备地址
 * @param addr  数据地址
 * @return u8   读出数据
 */
u8 i2c_read_one_byte(u8 daddr, u8 addr)
{
    delay_ms(1);
    return 0;
}
