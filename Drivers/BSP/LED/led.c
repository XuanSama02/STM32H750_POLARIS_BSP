#include "led.h"

/**
 * @brief ��ʼ��LED
 * 
 */
void led_init(void)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //����ʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();  //����GPIOBʱ��
    //��ʼ��LED0��LED1
    ymx_gpio_init.Pin   = LED0_PIN|LED1_PIN;          //PB1,0
    ymx_gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;        //�������
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //����
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(LED_GPIO_PORT, &ymx_gpio_init);
    //Ĭ�Ϲر�LED
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED0_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
}

