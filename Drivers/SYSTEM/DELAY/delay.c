#include "delay.h"
#include "sys.h"

static u32 fac_us = 0;      //us延时倍乘数

/**
 * @brief 初始化延迟函数,当使用UCOS的时候,此函数会初始化UCOS的时钟节拍,SYSTICK的时钟固定为AHB时钟的1/8
 * 
 * @param sysclk 系统时钟频率
 */
void delay_init(u16 sysclk)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);  //SysTick频率为HCLK
    fac_us = sysclk;                            //不论是否使用OS,fac_us都需要使用
}

/**
 * @brief 微秒级延时函数
 * 
 * @param nus 延时微秒数,nus<=1000
 */
void delay_us(u32 nus)
{		
    u32 ticks;
    u32 told   = 0;
    u32 tnow   = 0;
    u32 tcnt   = 0;
    u32 reload = SysTick->LOAD;  //LOAD的值	    	 
        ticks  = nus*fac_us;     //需要的节拍数 
        told   = SysTick->VAL;   //刚进入时的计数器值
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)    //这里注意一下SYSTICK是一个递减的计数器就可以了
                tcnt += told-tnow;
            else
                tcnt += reload-tnow+told;
            told = tnow;
            if(tcnt >= ticks)  //时间超过/等于要延迟的时间,则退出
                break;
        }
    }
}

/**
 * @brief 毫秒级延时函数
 * 
 * @param nms 延时毫秒数
 */
void delay_ms(u16 nms)
{
    u32 i;
    for(i=0; i<nms; i++)
        delay_us(1000);
}
