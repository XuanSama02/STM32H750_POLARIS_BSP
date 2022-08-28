#include "sdram.h"
#include "delay.h"

SDRAM_HandleTypeDef SDRAM_Handler;  //SDRAM���

/**
 * @brief SDRAM��ʼ��
 * 
 */
void sdram_init(void)
{
    FMC_SDRAM_TimingTypeDef ymx_sdram_timing_init;
    //����SDRAM
    SDRAM_Handler.Instance                = FMC_SDRAM_DEVICE;                    //SDRAM��BANK5,6
    SDRAM_Handler.Init.SDBank             = FMC_SDRAM_BANK1;                     //SDRAM����BANK5��
    SDRAM_Handler.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9;         //������
    SDRAM_Handler.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_13;           //������
    SDRAM_Handler.Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32;          //���ݿ��Ϊ32λ
    SDRAM_Handler.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;        //һ��4��BANK
    SDRAM_Handler.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_2;             //CASΪ2
    SDRAM_Handler.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;  //ʧ��д����
    SDRAM_Handler.Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;            //SDRAMʱ��ΪHCLK/2=200M/2=100M=10ns
    SDRAM_Handler.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;             //ʹ��ͻ��
    SDRAM_Handler.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;             //��ͨ����ʱ
    //����SDRAMʱ��
    ymx_sdram_timing_init.LoadToActiveDelay    = 2;  //����ģʽ�Ĵ���������ʱ����ӳ�Ϊ2��ʱ������
    ymx_sdram_timing_init.ExitSelfRefreshDelay = 8;  //�˳���ˢ���ӳ�Ϊ8��ʱ������
    ymx_sdram_timing_init.SelfRefreshTime      = 6;  //��ˢ��ʱ��Ϊ6��ʱ������
    ymx_sdram_timing_init.RowCycleDelay        = 6;  //��ѭ���ӳ�Ϊ6��ʱ������
    ymx_sdram_timing_init.WriteRecoveryTime    = 2;  //�ָ��ӳ�Ϊ2��ʱ������
    ymx_sdram_timing_init.RPDelay              = 2;  //��Ԥ����ӳ�Ϊ2��ʱ������
    ymx_sdram_timing_init.RCDDelay             = 2;  //�е����ӳ�Ϊ2��ʱ������
    HAL_SDRAM_Init(&SDRAM_Handler, &ymx_sdram_timing_init);
    //SDRAM��ʼ������
    sdram_init_sequence(&SDRAM_Handler);
}

/**
 * @brief SDRAM�ײ�����,��������,ʱ��ʹ��
 * 
 * @param hsdram sdram���
 */
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //ʹ��ʱ��
    __HAL_RCC_SYSCFG_CLK_ENABLE();  //ʹ��SYSCFGʱ��
    __HAL_RCC_FMC_CLK_ENABLE();     //ʹ��FMCʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();   //ʹ��GPIOEʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();   //ʹ��GPIOFʱ��
    __HAL_RCC_GPIOG_CLK_ENABLE();   //ʹ��GPIOGʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();   //ʹ��GPIOHʱ��
    __HAL_RCC_GPIOI_CLK_ENABLE();   //ʹ��GPIOIʱ��
    //��ʼ��PC0,2,3
    ymx_gpio_init.Pin       = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3;
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;            //��������
    ymx_gpio_init.Pull      = GPIO_PULLUP;                //����
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    ymx_gpio_init.Alternate = GPIO_AF12_FMC;              //����ΪFMC
    HAL_GPIO_Init(GPIOC, &ymx_gpio_init);
    //��ʼ��PD0,1,8,9,10,14,15
    ymx_gpio_init.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &ymx_gpio_init);
    //��ʼ��PE0,1,7,8,9,10,11,12,13,14,15
    ymx_gpio_init.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &ymx_gpio_init);
    //��ʼ��PF0,1,2,3,4,5,11,12,13,14,15
    ymx_gpio_init.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &ymx_gpio_init);
    //��ʼ��PG0,1,2,4,5,8,15
    ymx_gpio_init.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &ymx_gpio_init);
    //��ʼ��PH8,9,10,11,12,13,14,15
    ymx_gpio_init.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH, &ymx_gpio_init);
    //��ʼ��PI0,1,2,3,4,5,6,7,9,10
    ymx_gpio_init.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI, &ymx_gpio_init);
}

/**
 * @brief ����SDRAM��ʼ������
 * 
 * @param hsdram SDRAM���
 */
void sdram_init_sequence(SDRAM_HandleTypeDef *hsdram)
{
    u32 temp = 0;
    //SDRAM��������ʼ������Ժ���Ҫ��������˳���ʼ��SDRAM
    sdram_send_cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);        //ʱ������ʹ��
    delay_us(500);                                            //������ʱ200us
    sdram_send_cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);              //�����д洢��Ԥ���
    sdram_send_cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);  //������ˢ�´���
    /*
    ����ģʽ�Ĵ���,SDRAM��bit0~bit2Ϊָ��ͻ�����ʵĳ��ȣ�
    bit3Ϊָ��ͻ�����ʵ����ͣ�bit4~bit6ΪCASֵ��bit7��bit8Ϊ����ģʽ
    bit9Ϊָ����дͻ��ģʽ��bit10��bit11λ����λ
    */
    temp=(u32)SDRAM_MODEREG_BURST_LENGTH_4          |    //����ͻ������:1(������1/2/4/8)
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |    //����ͻ������:����(����������/����)
              SDRAM_MODEREG_CAS_LATENCY_2           |    //����CASֵ:2(������2/3)
              SDRAM_MODEREG_OPERATING_MODE_STANDARD |    //���ò���ģʽ:0,��׼ģʽ
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;      //����ͻ��дģʽ:1,�������
    sdram_send_cmd(0,FMC_SDRAM_CMD_LOAD_MODE, 1, temp);  //����SDRAM��ģʽ�Ĵ���
    /*
    ˢ��Ƶ�ʼ�����(��SDCLKƵ�ʼ���),���㷽��:
    COUNT = SDRAMˢ������/����-20 = SDRAMˢ������(us)*SDCLKƵ��(Mhz)/����
    ����ʹ�õ�SDRAMˢ������Ϊ64ms, SDCLK=200/2=100Mhz, ����Ϊ8192(2^13)
    ����,COUNT = 64*1000*100/8192-20 = 761  (W9825G6JH-6),ԭ��:677
    */
    HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler, 761);
}

/**
 * @brief ��ָ����ַ(addr+Bank5_SDRAM_ADDR)��ʼ,����д��n���ֽ�
 * 
 * @param pbuffer �ֽ�ָ��
 * @param addr    Ҫд��ĵ�ַ
 * @param n       Ҫд����ֽ���
 */
void sdram_write(u8 *pbuffer, u32 addr, u32 n)
{
    for(; n!=0; n--)
    {
        *(vu8*)(Bank5_SDRAM_ADDR+addr) = *pbuffer;
        addr++;
        pbuffer++;
    }
}

/**
 * @brief ��ָ����ַ((addr+Bank5_SDRAM_ADDR))��ʼ,��������n���ֽ�
 * 
 * @param pbuffer �ֽ�ָ��
 * @param addr    Ҫ��������ʼ��ַ
 * @param n       Ҫд����ֽ���
 */
void sdram_read(u8 *pbuffer, u32 addr, u32 n)
{
    for(; n!=0; n--)
    {
        *pbuffer++ = *(vu8*)(Bank5_SDRAM_ADDR+addr);
        addr++;
    }
}

/**
 * @brief ��SDRAM��������
 * 
 * @param bankx   0:��BANK5�ϵ�SDRAM����ָ�� 1:��BANK6�ϵ�SDRAM����ָ��
 * @param cmd     ָ��(0:����ģʽ/ 1:ʱ������ʹ��/ 2:Ԥ������д洢��/ 3:�Զ�ˢ��/ 4:����ģʽ�Ĵ���/ 5:��ˢ��/ 6:����)
 * @param refresh ��ˢ�´���
 * @param regval  ģʽ�Ĵ����Ķ���
 * @return u8 0:�ɹ� 1:ʧ��
 */
u8 sdram_send_cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval)
{
    u32 target_bank = 0;
    FMC_SDRAM_CommandTypeDef ymx_sdram_command;
    if(bankx == 0)
        target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
    else if(bankx == 1)
        target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
    ymx_sdram_command.CommandMode            = cmd;          //����
    ymx_sdram_command.CommandTarget          = target_bank;  //Ŀ��SDRAM�洢����
    ymx_sdram_command.AutoRefreshNumber      = refresh;      //��ˢ�´���
    ymx_sdram_command.ModeRegisterDefinition = regval;       //Ҫд��ģʽ�Ĵ�����ֵ
    if(HAL_SDRAM_SendCommand(&SDRAM_Handler, &ymx_sdram_command, 0XFFFF) == HAL_OK)  //��SDRAM��������
        return 0;
    else
        return 1;
}
