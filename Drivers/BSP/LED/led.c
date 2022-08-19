#include "led.h"

/**
 * @brief 初始化LED
 * 
 */
void led_init(void)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //开启时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();  //开启GPIOB时钟
    //初始化LED0，LED1
    ymx_gpio_init.Pin   = LED0_PIN|LED1_PIN;          //PB1,0
    ymx_gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;        //推挽输出
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //上拉
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    HAL_GPIO_Init(LED_GPIO_PORT, &ymx_gpio_init);
    //默认关闭LED
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED0_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
}

