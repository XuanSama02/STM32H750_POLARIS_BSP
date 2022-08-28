#include "usart.h"

//���ʹ��OS,����������ͷ�ļ�����
#if SYSTEM_SUPPORT_OS
    #include "includes.h"  //OS֧��
#endif

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
//#pragma import(__use_no_semihosting)

//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

FILE __stdout;

//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while((USART1->ISR&0X40) == 0);  //ѭ������,ֱ���������
    USART1->TDR = (u8)ch;
    return ch;
}

#if EN_USART1_RX  //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN];   //���ջ���,���USART_REC_LEN���ֽ�

//����״̬
//bit15,    ������ɱ�־
//bit14,    ���յ�0x0d
//bit13~0,  ���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0;  //����״̬���	

u8 aRxBuffer[RXBUFFERSIZE];       //HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART1_Handler; //UART���

/**
 * @brief ��ʼ������1
 * 
 * @param baud_rate ������
 */
void uart1_init(u32 baud_rate)
{
    //UART��ʼ������
    UART1_Handler.Instance        = USART1;               //USART1
    UART1_Handler.Init.BaudRate   = baud_rate;            //������
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
    UART1_Handler.Init.StopBits   = UART_STOPBITS_1;      //һ��ֹͣλ
    UART1_Handler.Init.Parity     = UART_PARITY_NONE;     //����żУ��λ
    UART1_Handler.Init.HwFlowCtl  = UART_HWCONTROL_NONE;  //��Ӳ������
    UART1_Handler.Init.Mode       = UART_MODE_TX_RX;      //�շ�ģʽ
    HAL_UART_Init(&UART1_Handler);                        //�˺���ͬʱ��ʹ��UART1
    
    HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);  //�ú����Ὺ�������ж�:��־λUART_IT_RXNE,��������:���ջ���,���ջ���Ľ������������
}

/**
 * @brief UART�ײ��ʼ��,ʱ��ʹ��,��������,�ж�����
 * 
 * @param huart uart���
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef ymx_gpio_init;

    if(huart->Instance == USART1)  //����Ǵ���1,���д���1��MSP��ʼ��
    {
        //����ʱ��
        __HAL_RCC_GPIOA_CLK_ENABLE();                    //ʹ��GPIOAʱ��
        __HAL_RCC_USART1_CLK_ENABLE();                   //ʹ��USART1ʱ��
        //����GPIO_PA9(TX)
        ymx_gpio_init.Pin       = GPIO_PIN_9;            //PA9
        ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;       //�����������
        ymx_gpio_init.Pull      = GPIO_PULLUP;           //����
        ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_HIGH;  //����
        ymx_gpio_init.Alternate = GPIO_AF7_USART1;       //����ΪUSART1
        HAL_GPIO_Init(GPIOA, &ymx_gpio_init);            //��ʼ��PA9
        //����GPIO_PA10(RX)
        ymx_gpio_init.Pin       = GPIO_PIN_10;           //PA10
        HAL_GPIO_Init(GPIOA, &ymx_gpio_init);            //��ʼ��PA10
    #if EN_USART1_RX
            HAL_NVIC_EnableIRQ(USART1_IRQn);             //ʹ��USART1�ж�ͨ��
            HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);     //��ռ���ȼ�3�������ȼ�3
    #endif
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART1)//����Ǵ���1
    {
        if((USART_RX_STA&0x8000) == 0)//����δ���
        {
            if(USART_RX_STA&0x4000)  //���յ���0x0d
            {
                if(aRxBuffer[0] != 0x0a)  //���մ���,���¿�ʼ
                    USART_RX_STA = 0;
                else                      //���������
                    USART_RX_STA |= 0x8000;
            }
            else                     //��û�յ�0X0D
            {
                if(aRxBuffer[0] == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF] = aRxBuffer[0];
                    USART_RX_STA++;
                    if(USART_RX_STA > (USART_REC_LEN-1))  //�������ݴ���,���¿�ʼ����
                        USART_RX_STA = 0;
                }
            }
        }
    }
}

void USART1_IRQHandler(void)                	
{ 
    u32 timeout  = 0;
    u32 maxDelay = 0x1FFFF;
#if SYSTEM_SUPPORT_OS  //ʹ��OS
    OSIntEnter();
#endif
    HAL_UART_IRQHandler(&UART1_Handler);  //����HAL���жϴ����ú���
    timeout=0;
    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY)  //�ȴ�����
    {
        timeout++;  //��ʱ����
        if(timeout > maxDelay)
            break;
    }

    timeout=0;
    while(HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)  //һ�δ������֮��,���¿����жϲ�����RxXferCountΪ1
    {
        timeout++;  //��ʱ����
        if(timeout > maxDelay)
            break;
    }

#if SYSTEM_SUPPORT_OS  //ʹ��OS
    OSIntExit();
#endif
}

#endif /*EN_USART1_RX*/
