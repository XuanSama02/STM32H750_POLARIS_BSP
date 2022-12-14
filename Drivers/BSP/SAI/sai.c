#include "sai.h"
#include "delay.h"

SAI_HandleTypeDef SAI1A_Handler;       //SAI1 Block A句柄
DMA_HandleTypeDef SAI1_TXDMA_Handler;  //DMA发送句柄
DMA_HandleTypeDef SAI1_RXDMA_Handler;  //DMA接收句柄

/**
 * @brief 开启SAIB的DMA功能
 * 
 */
void saia_dma_enable(void)
{
    u32 tempreg = 0;
    tempreg  = SAI1_Block_A->CR1;  //先读出以前的设置
    tempreg |= 1<<17;              //使能DMA
    SAI1_Block_A->CR1 = tempreg;   //写入CR1寄存器中
}

/**
 * @brief SAI Block A初始化, I2S, 飞利浦标准
 * 
 * @param mode    工作模式: 主发SAI_MODEMASTER_TX 主收SAI_MODEMASTER_RX 从发SAI_MODESLAVE_TX 从收SAI_MODESLAVE_RX
 * @param cpol    数据时钟: 下降沿SAI_CLOCKSTROBING_FALLINGEDGE 上升沿SAI_CLOCKSTROBING_RISINGEDGE
 * @param datalen 数据大小: SAI_DATASIZE_8/10/16/20/24/32
 */
void saia_init(u32 mode, u32 cpol, u32 datalen)
{
    HAL_SAI_DeInit(&SAI1A_Handler);                                //复位SAIA
    SAI1A_Handler.Instance            = SAI1_Block_A;              //SAI1 Bock A
    SAI1A_Handler.Init.AudioMode      = mode;                      //设置SAI1工作模式
    SAI1A_Handler.Init.Synchro        = SAI_ASYNCHRONOUS;          //音频模块异步
    SAI1A_Handler.Init.OutputDrive    = SAI_OUTPUTDRIVE_ENABLE;    //立即驱动音频模块输出
    SAI1A_Handler.Init.NoDivider      = SAI_MASTERDIVIDER_ENABLE;  //使能主时钟分频器(MCKDIV)
    SAI1A_Handler.Init.FIFOThreshold  = SAI_FIFOTHRESHOLD_1QF;     //设置FIFO阈值,1/4 FIFO
    SAI1A_Handler.Init.MonoStereoMode = SAI_STEREOMODE;            //立体声模式
    SAI1A_Handler.Init.Protocol       = SAI_FREE_PROTOCOL;         //设置SAI1协议为:自由协议(支持I2S/LSB/MSB/TDM/PCM/DSP等协议)
    SAI1A_Handler.Init.DataSize       = datalen;                   //设置数据大小
    SAI1A_Handler.Init.FirstBit       = SAI_FIRSTBIT_MSB;          //数据MSB位优先
    SAI1A_Handler.Init.ClockStrobing  = cpol;                      //数据在时钟的上升/下降沿选通
    //帧设置
    SAI1A_Handler.FrameInit.FrameLength       = 64;                             //设置帧长度为64,左通道32个SCK,右通道32个SCK
    SAI1A_Handler.FrameInit.ActiveFrameLength = 32;                             //设置帧同步有效电平长度,在I2S模式下=1/2帧长
    SAI1A_Handler.FrameInit.FSDefinition      = SAI_FS_CHANNEL_IDENTIFICATION;  //FS信号为SOF信号+通道识别信号
    SAI1A_Handler.FrameInit.FSPolarity        = SAI_FS_ACTIVE_LOW;              //FS低电平有效(下降沿)
    SAI1A_Handler.FrameInit.FSOffset          = SAI_FS_BEFOREFIRSTBIT;          //在slot0的第一位的前一位使能FS,以匹配飞利浦标准
    //SLOT设置
    SAI1A_Handler.SlotInit.FirstBitOffset = 0;                                  //slot偏移(FBOFF)为0
    SAI1A_Handler.SlotInit.SlotSize       = SAI_SLOTSIZE_32B;                   //slot大小为32位
    SAI1A_Handler.SlotInit.SlotNumber     = 2;                                  //slot数为2个
    SAI1A_Handler.SlotInit.SlotActive     = SAI_SLOTACTIVE_0|SAI_SLOTACTIVE_1;  //使能slot0和slot1
    //初始化SAI
    HAL_SAI_Init(&SAI1A_Handler);      //初始化SAIA
    __HAL_SAI_ENABLE(&SAI1A_Handler);  //使能SAIA
}

/**
 * @brief SAI底层驱动,引脚配置,时钟使能
 * 
 * @param hsai SAI句柄
 */
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //使能时钟
    __HAL_RCC_SAI1_CLK_ENABLE();   //使能SAI1时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();  //使能GPIOE时钟
    //配置GPIO,初始化PE2,3,4,5,6
    ymx_gpio_init.Pin       = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;          //推挽复用
    ymx_gpio_init.Pull      = GPIO_PULLUP;                //上拉
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    ymx_gpio_init.Alternate = GPIO_AF6_SAI1;              //复用为SAI1
    HAL_GPIO_Init(GPIOE, &ymx_gpio_init);
}

/*
SAI Block A采样率设置
采样率计算公式:
MCKDIV != 0: Fs = SAI_CK_x/[512*MCKDIV]
MCKDIV == 0: Fs = SAI_CK_x/256
SAI_CK_x = (HSE/pllm) * PLLSAIN/PLLSAIQ/(PLLSAIDivQ+1)
一般HSE = 25Mhz
pllm:       在Stm32_Clock_Init设置的时候确定,一般是25
PLLSAIN:    一般是50~432 
PLLSAIQ:    2~15 
PLLSAIDivQ: 1~32
MCKDIV:     0~15 
SAI A分频系数表@pllm=25,HSE=25Mhz,即vco输入频率为1Mhz 
*/
const u16 SAI_PSC_TBL[][5]=
{
    {800 ,256,5,25},	//8Khz采样率
    {1102,302,107,0},	//11.025Khz采样率 
    {1600,426,2,52},	//16Khz采样率
    {2205,429,38,2},	//22.05Khz采样率
    {3200,426,1,52},	//32Khz采样率
    {4410,429,1,38},	//44.1Khz采样率
    {4800,467,1,38},	//48Khz采样率
    {8820,429,1,19},	//88.2Khz采样率
    {9600,467,1,19},	//96Khz采样率
    {17640,271,1,6},	//176.4Khz采样率 
    {19200,295,6,0},	//192Khz采样率
};

/**
 * @brief 设置SAIA采样率 @MCKEN
 * 
 * @param samplerate 采样率 单位Hz
 * @return u8 0:设置成功 1:设置失败
 */
u8 saia_samplerate_config(u32 samplerate)
{
    u8 i = 0;
    RCC_PeriphCLKInitTypeDef ymx_periph_clk_init; 
    //看看采样率是否支持
    for(i=0; i<(sizeof(SAI_PSC_TBL)/10); i++)
    {
        if((samplerate/10) == SAI_PSC_TBL[i][0])
        {
            printf("采样率设置为%5.3fKHz\r\n", (float)(samplerate/1000));
            break;
        }
    }
    //不支持的采样率
    if(i == (sizeof(SAI_PSC_TBL)/10))
    {
        printf("采样率设置失败\r\n");
        return 1;
    }
    //配置SAI1时钟
    ymx_periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    ymx_periph_clk_init.Sai1ClockSelection   = RCC_SAI1CLKSOURCE_PLL2;
    ymx_periph_clk_init.PLL2.PLL2M           = 25;
    ymx_periph_clk_init.PLL2.PLL2N           = (u32)SAI_PSC_TBL[i][1];
    ymx_periph_clk_init.PLL2.PLL2P           = (u32)SAI_PSC_TBL[i][2];
    HAL_RCCEx_PeriphCLKConfig(&ymx_periph_clk_init);
    //重启SAIA
    __HAL_SAI_DISABLE(&SAI1A_Handler);               //关闭SAI
    SAI1A_Handler.Init.AudioFrequency = samplerate;  //设置播放频率  
    HAL_SAI_Init(&SAI1A_Handler);                    //初始化SAI
    saia_dma_enable();                               //开启SAI的DMA功能
    __HAL_SAI_ENABLE(&SAI1A_Handler);                //开启SAI
    return 0;
} 

/**
 * @brief SAIA TX DMA配置,设置为双缓冲模式,开启DMA传输完成中断
 * 
 * @param buf0  M0AR地址
 * @param buf1  M1AR地址
 * @param num   每次传输数据量
 * @param width 位宽(存储器和外设,同时设置) 0:8位 1:16位 2:32位
 */
void saia_tx_dma_init(u8* buf0, u8 *buf1, u16 num, u8 width)
{ 
    u32 memwidth = 0;
    u32 perwidth = 0;  //外设和存储器位宽
    switch(width)
    {
        case 0:  //8位
            memwidth = DMA_MDATAALIGN_BYTE;
            perwidth = DMA_PDATAALIGN_BYTE;
            break;
        case 1:  //16位
            memwidth = DMA_MDATAALIGN_HALFWORD;
            perwidth = DMA_PDATAALIGN_HALFWORD;
            break;
        case 2:  //32位
            memwidth = DMA_MDATAALIGN_WORD;
            perwidth = DMA_PDATAALIGN_WORD;
            break;
    }
    __HAL_RCC_DMA1_CLK_ENABLE();                                         //使能DMA1时钟
    __HAL_LINKDMA(&SAI1A_Handler, hdmatx, SAI1_TXDMA_Handler);           //将DMA与SAI联系起来
    SAI1_TXDMA_Handler.Instance                 = DMA1_Stream5;          //DMA1数据流5
    SAI1_TXDMA_Handler.Init.Request             = DMA_REQUEST_SAI1_A;    //SAI1 Bock A
    SAI1_TXDMA_Handler.Init.Direction           = DMA_MEMORY_TO_PERIPH;  //存储器到外设模式
    SAI1_TXDMA_Handler.Init.PeriphInc           = DMA_PINC_DISABLE;      //外设非增量模式
    SAI1_TXDMA_Handler.Init.MemInc              = DMA_MINC_ENABLE;       //存储器增量模式
    SAI1_TXDMA_Handler.Init.PeriphDataAlignment = perwidth;              //外设数据长度:16/32位
    SAI1_TXDMA_Handler.Init.MemDataAlignment    = memwidth;              //存储器数据长度:16/32位
    SAI1_TXDMA_Handler.Init.Mode                = DMA_CIRCULAR;          //使用循环模式
    SAI1_TXDMA_Handler.Init.Priority            = DMA_PRIORITY_HIGH;     //高优先级
    SAI1_TXDMA_Handler.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;  //不使用FIFO
    SAI1_TXDMA_Handler.Init.MemBurst            = DMA_MBURST_SINGLE;     //存储器单次突发传输
    SAI1_TXDMA_Handler.Init.PeriphBurst         = DMA_PBURST_SINGLE;     //外设突发单次传输
    HAL_DMA_DeInit(&SAI1_TXDMA_Handler);  //先清除以前的设置
    HAL_DMA_Init(&SAI1_TXDMA_Handler);    //初始化DMA
    //配置DMA
    HAL_DMAEx_MultiBufferStart(&SAI1_TXDMA_Handler, (u32)buf0, (u32)&SAI1_Block_A->DR, (u32)buf1,num);  //开启双缓冲
    __HAL_DMA_DISABLE(&SAI1_TXDMA_Handler);                       //先关闭DMA 
    delay_us(10);                                                 //10us延时,防止-O2优化出问题
    __HAL_DMA_ENABLE_IT(&SAI1_TXDMA_Handler, DMA_IT_TC);          //开启传输完成中断
    __HAL_DMA_CLEAR_FLAG(&SAI1_TXDMA_Handler, DMA_FLAG_TCIF1_5);  //清除DMA传输完成中断标志位
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);                //DMA中断优先级
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

//SAI DMA回调函数指针
void (*sai_tx_callback)(void);  //TX回调函数

/**
 * @brief DMA1_Stream5中断服务函数
 * 
 */
void DMA1_Stream5_IRQHandler(void)
{   
    if(__HAL_DMA_GET_FLAG(&SAI1_TXDMA_Handler,DMA_FLAG_TCIF1_5)!=RESET) //DMA传输完成
    {
        __HAL_DMA_CLEAR_FLAG(&SAI1_TXDMA_Handler,DMA_FLAG_TCIF1_5);     //清除DMA传输完成中断标志位
        if(sai_tx_callback!=NULL)sai_tx_callback();						//执行回调函数,读取数据等操作在这里面处理  
    }  											 
}  

/**
 * @brief SAI开始播放
 * 
 */
void sai_play_start(void)
{   	
    __HAL_DMA_ENABLE(&SAI1_TXDMA_Handler);	//开启DMA TX传输  			
}

/**
 * @brief 关闭SAI播放
 * 
 */
void sai_play_stop(void)
{   
    __HAL_DMA_DISABLE(&SAI1_TXDMA_Handler);	//结束播放  	 	 
} 
