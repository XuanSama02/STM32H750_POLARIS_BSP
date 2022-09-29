#include "key.h"
#include "delay.h"

/**
 * @brief 按键初始化函数
 * 
 */
void key_init(void)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //开启时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();  //开启GPIOA时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();  //开启GPIOC时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();  //开启GPIOH时钟
    //初始化WKUP
    ymx_gpio_init.Pin   = WKUP_PIN;                   //PA0
    ymx_gpio_init.Mode  = GPIO_MODE_INPUT;            //输入
    ymx_gpio_init.Pull  = GPIO_PULLDOWN;              //下拉
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    HAL_GPIO_Init(WKUP_GPIO_PORT, &ymx_gpio_init);
    //初始化KEY0，KEY1
    ymx_gpio_init.Pin   = KEY0_PIN|KEY1_PIN;          //PH2,3
    ymx_gpio_init.Mode  = GPIO_MODE_INPUT;            //输入
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //上拉
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    HAL_GPIO_Init(KEY0_GPIO_PORT, &ymx_gpio_init);
    //初始化KEY2
    ymx_gpio_init.Pin   = GPIO_PIN_13;                //PC13
    HAL_GPIO_Init(KEY2_GPIO_PORT, &ymx_gpio_init);
}

/**
 * @brief 按键扫描函数,此函数有响应优先级:KEY0>KEY1>KEY2>WKUP
 * 
 * @param mode 0:不支持连续按下
 *             1:支持连续按下
 * @return u8  按下的键值,0:无按键按下
 */
u8 key_scan(u8 mode)
{
    static u8 key_up = 1;  //按键松开标志
    if(mode == 1)          //支持连按
        key_up = 1;
    if(key_up && (KEY0_READ()==0 || KEY1_READ()==0 || KEY2_READ()==0 || WKUP_READ()==1))  //任意按键按下
    {
        delay_ms(10);
        key_up = 0;  //按键按下
        if(KEY0_READ() == 0)       //KEY0按下
            return KEY0_PRES;      //返回KEY0值
        else if(KEY1_READ() == 0)  //KEY1按下
            return KEY1_PRES;      //返回KEY1值
        else if(KEY2_READ() == 0)  //KEY2按下
            return KEY2_PRES;      //返回KEY2值
        else if(WKUP_READ() == 1)  //WKUP按下
            return WKUP_PRES;      //返回WKUP值
    }
    else if(KEY0_READ()==1 && KEY1_READ()==1 && KEY2_READ()==1 && WKUP_READ()==0)  //无按键按下
        key_up=1;  //按键松开
    return 0;      //无按键按下
}
