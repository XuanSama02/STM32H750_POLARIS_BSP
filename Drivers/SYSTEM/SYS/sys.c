#include "sys.h"

/*时钟设置函数
Fvco = Fs * (plln/pllm);
Fsys = Fvco/pllp = Fs * (plln/(pllm*pllp));
Fq   = Fvco/pllq = Fs * (plln/(pllm*pllq));

Fvco:VCO频率
Fsys: 系统时钟频率,也是PLL1的p分频输出时钟频率
Fq:   PLL1的q分频输出时钟频率
Fs:   PLL输入时钟频率,可以是HSI,CSI,HSE等. 

CPU频率(rcc_c_ck) = sys_d1cpre_ck = 400Mhz 
rcc_aclk = rcc_hclk3 = 200Mhz
AHB1/2/3/4(rcc_hclk1/2/3/4) = 200Mhz  
APB1/2/3/4(rcc_pclk1/2/3/4) = 100Mhz  
FMC时钟频率 = pll2_r_ck = ((25/25)*512/2) = 256Mhz

外部晶振为25M的时候,推荐值:plln=160,pllm=5,pllp=2,pllq=2.
得到:Fvco=25*(160/5)=800Mhz
     Fsys=800/2=400Mhz
     Fq=800/2=400Mhz
*/

/**
 * @brief STM32H7时钟配置函数,外部晶振为25MHz时,推荐参数(主频400MHz):160,5,2,2
 * 
 * @param plln PLL1的倍频系数(PLL倍频):4~512
 * @param pllm PLL1的预分频系数(进入PLL之前的分频):2~63
 * @param pllp PLL1的p分频系数(PLL之后的分频),分频后作为系统时钟:2~128,且为2的倍数
 * @param pllq PLL1的q分频系数(PLL之后的分频):1~128
 * @return u8 0:成功 1:失败
 */
u8 stm32_clock_init(u32 plln, u32 pllm, u32 pllp, u32 pllq)
{
    RCC_OscInitTypeDef ymx_osc_init;
    RCC_ClkInitTypeDef ymx_clk_init;

    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while((PWR->D3CR&(PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY);

    ymx_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;  //HSE外部晶振输入
    ymx_osc_init.HSEState       = RCC_HSE_ON;              //HSE时钟开启
    ymx_osc_init.HSIState       = RCC_HSI_OFF;             //SHI时钟关闭
    ymx_osc_init.CSIState       = RCC_CSI_OFF;             //CSI时钟关闭
    ymx_osc_init.PLL.PLLState   = RCC_PLL_ON;              //锁相环开启
    ymx_osc_init.PLL.PLLSource  = RCC_PLLSOURCE_HSE;       //锁相环时钟来源HSE

    ymx_osc_init.PLL.PLLN = plln;
    ymx_osc_init.PLL.PLLM = pllm;
    ymx_osc_init.PLL.PLLP = pllp;
    ymx_osc_init.PLL.PLLQ = pllq;

    ymx_osc_init.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;     //PLL1输出范围
    ymx_osc_init.PLL.PLLRGE    = RCC_PLL1VCIRANGE_2;  //PLL1输入范围
    if(HAL_RCC_OscConfig(&ymx_osc_init) != HAL_OK)    //配置时钟源
        return 1;

    qspi_memmap_mode(); //QSPI内存映射模式,需要在时钟初始化之前开启,否则会有各种问题

    ymx_clk_init.ClockType=(RCC_CLOCKTYPE_SYSCLK  |\
                            RCC_CLOCKTYPE_HCLK    |\
                            RCC_CLOCKTYPE_D1PCLK1 |\
                            RCC_CLOCKTYPE_PCLK1   |\
                            RCC_CLOCKTYPE_PCLK2   |\
                            RCC_CLOCKTYPE_D3PCLK1);

    ymx_clk_init.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;  //系统时钟来源:锁相环输出
    ymx_clk_init.SYSCLKDivider  = RCC_SYSCLK_DIV1;          //系统时钟分频系数:1
    ymx_clk_init.AHBCLKDivider  = RCC_HCLK_DIV2;            //HCLK时钟分频系数:2
    ymx_clk_init.APB1CLKDivider = RCC_APB1_DIV2;            //APB1时钟分频系数:2
    ymx_clk_init.APB2CLKDivider = RCC_APB2_DIV2;            //APB2时钟分频系数:2
    ymx_clk_init.APB3CLKDivider = RCC_APB3_DIV2;            //APB3时钟分频系数:2
    ymx_clk_init.APB4CLKDivider = RCC_APB4_DIV4;            //APB4时钟分频系数:4
    if(HAL_RCC_ClockConfig(&ymx_clk_init, FLASH_LATENCY_2) != HAL_OK)  //配置系统时钟
        return 1;

    __HAL_RCC_CSI_ENABLE();         //使能CSI时钟
    __HAL_RCC_SYSCFG_CLK_ENABLE();  //使能APB4_SYSCFG时钟
    HAL_EnableCompensationCell();   //使能IO补偿单元

    return 0;  //配置成功
}

/**
 * @brief 使能STM32H7的L1Cache,默认开启DCache透写
 * 
 */
void stm32_cache_enable(void)
{
    SCB_EnableICache();  //使能ICache
    SCB_EnableDCache();  //使能DCache   
    SCB->CACR|=1<<2;     //强制DCache透写,如不开启,实际使用中可能遇到各种问题
}

/**
 * @brief 判断ICache是否打开
 * 
 * @return u8 0:关闭 1:开启
 */
u8 stm32_icache_stastu(void)
{
    u8 sta;
    sta=((SCB->CCR)>>17)&0X01;
    return sta;
}

/**
 * @brief 判断DCache是否打开
 * 
 * @return u8 0:关闭 1:开启
 */
u8 stm32_dcache_status(void)
{
    u8 sta;
    sta=((SCB->CCR)>>16)&0X01;
    return sta;
}

/**
 * @brief QSPI进入内存映射模式,为减少文件引入,除GPIO驱动,其他外设均使用寄存器配置
 * 
 */
void qspi_memmap_mode(void)
{
    u32  tempreg   = 0;
    vu32 *data_reg = &QUADSPI->DR;
    GPIO_InitTypeDef ymx_gpio_init;

    RCC->AHB4ENR|=1<<1;   //使能GPIOB时钟
    RCC->AHB4ENR|=1<<5;   //使能GPIOF时钟
    RCC->AHB3ENR|=1<<14;  //使能QSPI时钟

    ymx_gpio_init.Pin       = GPIO_PIN_6;                 //PB6 AF10
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;            //复用推挽输出
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    ymx_gpio_init.Pull      = GPIO_NOPULL;                //无上拉下拉
    ymx_gpio_init.Alternate = GPIO_AF10_QUADSPI;          //复用为QSPI
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

    //QSPI设置,参考QSPI实验的QSPI_Init函数
    RCC->AHB3RSTR |= 1<<14;       //复位QSPI
    RCC->AHB3RSTR &= ~(1<<14);    //停止复位QSPI
    while(QUADSPI->SR&(1<<5));    //等待BUSY位清零
    QUADSPI->CR    = 0X01000310;  //设置CR寄存器
    QUADSPI->DCR   = 0X00160401;  //设置DCR寄存器
    QUADSPI->CR   |= 1<<0;        //使能QSPI

    /*
    注意:QSPI QE位的使能,在QSPI烧写算法里面,就已经设置了
    所以,这里可以不用设置QE位,否则需要加入对QE位置1的代码
    不过,代码必须通过仿真器下载,直接烧录到外部QSPI FLASH,是不可用的
    如果想直接烧录到外部QSPI FLASH也可以用,则需要在这里添加QE位置1的代码
    */

    //W25QXX进入QPI模式（0X38指令）
    while(QUADSPI->SR&(1<<5));         //等待BUSY位清零
    QUADSPI->CCR  = 0X00000138;        //发送0X38指令,W25QXX进入QPI模式
    while((QUADSPI->SR&(1<<1)) == 0);  //等待指令发送完成
    QUADSPI->FCR |= 1<<1;              //清除发送完成标志位

    //W25QXX写使能（0X06指令）
    while(QUADSPI->SR & (1<<5));       //等待BUSY位清零
    QUADSPI->CCR  = 0X00000106;        //发送0X06指令,W25QXX写使能
    while((QUADSPI->SR&(1<<1)) == 0);  //等待指令发送完成
    QUADSPI->FCR |= 1<<1;              //清除发送完成标志位

    //W25QXX设置QPI相关读参数（0XC0）
    while(QUADSPI->SR & (1<<5));       //等待BUSY位清零 
    QUADSPI->CCR  = 0X030003C0;        //发送0XC0指令,W25QXX读参数设置
    QUADSPI->DLR  = 0;
    while((QUADSPI->SR&(1<<2)) == 0);  //等待FTF
    *(vu8 *)data_reg = 3<<4;           //设置P4&P5=11,8个dummy clocks,104M
    QUADSPI->CR  |= 1<<2;              //终止传输
    while((QUADSPI->SR&(1<<1)) == 0);  //等待数据发送完成
    QUADSPI->FCR |= 1<<1;              //清除发送完成标志位
    while(QUADSPI->SR & (1<<5));       //等待BUSY位清零

    //MemroyMap模式设置
    while(QUADSPI->SR & (1<<5));  //等待BUSY位清零
    QUADSPI->ABR = 0;             //交替字节设置为0，实际上就是W25Q 0XEB指令的,M0~M7=0
    tempreg  = 0XEB;              //INSTRUCTION[7:0]=0XEB,发送0XEB指令(Fast Read QUAD I/O)
    tempreg |= 3<<8;              //IMODE  [1:0]=3, 四线传输指令
    tempreg |= 3<<10;             //ADDRESS[1:0]=3, 四线传输地址
    tempreg |= 2<<12;             //ADSIZE [1:0]=2, 24位地址长度
    tempreg |= 3<<14;             //ABMODE [1:0]=3, 四线传输交替字节
    tempreg |= 0<<16;             //ABSIZE [1:0]=0, 8位交替字节(M0~M7)
    tempreg |= 6<<18;             //DCYC   [4:0]=6, 6个dummy周期
    tempreg |= 3<<24;             //DMODE  [1:0]=3, 四线传输数据
    tempreg |= 3<<26;             //FMODE  [1:0]=3, 内存映射模式
    QUADSPI->CCR = tempreg;       //设置CCR寄存器

    //设置QSPI FLASH空间的MPU保护
    SCB->SHCSR &= ~(1<<16);       //禁止MemManage
    MPU->CTRL  &= ~(1<<0);        //禁止MPU
    MPU->RNR    = 0;              //设置保护区域编号为0(1~7可以给其他内存用)
    MPU->RBAR   = 0X90000000;     //基地址为0X9000 000,即QSPI的起始地址
    MPU->RASR   = 0X0303002D;     //设置相关保护参数(禁止共用,允许cache,允许缓冲),详见MPU实验的解析
    MPU->CTRL   = (1<<2)|(1<<0);  //使能PRIVDEFENA,使能MPU
    SCB->SHCSR |= 1<<16;          //使能MemManage
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief 当编译提示出错的时候此函数用来报告错误的文件和所在行
 * 
 * @param file 指向源文件
 * @param line 指向在文件中的行数
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif

#if defined(__clang__) //使用V6编译器(clang)

/**
 * @brief 执行WFI指令
 * 
 */
void __attribute__((noinline)) WFI_SET(void)
{
    __asm__("wfi");
}

/**
 * @brief 关闭所有中断
 * 
 */
void __attribute__((noinline)) INTX_DISABLE(void)
{
    __asm__("cpsid i \t\n"
            "bx lr");
}

/**
 * @brief 开启所有中断
 * 
 */
void __attribute__((noinline)) INTX_ENABLE(void)
{
    __asm__("cpsie i \t\n"
            "bx lr");
}

/**
 * @brief 设置堆栈地址
 * 
 * @param addr 栈顶地址
 */
void __attribute__((noinline)) MSR_MSP(u32 addr) 
{
    __asm__("msr msp, r0 \t\n"
            "bx r14");
}

#elif defined (__CC_ARM)    //使用V5编译器(ARMCC)

/**
 * @brief 执行WFI指令
 * 
 */
__asm void WFI_SET(void)
{
    WFI;		  
}

/**
 * @brief 关闭所有中断
 * 
 */
__asm void INTX_DISABLE(void)
{
    CPSID   I
    BX      LR	  
}

/**
 * @brief 开启所有中断
 * 
 */
__asm void INTX_ENABLE(void)
{
    CPSIE   I
    BX      LR  
}

/**
 * @brief 设置堆栈地址
 * 
 * @param addr 栈顶地址
 */
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0  //set Main Stack value
    BX r14
}

#endif
