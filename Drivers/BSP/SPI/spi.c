#include "spi.h"

SPI_HandleTypeDef SPI2_Handler;  //SPI2句柄

/**
 * @brief SPI2初始化,配置为主机模式
 * 
 */
void spi2_init(void)
{
    SPI2_Handler.Instance               = SPI2;                             //SPI2
    SPI2_Handler.Init.Mode              = SPI_MODE_MASTER;                  //设置SPI主机模式
    SPI2_Handler.Init.Direction         = SPI_DIRECTION_2LINES;             //设置SPI双线数据模式
    SPI2_Handler.Init.DataSize          = SPI_DATASIZE_8BIT;                //设置SPI的数据8bit
    SPI2_Handler.Init.CLKPolarity       = SPI_POLARITY_HIGH;                //串行同步时钟的空闲状态为高电平
    SPI2_Handler.Init.CLKPhase          = SPI_PHASE_2EDGE;                  //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI2_Handler.Init.NSS               = SPI_NSS_SOFT;                     //NSS信号由SSI位控制(软件触发)
    SPI2_Handler.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;            //NSS信号脉冲失能
    SPI2_Handler.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  //SPI主模式IO状态保持使能
    SPI2_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;        //波特率预分频值为256
    SPI2_Handler.Init.FirstBit          = SPI_FIRSTBIT_MSB;                 //数据传输从MSB位开始
    SPI2_Handler.Init.TIMode            = SPI_TIMODE_DISABLE;               //关闭TI模式
    SPI2_Handler.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;       //关闭硬件CRC校验
    SPI2_Handler.Init.CRCPolynomial     = 7;                                //CRC值计算的多项式
    HAL_SPI_Init(&SPI2_Handler);

    __HAL_SPI_ENABLE(&SPI2_Handler);  //使能SPI2
    spi2_read_write_byte(0Xff);       //启动传输
}

/**
 * @brief SPI底层驱动,会由__HAL_SPI_Init()自动调用
 * 
 * @param hspi SPI句柄
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef         ymx_gpio_init;
    RCC_PeriphCLKInitTypeDef ymx_periph_clk_init;
    //开启时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();  //使能GPIOF时钟
    __HAL_RCC_SPI2_CLK_ENABLE();   //使能SPI2时钟
    //设置SPI2的时钟源
    ymx_periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_SPI2;       //设置SPI2时钟源
    ymx_periph_clk_init.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;  //SPI2时钟源使用PLL1Q
    HAL_RCCEx_PeriphCLKConfig(&ymx_periph_clk_init);  //配置SPI2时钟
    //配置GPIO
    ymx_gpio_init.Pin       = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;  //PB13,PB14,PB15
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;                      //复用推挽输出
    ymx_gpio_init.Pull      = GPIO_PULLUP;                          //上拉
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;            //高速
    ymx_gpio_init.Alternate = GPIO_AF5_SPI2;                        //复用为SPI2
    HAL_GPIO_Init(GPIOB, &ymx_gpio_init);
}

/**
 * @brief SPI速度设置,SPI速度=PLL1Q/分频系数,一般PLL1Q=主频/2
 * 
 * @param SPI_BAUDRATEPRESCALER_x SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_256
 */
void spi2_set_speed(u32 SPI_BAUDRATEPRESCALER_x)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BAUDRATEPRESCALER_x));  //判断有效性
    __HAL_SPI_DISABLE(&SPI2_Handler);                        //关闭SPI
    SPI2_Handler.Instance->CFG1 &= ~(0X7<<28);               //位30-28清零，用来设置波特率
    SPI2_Handler.Instance->CFG1 |= SPI_BAUDRATEPRESCALER_x;  //设置SPI速度
    __HAL_SPI_ENABLE(&SPI2_Handler);                         //使能SPI
}

/**
 * @brief SPI2读写一个字节
 * 
 * @param txd 要写入的字节
 * @return u8 读取到的字节
 */
u8 spi2_read_write_byte(u8 txd)
{
    u8 rxd;
    HAL_SPI_TransmitReceive(&SPI2_Handler, &txd, &rxd, 1, 1000);
     return rxd;  //返回收到的数据		
}
