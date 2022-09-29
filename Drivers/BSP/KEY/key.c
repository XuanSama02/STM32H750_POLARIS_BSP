#include "key.h"
#include "delay.h"

/**
 * @brief ������ʼ������
 * 
 */
void key_init(void)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //����ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();  //����GPIOAʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();  //����GPIOCʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();  //����GPIOHʱ��
    //��ʼ��WKUP
    ymx_gpio_init.Pin   = WKUP_PIN;                   //PA0
    ymx_gpio_init.Mode  = GPIO_MODE_INPUT;            //����
    ymx_gpio_init.Pull  = GPIO_PULLDOWN;              //����
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(WKUP_GPIO_PORT, &ymx_gpio_init);
    //��ʼ��KEY0��KEY1
    ymx_gpio_init.Pin   = KEY0_PIN|KEY1_PIN;          //PH2,3
    ymx_gpio_init.Mode  = GPIO_MODE_INPUT;            //����
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //����
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(KEY0_GPIO_PORT, &ymx_gpio_init);
    //��ʼ��KEY2
    ymx_gpio_init.Pin   = GPIO_PIN_13;                //PC13
    HAL_GPIO_Init(KEY2_GPIO_PORT, &ymx_gpio_init);
}

/**
 * @brief ����ɨ�躯��,�˺�������Ӧ���ȼ�:KEY0>KEY1>KEY2>WKUP
 * 
 * @param mode 0:��֧����������
 *             1:֧����������
 * @return u8  ���µļ�ֵ,0:�ް�������
 */
u8 key_scan(u8 mode)
{
    static u8 key_up = 1;  //�����ɿ���־
    if(mode == 1)          //֧������
        key_up = 1;
    if(key_up && (KEY0_READ()==0 || KEY1_READ()==0 || KEY2_READ()==0 || WKUP_READ()==1))  //���ⰴ������
    {
        delay_ms(10);
        key_up = 0;  //��������
        if(KEY0_READ() == 0)       //KEY0����
            return KEY0_PRES;      //����KEY0ֵ
        else if(KEY1_READ() == 0)  //KEY1����
            return KEY1_PRES;      //����KEY1ֵ
        else if(KEY2_READ() == 0)  //KEY2����
            return KEY2_PRES;      //����KEY2ֵ
        else if(WKUP_READ() == 1)  //WKUP����
            return WKUP_PRES;      //����WKUPֵ
    }
    else if(KEY0_READ()==1 && KEY1_READ()==1 && KEY2_READ()==1 && WKUP_READ()==0)  //�ް�������
        key_up=1;  //�����ɿ�
    return 0;      //�ް�������
}
