#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM3_Handler;  //��ʱ����� 
TIM_HandleTypeDef TIM6_Handler;  //��ʱ��6��� 

__IO u32 ticknum=0;

u16 frame;
vu8 frameup;

/**
 * @brief ͨ�ö�ʱ��3�жϳ�ʼ��,�����ж�,APB1����,ʱ��ΪHCLK/2
 * 
 * @param arr �Զ���װֵ
 * @param psc Ԥ��Ƶϵ��
 */
void tim3_init(u16 arr, u16 psc)
{
    TIM3_Handler.Instance           = TIM3;  //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Period        = arr;   //�Զ�װ��ֵ
    TIM3_Handler.Init.Prescaler     = psc;   //��Ƶ
    TIM3_Handler.Init.CounterMode   = TIM_COUNTERMODE_UP;  //���ϼ�����
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM3_Handler);
    HAL_TIM_Base_Start_IT(&TIM3_Handler);    //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�ж�   
}

/**
 * @brief ͨ�ö�ʱ��6��ʼ��,�����ж�,APB1����,ʱ��ΪHCLK/2
 * 
 * @param arr �Զ���װֵ
 * @param psc Ԥ��Ƶϵ��
 */
void tim6_init(u16 arr, u16 psc)
{  
    TIM6_Handler.Instance           = TIM6;  //ͨ�ö�ʱ��6
    TIM6_Handler.Init.Period        = arr;   //�Զ�װ��ֵ
    TIM6_Handler.Init.Prescaler     = psc;   //��Ƶ
    TIM6_Handler.Init.CounterMode   = TIM_COUNTERMODE_UP;  //���ϼ�����
    TIM6_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM6_Handler);
    HAL_TIM_Base_Start_IT(&TIM6_Handler);    //ʹ�ܶ�ʱ��6�Ͷ�ʱ��6�ж�   
}

/**
 * @brief ��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�,�ᱻHAL_TIM_Base_Init()�Զ�����
 * 
 * @param htim ��ʱ�����
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��;
        HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);  //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
        HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�
    }
    else if(htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();                //ʹ��TIM6ʱ��;
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 3);  //�����ж����ȼ�����ռ���ȼ�0�������ȼ�3
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);          //����ITM6�ж�
    }
}

/**
 * @brief ��ʱ��3�жϷ�����
 * 
 */
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

/**
 * @brief ��ʱ��6�жϷ�����
 * 
 */
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM6_Handler);
}

/**
 * @brief ��ʱ��3�жϻص�����
 * 
 * @param htim 
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)          //��ʱ��3
    {
        //ticknum++;
        printf("frame:%d\r\n", frame);  //��ӡ֡��
        frame = 0;
    }
    else if(htim->Instance == TIM6)     //��ʱ��6
    {
        frameup = 1;
    }
}

/**
 * @brief ��ȡticknum
 * 
 * @return u32 ticknum
 */
u32 get_ticknum(void)
{
    return ticknum;
}

/**
 * @brief ʹ��TIM3
 * 
 */
void tim3_start(void)
{
    __HAL_TIM_ENABLE(&TIM3_Handler);
    ticknum=0;
}

/**
 * @brief �ر�TIM3
 * 
 */
void tim3_stop(void)
{
    __HAL_TIM_DISABLE(&TIM3_Handler);
}
