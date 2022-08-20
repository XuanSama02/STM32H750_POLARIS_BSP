#include "w25qxx.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"

u16 W25QXX_TYPE=W25Q256;  //Ĭ����W25Q256
u8 W25QXX_BUFFER[4096];   //W25Qxx������

//4K Bytes  = 1 Sector
//16 Sector = 1 Block

//W25Q256
//����Ϊ32M,512��Block,8192��Sector

/**
 * @brief ��ʼ��W25Qxx
 * 
 */
void w25qxx_init(void)
{
    u8 temp;
    GPIO_InitTypeDef ymx_gpio_init;
    //ʹ��ʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();  //ʹ��GPIOFʱ��
    //����GPIO
    ymx_gpio_init.Pin   = GPIO_PIN_10;                //PF10
    ymx_gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;        //�������
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //����
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(GPIOF, &ymx_gpio_init);
    //��ʼ��SPI2
    W25QXX_CS(1);  //SPI FLASH��ѡ��
    spi2_init();   //��ʼ��SPI2
    spi2_set_speed(SPI_BAUDRATEPRESCALER_8);  //����Ϊ50Mʱ��,����ģʽ
    //��ʼ��W25Qxx
    W25QXX_TYPE = w25qxx_read_id();  //��ȡFLASH ID
    //������W25Q256
    if(W25QXX_TYPE == W25Q256)       //SPI FLASHΪW25Q256
    {
        temp = w25qxx_read_sr(3);    //��ȡ״̬�Ĵ���3���жϵ�ַģʽ
        if((temp&0X01) == 0)         //�������4�ֽڵ�ַģʽ,�����4�ֽڵ�ַģʽ
        {
            W25QXX_CS(0);  //Ƭѡѡ��
            spi2_read_write_byte(W25X_Enable4ByteAddr);  //���ͽ���4�ֽڵ�ַģʽָ��
            W25QXX_CS(1);  //Ƭѡȡ��   
        }
    }
}  

/**
 * @brief ��ȡW25Qxx���ͺ�
 * 
 * @return u16 W25Qxx�ͺ�ֵ
 */
u16 w25qxx_read_id(void)
{
    u16 temp = 0;
    W25QXX_CS(0);
    spi2_read_write_byte(0x90);  //���Ͷ�ȡID����
    spi2_read_write_byte(0x00);
    spi2_read_write_byte(0x00);
    spi2_read_write_byte(0x00);
    temp |= spi2_read_write_byte(0xFF)<<8;
    temp |= spi2_read_write_byte(0xFF);
    W25QXX_CS(1);
    return temp;
}

/**
 * @brief ��ȡW25Qxx��״̬�Ĵ���,W25Qxxһ��������״̬�Ĵ���
 * 
 *        ״̬�Ĵ���1:
 *        7   6  5  4   3   2   1   0
 *        SPR RV TB BP2 BP1 BP0 WEL BUSY
 * 
 *        ״̬�Ĵ���2:
 *        7   6   5   4   3   2   1  0
 *        SUS CMP LB3 LB2 LB1 (R) QE SRP1
 * 
 *        ״̬�Ĵ���3:
 *        7        6    5    4   3   2   1   0
 *        HOLD/RST DRV1 DRV0 (R) (R) WPS ADP ADS
 * 
 *        SPR:״̬�Ĵ�������λ,Ĭ��0,���WPʹ��
 *        TB,BP2,BP1,BP0:FLASH����д��������
 *        WEL:дʹ������
 *        BUSY:æ���λ(1:æ 0:��)
 * 
 * @param regno ״̬�Ĵ������: 1~3
 * @return u8 ״̬�Ĵ�����ֵ
 */
u8 w25qxx_read_sr(u8 regno)
{  
    u8 byte    = 0;
    u8 command = 0; 
    switch(regno)  //���ݴ���״̬�Ĵ������ѡ���Ӧָ��
    {
        case 1:command  = W25X_ReadStatusReg1; break;
        case 2:command  = W25X_ReadStatusReg2; break;
        case 3:command  = W25X_ReadStatusReg3; break;
        default:command = W25X_ReadStatusReg1; break;
    }
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(command);      //���Ͷ�ȡ״̬�Ĵ�������
    byte = spi2_read_write_byte(0Xff);  //��ȡһ���ֽ�
    W25QXX_CS(1);  //Ƭѡȡ��
    return byte;
}

/**
 * @brief дW25Qxx״̬�Ĵ���
 * 
 * @param regno ״̬�Ĵ������: 1~3
 * @param sr ״̬�Ĵ�����ֵ
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
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(command);  //����дȡ״̬�Ĵ�������
    spi2_read_write_byte(sr);       //д��һ���ֽ�
    W25QXX_CS(1);  //Ƭѡȡ��
}

/**
 * @brief W25Qxxдʹ��
 * 
 */
void w25qxx_write_enable(void)
{
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(W25X_WriteEnable);  //����дʹ��ָ��
    W25QXX_CS(1);  //Ƭѡȡ��
}

/**
 * @brief W25Qxxд��ֹ
 * 
 */
void w25qxx_write_disable(void)
{
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(W25X_WriteDisable);  //����д��ָֹ��
    W25QXX_CS(1);  //Ƭѡȡ��
}

/**
 * @brief ��ָ����ַ��ʼ��ȡָ�����ȵ�����
 * 
 * @param pbuffer ���ݴ洢��
 * @param addr    ��ʼ��ȡ�ĵ�ַ(24bit)
 * @param num     Ҫ��ȡ���ֽ���(���65535)
 */
void w25qxx_read(u8* pbuffer, u32 addr, u16 num)
{
    u16 i;
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(W25X_ReadData);     //���Ͷ�ȡ����
    if(W25QXX_TYPE == W25Q256)               //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
        spi2_read_write_byte((u8)((addr)>>24));
    spi2_read_write_byte((u8)((addr)>>16));  //����24bit��ַ
    spi2_read_write_byte((u8)((addr)>>8));
    spi2_read_write_byte((u8)addr);
    for(i=0; i<num; i++)  //ѭ������
        pbuffer[i] = spi2_read_write_byte(0XFF);
    W25QXX_CS(1);  //Ƭѡȡ��
}

/**
 * @brief ��ָ����ַ��ʼд�����256�ֽڵ�����
 * 
 * @param pbuffer ���ݴ洢��
 * @param addr    ��ʼд��ĵ�ַ(24bit)
 * @param num     Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���
 */
void W25QXX_Write_Page(u8* pbuffer, u32 addr, u16 num)
{
    u16 i;
    w25qxx_write_enable();  //д��ʹ��
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(W25X_PageProgram);  //����дҳ����
    if(W25QXX_TYPE == W25Q256)               //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
        spi2_read_write_byte((u8)((addr)>>24));
    spi2_read_write_byte((u8)((addr)>>16));  //����24bit��ַ
    spi2_read_write_byte((u8)((addr)>>8));
    spi2_read_write_byte((u8)addr);
    for(i=0; i<num; i++)  //ѭ��д��
        spi2_read_write_byte(pbuffer[i]);
    W25QXX_CS(1);  //Ƭѡȡ��
    w25qxx_wait_busy();   //�ȴ�д�����
}

/**
 * @brief ��У��д��,��д��ַ��Χ����������Ϊ0xFF,�����ڷ�0xFF��ʧ��,�����Զ���ҳ����,��Ҫȷ����ַ��Խ��
 * 
 * @param pbuffer ���ݴ洢��
 * @param addr    ��ʼд��ĵ�ַ(24bit)
 * @param num     Ҫд����ֽ���(���65535)
 */
void w25qxx_write_nocheck(u8* pbuffer, u32 addr, u16 num)
{
    u16 pageremain;
    pageremain = 256-addr%256;  //��ҳʣ����ֽ���
    if(num <= pageremain)       //������256���ֽ�
        pageremain = num;
    while(1)
    {
        W25QXX_Write_Page(pbuffer, addr, pageremain);
        if(num == pageremain)   //д�������
            break;
        else  //num > pageremain
        {
            pbuffer += pageremain;
            addr    += pageremain;
            num     -= pageremain;  //��ȥ�Ѿ�д���˵��ֽ���
            if(num>256)             //һ�ο���д��256���ֽ�
                pageremain = 256;
            else                    //����256���ֽ���
                pageremain = num;
        }
    }
} 

/**
 * @brief ��ָ����ַ��ʼд��ָ�����ȵ�����,�ú�������������
 * 
 * @param pbuffer ���ݴ洢��
 * @param addr    ��ʼд��ĵ�ַ(24bit)
 * @param num     Ҫд����ֽ���(���65535)
 */
void w25qxx_write(u8* pbuffer, u32 addr, u16 num)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 * W25QXX_BUF;
    secpos     = addr/4096;    //������ַ
    secoff     = addr%4096;    //�������ڵ�ƫ��
    secremain  = 4096-secoff;  //����ʣ��ռ��С
    W25QXX_BUF = W25QXX_BUFFER;
    //printf("ad:%X,nb:%X\r\n",addr,num);  //������
    if(num <= secremain)       //������4096���ֽ�
        secremain = num;
    while(1)
    {
        w25qxx_read(W25QXX_BUF, secpos*4096, 4096);  //������������������
        for(i=0; i<secremain; i++)                   //У������
            if(W25QXX_BUF[secoff+i] != 0XFF)         //��ȡ����0xFF,��Ҫ����
                break;
        if(i < secremain)                 //��Ҫ����
        {
            w25qxx_erase_sector(secpos);  //�����������
            for(i=0; i<secremain; i++)    //����
                W25QXX_BUF[i+secoff] = pbuffer[i];
            w25qxx_write_nocheck(W25QXX_BUF, secpos*4096, 4096);  //д����������
        }
        else                              //д�Ѿ������˵�,ֱ��д������ʣ������
            w25qxx_write_nocheck(pbuffer, addr, secremain);
        if(num == secremain)  //д�������
            break;
        else                  //д��δ����
        {
            secpos++;              //������ַ��1
            secoff   = 0;          //ƫ��λ��Ϊ0
            pbuffer += secremain;  //ָ��ƫ��
            addr    += secremain;  //д��ַƫ��
            num     -= secremain;  //�ֽ����ݼ�
            if(num > 4096)  //��һ����������д����
                secremain = 4096;
            else            //��һ����������д����
                secremain = num;
        }
    }
}

/**
 * @brief ��������оƬ,��Ҫ�����ȴ�ʱ��
 * 
 */
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable();  //д��ʹ��
    w25qxx_wait_busy();
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(W25X_ChipErase);  //����Ƭ��������
    W25QXX_CS(1);  //Ƭѡȡ��
    w25qxx_wait_busy();  //�ȴ�оƬ��������
}

/**
 * @brief ����һ������,��Ҫʱ��150ms/����
 * 
 * @param Dst_Addr ������ַ
 */
void w25qxx_erase_sector(u32 Dst_Addr)   
{  
    //����falsh�������,������
    //printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr *= 4096;
    w25qxx_write_enable();  //д��ʹ��
    w25qxx_wait_busy();
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(W25X_SectorErase);      //������������ָ��
    if(W25QXX_TYPE == W25Q256)                   //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
        spi2_read_write_byte((u8)((Dst_Addr)>>24));
    spi2_read_write_byte((u8)((Dst_Addr)>>16));  //����24bit��ַ
    spi2_read_write_byte((u8)((Dst_Addr)>>8));
    spi2_read_write_byte((u8)Dst_Addr);
    W25QXX_CS(1);  //Ƭѡȡ��
    w25qxx_wait_busy();  //�ȴ��������
}

/**
 * @brief �ȴ�����
 * 
 */
void w25qxx_wait_busy(void)
{
    while((w25qxx_read_sr(1)&0x01) == 0x01);  //�ȴ�BUSYλ���
}

/**
 * @brief �������ģʽ
 * 
 */
void w25qxx_powerdown(void)
{
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(W25X_PowerDown);  //���͵�������
    W25QXX_CS(1);  //Ƭѡȡ��
    delay_us(3);  //�ȴ�TPD
}

/**
 * @brief ����
 * 
 */
void w25qxx_wakeup(void)
{
    W25QXX_CS(0);  //Ƭѡѡ��
    spi2_read_write_byte(W25X_ReleasePowerDown);  //���ͻ���ָ��
    W25QXX_CS(1);  //Ƭѡȡ��
    delay_us(3);  //�ȴ�TRES1
}
