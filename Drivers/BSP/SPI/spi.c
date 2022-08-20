#include "spi.h"

SPI_HandleTypeDef SPI2_Handler;  //SPI2���

/**
 * @brief SPI2��ʼ��,����Ϊ����ģʽ
 * 
 */
void spi2_init(void)
{
    SPI2_Handler.Instance               = SPI2;                             //SPI2
    SPI2_Handler.Init.Mode              = SPI_MODE_MASTER;                  //����SPI����ģʽ
    SPI2_Handler.Init.Direction         = SPI_DIRECTION_2LINES;             //����SPI˫������ģʽ
    SPI2_Handler.Init.DataSize          = SPI_DATASIZE_8BIT;                //����SPI������8bit
    SPI2_Handler.Init.CLKPolarity       = SPI_POLARITY_HIGH;                //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI2_Handler.Init.CLKPhase          = SPI_PHASE_2EDGE;                  //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI2_Handler.Init.NSS               = SPI_NSS_SOFT;                     //NSS�ź���SSIλ����(�������)
    SPI2_Handler.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;            //NSS�ź�����ʧ��
    SPI2_Handler.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  //SPI��ģʽIO״̬����ʹ��
    SPI2_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;        //������Ԥ��ƵֵΪ256
    SPI2_Handler.Init.FirstBit          = SPI_FIRSTBIT_MSB;                 //���ݴ����MSBλ��ʼ
    SPI2_Handler.Init.TIMode            = SPI_TIMODE_DISABLE;               //�ر�TIģʽ
    SPI2_Handler.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;       //�ر�Ӳ��CRCУ��
    SPI2_Handler.Init.CRCPolynomial     = 7;                                //CRCֵ����Ķ���ʽ
    HAL_SPI_Init(&SPI2_Handler);

    __HAL_SPI_ENABLE(&SPI2_Handler);  //ʹ��SPI2
    spi2_read_write_byte(0Xff);       //��������
}

/**
 * @brief SPI�ײ�����,����__HAL_SPI_Init()�Զ�����
 * 
 * @param hspi SPI���
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef         ymx_gpio_init;
    RCC_PeriphCLKInitTypeDef ymx_periph_clk_init;
    //����ʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();  //ʹ��GPIOFʱ��
    __HAL_RCC_SPI2_CLK_ENABLE();   //ʹ��SPI2ʱ��
    //����SPI2��ʱ��Դ
    ymx_periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_SPI2;       //����SPI2ʱ��Դ
    ymx_periph_clk_init.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;  //SPI2ʱ��Դʹ��PLL1Q
    HAL_RCCEx_PeriphCLKConfig(&ymx_periph_clk_init);  //����SPI2ʱ��
    //����GPIO
    ymx_gpio_init.Pin       = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;  //PB13,PB14,PB15
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;                      //�����������
    ymx_gpio_init.Pull      = GPIO_PULLUP;                          //����
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;            //����
    ymx_gpio_init.Alternate = GPIO_AF5_SPI2;                        //����ΪSPI2
    HAL_GPIO_Init(GPIOB, &ymx_gpio_init);
}

/**
 * @brief SPI�ٶ�����,SPI�ٶ�=PLL1Q/��Ƶϵ��,һ��PLL1Q=��Ƶ/2
 * 
 * @param SPI_BAUDRATEPRESCALER_x SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_256
 */
void spi2_set_speed(u32 SPI_BAUDRATEPRESCALER_x)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BAUDRATEPRESCALER_x));  //�ж���Ч��
    __HAL_SPI_DISABLE(&SPI2_Handler);                        //�ر�SPI
    SPI2_Handler.Instance->CFG1 &= ~(0X7<<28);               //λ30-28���㣬�������ò�����
    SPI2_Handler.Instance->CFG1 |= SPI_BAUDRATEPRESCALER_x;  //����SPI�ٶ�
    __HAL_SPI_ENABLE(&SPI2_Handler);                         //ʹ��SPI
}

/**
 * @brief SPI2��дһ���ֽ�
 * 
 * @param txd Ҫд����ֽ�
 * @return u8 ��ȡ�����ֽ�
 */
u8 spi2_read_write_byte(u8 txd)
{
    u8 rxd;
    HAL_SPI_TransmitReceive(&SPI2_Handler, &txd, &rxd, 1, 1000);
     return rxd;  //�����յ�������		
}
