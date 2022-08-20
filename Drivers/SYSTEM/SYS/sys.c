#include "sys.h"

/*ʱ�����ú���
Fvco = Fs * (plln/pllm);
Fsys = Fvco/pllp = Fs * (plln/(pllm*pllp));
Fq   = Fvco/pllq = Fs * (plln/(pllm*pllq));

Fvco:VCOƵ��
Fsys: ϵͳʱ��Ƶ��,Ҳ��PLL1��p��Ƶ���ʱ��Ƶ��
Fq:   PLL1��q��Ƶ���ʱ��Ƶ��
Fs:   PLL����ʱ��Ƶ��,������HSI,CSI,HSE��. 

CPUƵ��(rcc_c_ck) = sys_d1cpre_ck = 400Mhz 
rcc_aclk = rcc_hclk3 = 200Mhz
AHB1/2/3/4(rcc_hclk1/2/3/4) = 200Mhz  
APB1/2/3/4(rcc_pclk1/2/3/4) = 100Mhz  
FMCʱ��Ƶ�� = pll2_r_ck = ((25/25)*512/2) = 256Mhz

�ⲿ����Ϊ25M��ʱ��,�Ƽ�ֵ:plln=160,pllm=5,pllp=2,pllq=2.
�õ�:Fvco=25*(160/5)=800Mhz
     Fsys=800/2=400Mhz
     Fq=800/2=400Mhz
*/

/**
 * @brief STM32H7ʱ�����ú���,�ⲿ����Ϊ25MHzʱ,�Ƽ�����(��Ƶ400MHz):160,5,2,2
 * 
 * @param plln PLL1�ı�Ƶϵ��(PLL��Ƶ):4~512
 * @param pllm PLL1��Ԥ��Ƶϵ��(����PLL֮ǰ�ķ�Ƶ):2~63
 * @param pllp PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ),��Ƶ����Ϊϵͳʱ��:2~128,��Ϊ2�ı���
 * @param pllq PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ):1~128
 * @return u8 0:�ɹ� 1:ʧ��
 */
u8 stm32_clock_init(u32 plln, u32 pllm, u32 pllp, u32 pllq)
{
    RCC_OscInitTypeDef ymx_osc_init;
    RCC_ClkInitTypeDef ymx_clk_init;

    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while((PWR->D3CR&(PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY);

    ymx_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;  //HSE�ⲿ��������
    ymx_osc_init.HSEState       = RCC_HSE_ON;              //HSEʱ�ӿ���
    ymx_osc_init.HSIState       = RCC_HSI_OFF;             //SHIʱ�ӹر�
    ymx_osc_init.CSIState       = RCC_CSI_OFF;             //CSIʱ�ӹر�
    ymx_osc_init.PLL.PLLState   = RCC_PLL_ON;              //���໷����
    ymx_osc_init.PLL.PLLSource  = RCC_PLLSOURCE_HSE;       //���໷ʱ����ԴHSE

    ymx_osc_init.PLL.PLLN = plln;
    ymx_osc_init.PLL.PLLM = pllm;
    ymx_osc_init.PLL.PLLP = pllp;
    ymx_osc_init.PLL.PLLQ = pllq;

    ymx_osc_init.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;     //PLL1�����Χ
    ymx_osc_init.PLL.PLLRGE    = RCC_PLL1VCIRANGE_2;  //PLL1���뷶Χ
    if(HAL_RCC_OscConfig(&ymx_osc_init) != HAL_OK)    //����ʱ��Դ
        return 1;

    qspi_memmap_mode(); //QSPI�ڴ�ӳ��ģʽ,��Ҫ��ʱ�ӳ�ʼ��֮ǰ����,������и�������

    ymx_clk_init.ClockType=(RCC_CLOCKTYPE_SYSCLK  |\
                            RCC_CLOCKTYPE_HCLK    |\
                            RCC_CLOCKTYPE_D1PCLK1 |\
                            RCC_CLOCKTYPE_PCLK1   |\
                            RCC_CLOCKTYPE_PCLK2   |\
                            RCC_CLOCKTYPE_D3PCLK1);

    ymx_clk_init.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;  //ϵͳʱ����Դ:���໷���
    ymx_clk_init.SYSCLKDivider  = RCC_SYSCLK_DIV1;          //ϵͳʱ�ӷ�Ƶϵ��:1
    ymx_clk_init.AHBCLKDivider  = RCC_HCLK_DIV2;            //HCLKʱ�ӷ�Ƶϵ��:2
    ymx_clk_init.APB1CLKDivider = RCC_APB1_DIV2;            //APB1ʱ�ӷ�Ƶϵ��:2
    ymx_clk_init.APB2CLKDivider = RCC_APB2_DIV2;            //APB2ʱ�ӷ�Ƶϵ��:2
    ymx_clk_init.APB3CLKDivider = RCC_APB3_DIV2;            //APB3ʱ�ӷ�Ƶϵ��:2
    ymx_clk_init.APB4CLKDivider = RCC_APB4_DIV4;            //APB4ʱ�ӷ�Ƶϵ��:4
    if(HAL_RCC_ClockConfig(&ymx_clk_init, FLASH_LATENCY_2) != HAL_OK)  //����ϵͳʱ��
        return 1;

    __HAL_RCC_CSI_ENABLE();         //ʹ��CSIʱ��
    __HAL_RCC_SYSCFG_CLK_ENABLE();  //ʹ��APB4_SYSCFGʱ��
    HAL_EnableCompensationCell();   //ʹ��IO������Ԫ

    return 0;  //���óɹ�
}

/**
 * @brief ʹ��STM32H7��L1Cache,Ĭ�Ͽ���DCache͸д
 * 
 */
void stm32_cache_enable(void)
{
    SCB_EnableICache();  //ʹ��ICache
    SCB_EnableDCache();  //ʹ��DCache   
    SCB->CACR|=1<<2;     //ǿ��DCache͸д,�粻����,ʵ��ʹ���п���������������
}

/**
 * @brief �ж�ICache�Ƿ��
 * 
 * @return u8 0:�ر� 1:����
 */
u8 stm32_icache_stastu(void)
{
    u8 sta;
    sta=((SCB->CCR)>>17)&0X01;
    return sta;
}

/**
 * @brief �ж�DCache�Ƿ��
 * 
 * @return u8 0:�ر� 1:����
 */
u8 stm32_dcache_status(void)
{
    u8 sta;
    sta=((SCB->CCR)>>16)&0X01;
    return sta;
}

/**
 * @brief QSPI�����ڴ�ӳ��ģʽ,Ϊ�����ļ�����,��GPIO����,���������ʹ�üĴ�������
 * 
 */
void qspi_memmap_mode(void)
{
    u32  tempreg   = 0;
    vu32 *data_reg = &QUADSPI->DR;
    GPIO_InitTypeDef ymx_gpio_init;

    RCC->AHB4ENR|=1<<1;   //ʹ��GPIOBʱ��
    RCC->AHB4ENR|=1<<5;   //ʹ��GPIOFʱ��
    RCC->AHB3ENR|=1<<14;  //ʹ��QSPIʱ��

    ymx_gpio_init.Pin       = GPIO_PIN_6;                 //PB6 AF10
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;            //�����������
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    ymx_gpio_init.Pull      = GPIO_NOPULL;                //����������
    ymx_gpio_init.Alternate = GPIO_AF10_QUADSPI;          //����ΪQSPI
    HAL_GPIO_Init(GPIOB, &ymx_gpio_init);

    ymx_gpio_init.Pin       = GPIO_PIN_2;                 //PB2 AF9	
    ymx_gpio_init.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &ymx_gpio_init);

    ymx_gpio_init.Pin       = GPIO_PIN_6|GPIO_PIN_7;      //PF6,7 AF9
    ymx_gpio_init.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &ymx_gpio_init);

    ymx_gpio_init.Pin       = GPIO_PIN_8|GPIO_PIN_9;      //PF8,9 AF10
    ymx_gpio_init.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOF, &ymx_gpio_init);

    //QSPI����,�ο�QSPIʵ���QSPI_Init����
    RCC->AHB3RSTR |= 1<<14;       //��λQSPI
    RCC->AHB3RSTR &= ~(1<<14);    //ֹͣ��λQSPI
    while(QUADSPI->SR&(1<<5));    //�ȴ�BUSYλ����
    QUADSPI->CR    = 0X01000310;  //����CR�Ĵ���
    QUADSPI->DCR   = 0X00160401;  //����DCR�Ĵ���
    QUADSPI->CR   |= 1<<0;        //ʹ��QSPI

    /*
    ע��:QSPI QEλ��ʹ��,��QSPI��д�㷨����,���Ѿ�������
    ����,������Բ�������QEλ,������Ҫ�����QEλ��1�Ĵ���
    ����,�������ͨ������������,ֱ����¼���ⲿQSPI FLASH,�ǲ����õ�
    �����ֱ����¼���ⲿQSPI FLASHҲ������,����Ҫ���������QEλ��1�Ĵ���
    */

    //W25QXX����QPIģʽ��0X38ָ�
    while(QUADSPI->SR&(1<<5));         //�ȴ�BUSYλ����
    QUADSPI->CCR  = 0X00000138;        //����0X38ָ��,W25QXX����QPIģʽ
    while((QUADSPI->SR&(1<<1)) == 0);  //�ȴ�ָ������
    QUADSPI->FCR |= 1<<1;              //���������ɱ�־λ

    //W25QXXдʹ�ܣ�0X06ָ�
    while(QUADSPI->SR & (1<<5));       //�ȴ�BUSYλ����
    QUADSPI->CCR  = 0X00000106;        //����0X06ָ��,W25QXXдʹ��
    while((QUADSPI->SR&(1<<1)) == 0);  //�ȴ�ָ������
    QUADSPI->FCR |= 1<<1;              //���������ɱ�־λ

    //W25QXX����QPI��ض�������0XC0��
    while(QUADSPI->SR & (1<<5));       //�ȴ�BUSYλ���� 
    QUADSPI->CCR  = 0X030003C0;        //����0XC0ָ��,W25QXX����������
    QUADSPI->DLR  = 0;
    while((QUADSPI->SR&(1<<2)) == 0);  //�ȴ�FTF
    *(vu8 *)data_reg = 3<<4;           //����P4&P5=11,8��dummy clocks,104M
    QUADSPI->CR  |= 1<<2;              //��ֹ����
    while((QUADSPI->SR&(1<<1)) == 0);  //�ȴ����ݷ������
    QUADSPI->FCR |= 1<<1;              //���������ɱ�־λ
    while(QUADSPI->SR & (1<<5));       //�ȴ�BUSYλ����

    //MemroyMapģʽ����
    while(QUADSPI->SR & (1<<5));  //�ȴ�BUSYλ����
    QUADSPI->ABR = 0;             //�����ֽ�����Ϊ0��ʵ���Ͼ���W25Q 0XEBָ���,M0~M7=0
    tempreg  = 0XEB;              //INSTRUCTION[7:0]=0XEB,����0XEBָ��(Fast Read QUAD I/O)
    tempreg |= 3<<8;              //IMODE  [1:0]=3, ���ߴ���ָ��
    tempreg |= 3<<10;             //ADDRESS[1:0]=3, ���ߴ����ַ
    tempreg |= 2<<12;             //ADSIZE [1:0]=2, 24λ��ַ����
    tempreg |= 3<<14;             //ABMODE [1:0]=3, ���ߴ��佻���ֽ�
    tempreg |= 0<<16;             //ABSIZE [1:0]=0, 8λ�����ֽ�(M0~M7)
    tempreg |= 6<<18;             //DCYC   [4:0]=6, 6��dummy����
    tempreg |= 3<<24;             //DMODE  [1:0]=3, ���ߴ�������
    tempreg |= 3<<26;             //FMODE  [1:0]=3, �ڴ�ӳ��ģʽ
    QUADSPI->CCR = tempreg;       //����CCR�Ĵ���

    //����QSPI FLASH�ռ��MPU����
    SCB->SHCSR &= ~(1<<16);       //��ֹMemManage
    MPU->CTRL  &= ~(1<<0);        //��ֹMPU
    MPU->RNR    = 0;              //���ñ���������Ϊ0(1~7���Ը������ڴ���)
    MPU->RBAR   = 0X90000000;     //����ַΪ0X9000 000,��QSPI����ʼ��ַ
    MPU->RASR   = 0X0303002D;     //������ر�������(��ֹ����,����cache,������),���MPUʵ��Ľ���
    MPU->CTRL   = (1<<2)|(1<<0);  //ʹ��PRIVDEFENA,ʹ��MPU
    SCB->SHCSR |= 1<<16;          //ʹ��MemManage
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief ��������ʾ�����ʱ��˺����������������ļ���������
 * 
 * @param file ָ��Դ�ļ�
 * @param line ָ�����ļ��е�����
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif

#if defined(__clang__) //ʹ��V6������(clang)

/**
 * @brief ִ��WFIָ��
 * 
 */
void __attribute__((noinline)) WFI_SET(void)
{
    __asm__("wfi");
}

/**
 * @brief �ر������ж�
 * 
 */
void __attribute__((noinline)) INTX_DISABLE(void)
{
    __asm__("cpsid i \t\n"
            "bx lr");
}

/**
 * @brief ���������ж�
 * 
 */
void __attribute__((noinline)) INTX_ENABLE(void)
{
    __asm__("cpsie i \t\n"
            "bx lr");
}

/**
 * @brief ���ö�ջ��ַ
 * 
 * @param addr ջ����ַ
 */
void __attribute__((noinline)) MSR_MSP(u32 addr) 
{
    __asm__("msr msp, r0 \t\n"
            "bx r14");
}

#elif defined (__CC_ARM)    //ʹ��V5������(ARMCC)

/**
 * @brief ִ��WFIָ��
 * 
 */
__asm void WFI_SET(void)
{
    WFI;		  
}

/**
 * @brief �ر������ж�
 * 
 */
__asm void INTX_DISABLE(void)
{
    CPSID   I
    BX      LR	  
}

/**
 * @brief ���������ж�
 * 
 */
__asm void INTX_ENABLE(void)
{
    CPSIE   I
    BX      LR  
}

/**
 * @brief ���ö�ջ��ַ
 * 
 * @param addr ջ����ַ
 */
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0  //set Main Stack value
    BX r14
}

#endif
