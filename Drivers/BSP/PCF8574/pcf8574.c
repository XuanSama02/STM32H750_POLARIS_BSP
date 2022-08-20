#include "pcf8574.h"
#include "delay.h"

/**
 * @brief 初始化PCF8574
 * 
 * @return u8 1:初始化失败 0:初始化成功
 */
u8 pcf8574_init(void)
{
    u8 temp = 0;
    GPIO_InitTypeDef ymx_gpio_init;
    //开启时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();  //使能GPIOB时钟
    //配置GPIO
    ymx_gpio_init.Pin   = GPIO_PIN_10;                //PG10
    ymx_gpio_init.Mode  = GPIO_MODE_INPUT;            //输入
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //上拉
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    HAL_GPIO_Init(GPIOG, &ymx_gpio_init);             //初始化
    i2c_init();                                       //I2C初始化
    //检查PCF8574是否在位
    i2c_start();
    i2c_send_byte(PCF8574_ADDR);  //写地址
    temp = i2c_wait_ack();        //等待应答,根据是否有ACK应答,来判断PCF8574的状态
    i2c_stop();                   //产生一个停止条件
    pcf8574_write_byte(0XFF);     //默认情况下所有IO输出高电平
    return temp;
}

/**
 * @brief 读取PCF8574的8位IO值
 * 
 * @return u8 读到的数据
 */
u8 pcf8574_read_byte(void)
{				  
    u8 temp = 0;
    i2c_start();
    i2c_send_byte(PCF8574_ADDR|0X01);  //进入接收模式
    i2c_wait_ack();
    temp = i2c_read_byte(0);
    i2c_stop();  //产生一个停止条件
    return temp;
}

/**
 * @brief 向PCF8574写入8位IO值
 * 
 * @param data 要写入的数据
 */
void pcf8574_write_byte(u8 data)
{
    i2c_start();
    i2c_send_byte(PCF8574_ADDR|0X00);  //发送器件地址0X40,写数据
    i2c_wait_ack();
    i2c_send_byte(data);  //发送字节
    i2c_wait_ack();
    i2c_stop();  //产生一个停止条件
    delay_ms(10);
}

/**
 * @brief 读取PCF8574的某个IO的值
 * 
 * @param bit 要读取的IO编号,0~7
 * @return u8 此IO的值,0或1
 */
u8 pcf8574_read_bit(u8 bit)
{
    u8 data;
    data=pcf8574_read_byte();  //先读取这个8位IO的值
    if(data & (1<<bit))
        return 1;
    else
        return 0;
}  

/**
 * @brief 设置PCF8574某个IO的高低电平
 * 
 * @param bit 要设置的IO编号,0~7
 * @param sta IO的状态;0或1
 */
void pcf8574_write_bit(u8 bit, u8 sta)
{
    u8 data;
    data = pcf8574_read_byte();  //先读出原来的设置
    if(sta == 0)
        data &= ~(1<<bit);
    else
        data |= 1<<bit;
    pcf8574_write_byte(data);    //写入新的数据
}
