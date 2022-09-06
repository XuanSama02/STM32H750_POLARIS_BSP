#include "delay.h"
#include "sys.h"

static u32 fac_us = 0;      //us��ʱ������

/**
 * @brief ��ʼ���ӳٺ���,��ʹ��UCOS��ʱ��,�˺������ʼ��UCOS��ʱ�ӽ���,SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
 * 
 * @param sysclk ϵͳʱ��Ƶ��
 */
void delay_init(u16 sysclk)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);  //SysTickƵ��ΪHCLK
    fac_us = sysclk;                            //�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
}

/**
 * @brief ΢�뼶��ʱ����
 * 
 * @param nus ��ʱ΢����,nus<=1000
 */
void delay_us(u32 nus)
{		
    u32 ticks;
    u32 told   = 0;
    u32 tnow   = 0;
    u32 tcnt   = 0;
    u32 reload = SysTick->LOAD;  //LOAD��ֵ	    	 
        ticks  = nus*fac_us;     //��Ҫ�Ľ����� 
        told   = SysTick->VAL;   //�ս���ʱ�ļ�����ֵ
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)    //����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����
                tcnt += told-tnow;
            else
                tcnt += reload-tnow+told;
            told = tnow;
            if(tcnt >= ticks)  //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�
                break;
        }
    }
}

/**
 * @brief ���뼶��ʱ����
 * 
 * @param nms ��ʱ������
 */
void delay_ms(u16 nms)
{
    u32 i;
    for(i=0; i<nms; i++)
        delay_us(1000);
}
