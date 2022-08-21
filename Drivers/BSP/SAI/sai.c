#include "sai.h"
#include "delay.h"

SAI_HandleTypeDef SAI1A_Handler;       //SAI1 Block A���
SAI_HandleTypeDef SAI1B_Handler;       //SAI1 Block B���
DMA_HandleTypeDef SAI1_TXDMA_Handler;  //DMA���;��
DMA_HandleTypeDef SAI1_RXDMA_Handler;  //DMA���վ��

/**
 * @brief ����SAIB��DMA����
 * 
 */
void saia_dma_enable(void)
{
    u32 tempreg = 0;
    tempreg  = SAI1_Block_A->CR1;  //�ȶ�����ǰ������
    tempreg |= 1<<17;              //ʹ��DMA
    SAI1_Block_A->CR1 = tempreg;   //д��CR1�Ĵ�����
}

/**
 * @brief ����SAIA��DMA����
 * 
 */
void SAIB_DMA_Enable(void)
{
    u32 tempreg = 0;
    tempreg  = SAI1_Block_B->CR1;  //�ȶ�����ǰ������
    tempreg |= 1<<17;              //ʹ��DMA
    SAI1_Block_B->CR1 = tempreg;   //д��CR1�Ĵ�����
}

/**
 * @brief SAI Block A��ʼ��, I2S, �����ֱ�׼
 * 
 * @param mode    ����ģʽ: ����SAI_MODEMASTER_TX ����SAI_MODEMASTER_RX �ӷ�SAI_MODESLAVE_TX ����SAI_MODESLAVE_RX
 * @param cpol    ����ʱ��: �½���SAI_CLOCKSTROBING_FALLINGEDGE ������SAI_CLOCKSTROBING_RISINGEDGE
 * @param datalen ���ݴ�С: SAI_DATASIZE_8/10/16/20/24/32
 */
void saia_init(u32 mode, u32 cpol, u32 datalen)
{
    HAL_SAI_DeInit(&SAI1A_Handler);                                //��λSAIA
    SAI1A_Handler.Instance            = SAI1_Block_A;              //SAI1 Bock A
    SAI1A_Handler.Init.AudioMode      = mode;                      //����SAI1����ģʽ
    SAI1A_Handler.Init.Synchro        = SAI_ASYNCHRONOUS;          //��Ƶģ���첽
    SAI1A_Handler.Init.OutputDrive    = SAI_OUTPUTDRIVE_ENABLE;    //����������Ƶģ�����
    SAI1A_Handler.Init.NoDivider      = SAI_MASTERDIVIDER_ENABLE;  //ʹ����ʱ�ӷ�Ƶ��(MCKDIV)
    SAI1A_Handler.Init.FIFOThreshold  = SAI_FIFOTHRESHOLD_1QF;     //����FIFO��ֵ,1/4 FIFO
    SAI1A_Handler.Init.MonoStereoMode = SAI_STEREOMODE;            //������ģʽ
    SAI1A_Handler.Init.Protocol       = SAI_FREE_PROTOCOL;         //����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
    SAI1A_Handler.Init.DataSize       = datalen;                   //�������ݴ�С
    SAI1A_Handler.Init.FirstBit       = SAI_FIRSTBIT_MSB;          //����MSBλ����
    SAI1A_Handler.Init.ClockStrobing  = cpol;                      //������ʱ�ӵ�����/�½���ѡͨ
    //֡����
    SAI1A_Handler.FrameInit.FrameLength       = 64;                             //����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK
    SAI1A_Handler.FrameInit.ActiveFrameLength = 32;                             //����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��
    SAI1A_Handler.FrameInit.FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION;  //FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
    SAI1A_Handler.FrameInit.FSPolarity        = SAI_FS_ACTIVE_LOW;              //FS�͵�ƽ��Ч(�½���)
    SAI1A_Handler.FrameInit.FSOffset          = SAI_FS_BEFOREFIRSTBIT;          //��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼
    //SLOT����
    SAI1A_Handler.SlotInit.FirstBitOffset = 0;                                  //slotƫ��(FBOFF)Ϊ0
    SAI1A_Handler.SlotInit.SlotSize       = SAI_SLOTSIZE_32B;                   //slot��СΪ32λ
    SAI1A_Handler.SlotInit.SlotNumber     = 2;                                  //slot��Ϊ2��
    SAI1A_Handler.SlotInit.SlotActive     = SAI_SLOTACTIVE_0|SAI_SLOTACTIVE_1;  //ʹ��slot0��slot1
    //��ʼ��SAI
    HAL_SAI_Init(&SAI1A_Handler);      //��ʼ��SAIA
    __HAL_SAI_ENABLE(&SAI1A_Handler);  //ʹ��SAIA
}

/**
 * @brief SAI Block B��ʼ��, I2S, �����ֱ�׼
 * 
 * @param mode    ����ģʽ: ����SAI_MODEMASTER_TX ����SAI_MODEMASTER_RX �ӷ�SAI_MODESLAVE_TX ����SAI_MODESLAVE_RX
 * @param cpol    ����ʱ��: �½���SAI_CLOCKSTROBING_FALLINGEDGE ������SAI_CLOCKSTROBING_RISINGEDGE
 * @param datalen ���ݴ�С: SAI_DATASIZE_8/10/16/20/24/32
 */
void SAIB_Init(u32 mode, u32 cpol, u32 datalen)
{
    HAL_SAI_DeInit(&SAI1B_Handler);                                     //��λSAIB
    SAI1B_Handler.Instance            = SAI1_Block_B;                   //SAI1 Bock B
    SAI1B_Handler.Init.AudioMode      = SAI_MODESLAVE_RX;               //����SAI1����ģʽ
    SAI1B_Handler.Init.Synchro        = SAI_SYNCHRONOUS;                //��Ƶģ��ͬ��
    SAI1B_Handler.Init.OutputDrive    = SAI_OUTPUTDRIVE_ENABLE;         //����������Ƶģ�����
    SAI1B_Handler.Init.NoDivider      = SAI_MASTERDIVIDER_ENABLE;       //ʹ����ʱ�ӷ�Ƶ��(MCKDIV)
    SAI1B_Handler.Init.FIFOThreshold  = SAI_FIFOTHRESHOLD_1QF;          //����FIFO��ֵ,1/4 FIFO
    SAI1B_Handler.Init.MonoStereoMode = SAI_STEREOMODE;                 //������ģʽ
    SAI1B_Handler.Init.Protocol       = SAI_FREE_PROTOCOL;              //����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
    SAI1B_Handler.Init.DataSize       = SAI_DATASIZE_16;                //�������ݴ�С
    SAI1B_Handler.Init.FirstBit       = SAI_FIRSTBIT_MSB;               //����MSBλ����
    SAI1B_Handler.Init.ClockStrobing  = SAI_CLOCKSTROBING_FALLINGEDGE;  //������ʱ�ӵ�����/�½���ѡͨ
    //֡����
    SAI1B_Handler.FrameInit.FrameLength       = 64;                             //����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
    SAI1B_Handler.FrameInit.ActiveFrameLength = 32;                             //����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
    SAI1B_Handler.FrameInit.FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION;  //FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
    SAI1B_Handler.FrameInit.FSPolarity        = SAI_FS_ACTIVE_LOW;              //FS�͵�ƽ��Ч(�½���)
    SAI1B_Handler.FrameInit.FSOffset          = SAI_FS_BEFOREFIRSTBIT;          //��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	
    //SLOT����
    SAI1B_Handler.SlotInit.FirstBitOffset = 0;                                  //slotƫ��(FBOFF)Ϊ0
    SAI1B_Handler.SlotInit.SlotSize       = SAI_SLOTSIZE_32B;                   //slot��СΪ32λ
    SAI1B_Handler.SlotInit.SlotNumber     = 2;                                  //slot��Ϊ2��    
    SAI1B_Handler.SlotInit.SlotActive     = SAI_SLOTACTIVE_0|SAI_SLOTACTIVE_1;  //ʹ��slot0��slot1
    //��ʼ��SAIB
    HAL_SAI_Init(&SAI1B_Handler);      //��ʼ��SAIB
    SAIB_DMA_Enable();                 //ʹ��SAIB��DMA����
    __HAL_SAI_ENABLE(&SAI1B_Handler);  //ʹ��SAIB 
}

/**
 * @brief SAI�ײ�����,��������,ʱ��ʹ��
 * 
 * @param hsai SAI���
 */
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //ʹ��ʱ��
    __HAL_RCC_SAI1_CLK_ENABLE();   //ʹ��SAI1ʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();  //ʹ��GPIOEʱ��
    //����GPIO,��ʼ��PE2,3,4,5,6
    ymx_gpio_init.Pin       = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;          //���츴��
    ymx_gpio_init.Pull      = GPIO_PULLUP;                //����
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    ymx_gpio_init.Alternate = GPIO_AF6_SAI1;              //����ΪSAI1
    HAL_GPIO_Init(GPIOE, &ymx_gpio_init);
}

/*
SAI Block A����������
�����ʼ��㹫ʽ:
MCKDIV != 0: Fs = SAI_CK_x/[512*MCKDIV]
MCKDIV == 0: Fs = SAI_CK_x/256
SAI_CK_x = (HSE/pllm) * PLLSAIN/PLLSAIQ/(PLLSAIDivQ+1)
һ��HSE = 25Mhz
pllm:       ��Stm32_Clock_Init���õ�ʱ��ȷ��,һ����25
PLLSAIN:    һ����50~432 
PLLSAIQ:    2~15 
PLLSAIDivQ: 1~32
MCKDIV:     0~15 
SAI A��Ƶϵ����@pllm=25,HSE=25Mhz,��vco����Ƶ��Ϊ1Mhz 
*/
const u16 SAI_PSC_TBL[][5]=
{
    {800 ,256,5,25},	//8Khz������
    {1102,302,107,0},	//11.025Khz������ 
    {1600,426,2,52},	//16Khz������
    {2205,429,38,2},	//22.05Khz������
    {3200,426,1,52},	//32Khz������
    {4410,429,1,38},	//44.1Khz������
    {4800,467,1,38},	//48Khz������
    {8820,429,1,19},	//88.2Khz������
    {9600,467,1,19},	//96Khz������
    {17640,271,1,6},	//176.4Khz������ 
    {19200,295,6,0},	//192Khz������
};

/**
 * @brief ����SAIA������ @MCKEN
 * 
 * @param samplerate ������ ��λHz
 * @return u8 0:���óɹ� 1:����ʧ��
 */
u8 saia_samplerate_config(u32 samplerate)
{
    u8 i = 0;
    RCC_PeriphCLKInitTypeDef ymx_periph_clk_init; 
    //�����������Ƿ�֧��
    for(i=0; i<(sizeof(SAI_PSC_TBL)/10); i++)
    {
        if((samplerate/10) == SAI_PSC_TBL[i][0])
        {
            printf("����������Ϊ%5.3fKHz\r\n", (float)(samplerate/1000));
            break;
        }
    }
    //��֧�ֵĲ�����
    if(i == (sizeof(SAI_PSC_TBL)/10))
    {
        printf("����������ʧ��\r\n");
        return 1;
    }
    //����SAI1ʱ��
    ymx_periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    ymx_periph_clk_init.Sai1ClockSelection   = RCC_SAI1CLKSOURCE_PLL2;
    ymx_periph_clk_init.PLL2.PLL2M           = 25;
    ymx_periph_clk_init.PLL2.PLL2N           = (u32)SAI_PSC_TBL[i][1];
    ymx_periph_clk_init.PLL2.PLL2P           = (u32)SAI_PSC_TBL[i][2];
    HAL_RCCEx_PeriphCLKConfig(&ymx_periph_clk_init);
    //����SAIA
    __HAL_SAI_DISABLE(&SAI1A_Handler);               //�ر�SAI
    SAI1A_Handler.Init.AudioFrequency = samplerate;  //���ò���Ƶ��  
    HAL_SAI_Init(&SAI1A_Handler);                    //��ʼ��SAI
    saia_dma_enable();                               //����SAI��DMA����
    __HAL_SAI_ENABLE(&SAI1A_Handler);                //����SAI
    return 0;
} 

/**
 * @brief SAIA TX DMA����,����Ϊ˫����ģʽ,����DMA��������ж�
 * 
 * @param buf0  M0AR��ַ
 * @param buf1  M1AR��ַ
 * @param num   ÿ�δ���������
 * @param width λ��(�洢��������,ͬʱ����) 0:8λ 1:16λ 2:32λ
 */
void saia_tx_dma_init(u8* buf0, u8 *buf1, u16 num, u8 width)
{ 
    u32 memwidth = 0;
    u32 perwidth = 0;  //����ʹ洢��λ��
    switch(width)
    {
        case 0:  //8λ
            memwidth = DMA_MDATAALIGN_BYTE;
            perwidth = DMA_PDATAALIGN_BYTE;
            break;
        case 1:  //16λ
            memwidth = DMA_MDATAALIGN_HALFWORD;
            perwidth = DMA_PDATAALIGN_HALFWORD;
            break;
        case 2:  //32λ
            memwidth = DMA_MDATAALIGN_WORD;
            perwidth = DMA_PDATAALIGN_WORD;
            break;
    }
    __HAL_RCC_DMA1_CLK_ENABLE();                                         //ʹ��DMA1ʱ��
    __HAL_LINKDMA(&SAI1A_Handler, hdmatx, SAI1_TXDMA_Handler);           //��DMA��SAI��ϵ����
    SAI1_TXDMA_Handler.Instance                 = DMA1_Stream5;          //DMA1������5
    SAI1_TXDMA_Handler.Init.Request             = DMA_REQUEST_SAI1_A;    //SAI1 Bock A
    SAI1_TXDMA_Handler.Init.Direction           = DMA_MEMORY_TO_PERIPH;  //�洢��������ģʽ
    SAI1_TXDMA_Handler.Init.PeriphInc           = DMA_PINC_DISABLE;      //���������ģʽ
    SAI1_TXDMA_Handler.Init.MemInc              = DMA_MINC_ENABLE;       //�洢������ģʽ
    SAI1_TXDMA_Handler.Init.PeriphDataAlignment = perwidth;              //�������ݳ���:16/32λ
    SAI1_TXDMA_Handler.Init.MemDataAlignment    = memwidth;              //�洢�����ݳ���:16/32λ
    SAI1_TXDMA_Handler.Init.Mode                = DMA_CIRCULAR;          //ʹ��ѭ��ģʽ
    SAI1_TXDMA_Handler.Init.Priority            = DMA_PRIORITY_HIGH;     //�����ȼ�
    SAI1_TXDMA_Handler.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;  //��ʹ��FIFO
    SAI1_TXDMA_Handler.Init.MemBurst            = DMA_MBURST_SINGLE;     //�洢������ͻ������
    SAI1_TXDMA_Handler.Init.PeriphBurst         = DMA_PBURST_SINGLE;     //����ͻ�����δ���
    HAL_DMA_DeInit(&SAI1_TXDMA_Handler);  //�������ǰ������
    HAL_DMA_Init(&SAI1_TXDMA_Handler);    //��ʼ��DMA
    //����DMA
    HAL_DMAEx_MultiBufferStart(&SAI1_TXDMA_Handler, (u32)buf0, (u32)&SAI1_Block_A->DR, (u32)buf1,num);  //����˫����
    __HAL_DMA_DISABLE(&SAI1_TXDMA_Handler);                       //�ȹر�DMA 
    delay_us(10);                                                 //10us��ʱ,��ֹ-O2�Ż�������
    __HAL_DMA_ENABLE_IT(&SAI1_TXDMA_Handler, DMA_IT_TC);          //������������ж�
    __HAL_DMA_CLEAR_FLAG(&SAI1_TXDMA_Handler, DMA_FLAG_TCIF1_5);  //���DMA��������жϱ�־λ
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);                //DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

//SAI DMA�ص�����ָ��
void (*sai_tx_callback)(void);  //TX�ص�����

/**
 * @brief DMA1_Stream5�жϷ�����
 * 
 */
void DMA1_Stream5_IRQHandler(void)
{   
    if(__HAL_DMA_GET_FLAG(&SAI1_TXDMA_Handler,DMA_FLAG_TCIF1_5)!=RESET) //DMA�������
    {
        __HAL_DMA_CLEAR_FLAG(&SAI1_TXDMA_Handler,DMA_FLAG_TCIF1_5);     //���DMA��������жϱ�־λ
        if(sai_tx_callback!=NULL)sai_tx_callback();						//ִ�лص�����,��ȡ���ݵȲ����������洦��  
    }  											 
}  

/**
 * @brief SAI��ʼ����
 * 
 */
void sai_play_start(void)
{   	
    __HAL_DMA_ENABLE(&SAI1_TXDMA_Handler);	//����DMA TX����  			
}

/**
 * @brief �ر�SAI����
 * 
 */
void sai_play_stop(void)
{   
    __HAL_DMA_DISABLE(&SAI1_TXDMA_Handler);	//��������  	 	 
} 
