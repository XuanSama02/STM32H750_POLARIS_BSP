#include "w25qxx.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"

u16 W25QXX_TYPE=W25Q256;  //默认是W25Q256
u8 W25QXX_BUFFER[4096];   //W25Qxx缓存区

//4K Bytes  = 1 Sector
//16 Sector = 1 Block

//W25Q256
//容量为32M,512个Block,8192个Sector

/**
 * @brief 初始化W25Qxx
 * 
 */
void w25qxx_init(void)
{
    u8 temp;
    GPIO_InitTypeDef ymx_gpio_init;
    //使能时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();  //使能GPIOF时钟
    //配置GPIO
    ymx_gpio_init.Pin   = GPIO_PIN_10;                //PF10
    ymx_gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;        //推挽输出
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //上拉
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //高速
    HAL_GPIO_Init(GPIOF, &ymx_gpio_init);
    //初始化SPI2
    W25QXX_CS(1);  //SPI FLASH不选中
    spi2_init();   //初始化SPI2
    spi2_set_speed(SPI_BAUDRATEPRESCALER_8);  //设置为50M时钟,高速模式
    //初始化W25Qxx
    W25QXX_TYPE = w25qxx_read_id();  //读取FLASH ID
    //仅适配W25Q256
    if(W25QXX_TYPE == W25Q256)       //SPI FLASH为W25Q256
    {
        temp = w25qxx_read_sr(3);    //读取状态寄存器3，判断地址模式
        if((temp&0X01) == 0)         //如果不是4字节地址模式,则进入4字节地址模式
        {
            W25QXX_CS(0);  //片选选中
            spi2_read_write_byte(W25X_Enable4ByteAddr);  //发送进入4字节地址模式指令
            W25QXX_CS(1);  //片选取消   
        }
    }
}  

/**
 * @brief 读取W25Qxx的型号
 * 
 * @return u16 W25Qxx型号值
 */
u16 w25qxx_read_id(void)
{
    u16 temp = 0;
    W25QXX_CS(0);
    spi2_read_write_byte(0x90);  //发送读取ID命令
    spi2_read_write_byte(0x00);
    spi2_read_write_byte(0x00);
    spi2_read_write_byte(0x00);
    temp |= spi2_read_write_byte(0xFF)<<8;
    temp |= spi2_read_write_byte(0xFF);
    W25QXX_CS(1);
    return temp;
}

/**
 * @brief 读取W25Qxx的状态寄存器,W25Qxx一共有三个状态寄存器
 * 
 *        状态寄存器1:
 *        7   6  5  4   3   2   1   0
 *        SPR RV TB BP2 BP1 BP0 WEL BUSY
 * 
 *        状态寄存器2:
 *        7   6   5   4   3   2   1  0
 *        SUS CMP LB3 LB2 LB1 (R) QE SRP1
 * 
 *        状态寄存器3:
 *        7        6    5    4   3   2   1   0
 *        HOLD/RST DRV1 DRV0 (R) (R) WPS ADP ADS
 * 
 *        SPR:状态寄存器保护位,默认0,配合WP使用
 *        TB,BP2,BP1,BP0:FLASH区域写保护设置
 *        WEL:写使能锁定
 *        BUSY:忙标记位(1:忙 0:闲)
 * 
 * @param regno 状态寄存器编号: 1~3
 * @return u8 状态寄存器的值
 */
u8 w25qxx_read_sr(u8 regno)
{  
    u8 byte    = 0;
    u8 command = 0; 
    switch(regno)  //根据传入状态寄存器编号选择对应指令
    {
        case 1:command  = W25X_ReadStatusReg1; break;
        case 2:command  = W25X_ReadStatusReg2; break;
        case 3:command  = W25X_ReadStatusReg3; break;
        default:command = W25X_ReadStatusReg1; break;
    }
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(command);      //发送读取状态寄存器命令
    byte = spi2_read_write_byte(0Xff);  //读取一个字节
    W25QXX_CS(1);  //片选取消
    return byte;
}

/**
 * @brief 写W25Qxx状态寄存器
 * 
 * @param regno 状态寄存器编号: 1~3
 * @param sr 状态寄存器的值
 */
void w25qxx_write_sr(u8 regno, u8 sr)   
{
    u8 command=0;
    switch(regno)
    {
        case 1:command  = W25X_WriteStatusReg1; break;
        case 2:command  = W25X_WriteStatusReg2; break;
        case 3:command  = W25X_WriteStatusReg3; break;
        default:command = W25X_WriteStatusReg1; break;
    }
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(command);  //发送写取状态寄存器命令
    spi2_read_write_byte(sr);       //写入一个字节
    W25QXX_CS(1);  //片选取消
}

/**
 * @brief W25Qxx写使能
 * 
 */
void w25qxx_write_enable(void)
{
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(W25X_WriteEnable);  //发送写使能指令
    W25QXX_CS(1);  //片选取消
}

/**
 * @brief W25Qxx写禁止
 * 
 */
void w25qxx_write_disable(void)
{
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(W25X_WriteDisable);  //发送写禁止指令
    W25QXX_CS(1);  //片选取消
}

/**
 * @brief 在指定地址开始读取指定长度的数据
 * 
 * @param pbuffer 数据存储区
 * @param addr    开始读取的地址(24bit)
 * @param num     要读取的字节数(最大65535)
 */
void w25qxx_read(u8* pbuffer, u32 addr, u16 num)
{
    u16 i;
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(W25X_ReadData);     //发送读取命令
    if(W25QXX_TYPE == W25Q256)               //如果是W25Q256的话地址为4字节的，要发送最高8位
        spi2_read_write_byte((u8)((addr)>>24));
    spi2_read_write_byte((u8)((addr)>>16));  //发送24bit地址
    spi2_read_write_byte((u8)((addr)>>8));
    spi2_read_write_byte((u8)addr);
    for(i=0; i<num; i++)  //循环读数
        pbuffer[i] = spi2_read_write_byte(0XFF);
    W25QXX_CS(1);  //片选取消
}

/**
 * @brief 在指定地址开始写入最大256字节的数据
 * 
 * @param pbuffer 数据存储区
 * @param addr    开始写入的地址(24bit)
 * @param num     要写入的字节数(最大256),该数不应该超过该页的剩余字节数
 */
void W25QXX_Write_Page(u8* pbuffer, u32 addr, u16 num)
{
    u16 i;
    w25qxx_write_enable();  //写入使能
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(W25X_PageProgram);  //发送写页命令
    if(W25QXX_TYPE == W25Q256)               //如果是W25Q256的话地址为4字节的，要发送最高8位
        spi2_read_write_byte((u8)((addr)>>24));
    spi2_read_write_byte((u8)((addr)>>16));  //发送24bit地址
    spi2_read_write_byte((u8)((addr)>>8));
    spi2_read_write_byte((u8)addr);
    for(i=0; i<num; i++)  //循环写数
        spi2_read_write_byte(pbuffer[i]);
    W25QXX_CS(1);  //片选取消
    w25qxx_wait_busy();   //等待写入结束
}

/**
 * @brief 无校验写入,所写地址范围内所有数据为0xFF,否则将在非0xFF处失败,具有自动换页功能,需要确保地址不越界
 * 
 * @param pbuffer 数据存储区
 * @param addr    开始写入的地址(24bit)
 * @param num     要写入的字节数(最大65535)
 */
void w25qxx_write_nocheck(u8* pbuffer, u32 addr, u16 num)
{
    u16 pageremain;
    pageremain = 256-addr%256;  //单页剩余的字节数
    if(num <= pageremain)       //不大于256个字节
        pageremain = num;
    while(1)
    {
        W25QXX_Write_Page(pbuffer, addr, pageremain);
        if(num == pageremain)   //写入结束了
            break;
        else  //num > pageremain
        {
            pbuffer += pageremain;
            addr    += pageremain;
            num     -= pageremain;  //减去已经写入了的字节数
            if(num>256)             //一次可以写入256个字节
                pageremain = 256;
            else                    //不够256个字节了
                pageremain = num;
        }
    }
} 

/**
 * @brief 在指定地址开始写入指定长度的数据,该函数带擦除操作
 * 
 * @param pbuffer 数据存储区
 * @param addr    开始写入的地址(24bit)
 * @param num     要写入的字节数(最大65535)
 */
void w25qxx_write(u8* pbuffer, u32 addr, u16 num)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 * W25QXX_BUF;
    secpos     = addr/4096;    //扇区地址
    secoff     = addr%4096;    //在扇区内的偏移
    secremain  = 4096-secoff;  //扇区剩余空间大小
    W25QXX_BUF = W25QXX_BUFFER;
    //printf("ad:%X,nb:%X\r\n",addr,num);  //测试用
    if(num <= secremain)       //不大于4096个字节
        secremain = num;
    while(1)
    {
        w25qxx_read(W25QXX_BUF, secpos*4096, 4096);  //读出整个扇区的内容
        for(i=0; i<secremain; i++)                   //校验数据
            if(W25QXX_BUF[secoff+i] != 0XFF)         //读取到非0xFF,需要擦除
                break;
        if(i < secremain)                 //需要擦除
        {
            w25qxx_erase_sector(secpos);  //擦除这个扇区
            for(i=0; i<secremain; i++)    //复制
                W25QXX_BUF[i+secoff] = pbuffer[i];
            w25qxx_write_nocheck(W25QXX_BUF, secpos*4096, 4096);  //写入整个扇区
        }
        else                              //写已经擦除了的,直接写入扇区剩余区间
            w25qxx_write_nocheck(pbuffer, addr, secremain);
        if(num == secremain)  //写入结束了
            break;
        else                  //写入未结束
        {
            secpos++;              //扇区地址增1
            secoff   = 0;          //偏移位置为0
            pbuffer += secremain;  //指针偏移
            addr    += secremain;  //写地址偏移
            num     -= secremain;  //字节数递减
            if(num > 4096)  //下一个扇区还是写不完
                secremain = 4096;
            else            //下一个扇区可以写完了
                secremain = num;
        }
    }
}

/**
 * @brief 擦除整个芯片,需要超长等待时间
 * 
 */
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable();  //写入使能
    w25qxx_wait_busy();
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(W25X_ChipErase);  //发送片擦除命令
    W25QXX_CS(1);  //片选取消
    w25qxx_wait_busy();  //等待芯片擦除结束
}

/**
 * @brief 擦除一个扇区,需要时间150ms/扇区
 * 
 * @param Dst_Addr 扇区地址
 */
void w25qxx_erase_sector(u32 Dst_Addr)   
{  
    //监视falsh擦除情况,测试用
    //printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr *= 4096;
    w25qxx_write_enable();  //写入使能
    w25qxx_wait_busy();
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(W25X_SectorErase);      //发送扇区擦除指令
    if(W25QXX_TYPE == W25Q256)                   //如果是W25Q256的话地址为4字节的，要发送最高8位
        spi2_read_write_byte((u8)((Dst_Addr)>>24));
    spi2_read_write_byte((u8)((Dst_Addr)>>16));  //发送24bit地址
    spi2_read_write_byte((u8)((Dst_Addr)>>8));
    spi2_read_write_byte((u8)Dst_Addr);
    W25QXX_CS(1);  //片选取消
    w25qxx_wait_busy();  //等待擦除完成
}

/**
 * @brief 等待空闲
 * 
 */
void w25qxx_wait_busy(void)
{
    while((w25qxx_read_sr(1)&0x01) == 0x01);  //等待BUSY位清空
}

/**
 * @brief 进入掉电模式
 * 
 */
void w25qxx_powerdown(void)
{
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(W25X_PowerDown);  //发送掉电命令
    W25QXX_CS(1);  //片选取消
    delay_us(3);  //等待TPD
}

/**
 * @brief 唤醒
 * 
 */
void w25qxx_wakeup(void)
{
    W25QXX_CS(0);  //片选选中
    spi2_read_write_byte(W25X_ReleasePowerDown);  //发送唤醒指令
    W25QXX_CS(1);  //片选取消
    delay_us(3);  //等待TRES1
}
