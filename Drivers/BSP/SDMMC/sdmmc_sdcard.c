#include "sdmmc_sdcard.h"
#include "string.h"  
#include "usart.h"	 

static u8 CardType=STD_CAPACITY_SD_CARD_V1_1;  //SD�����ͣ�Ĭ��Ϊ1.x����
static u32 CSD_Tab[4],CID_Tab[4],RCA=0;        //SD��CSD,CID�Լ���Ե�ַ(RCA)����
SD_CardInfo SDCardInfo;                        //SD����Ϣ

//SD_ReadDisk/SD_WriteDisk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
//��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
__align(4) u8 SDMMC_DATA_BUFFER[512];
 
/**
 * @brief ��ʼ��SD��
 * 
 * @return SD_Error SD������״̬
 */
SD_Error sd_init(void)
{
    SD_Error errorstatus = SD_OK;
    u8 clkdiv = 0;
    //SDMMC1 IO�ڳ�ʼ��
    GPIO_InitTypeDef ymx_gpio_init;

    __HAL_RCC_SDMMC1_CLK_ENABLE();  //ʹ��SDMMC1ʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��

    //PC8,9,10,11,12
    ymx_gpio_init.Pin       = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;            //���츴��
    ymx_gpio_init.Pull      = GPIO_NOPULL;                //��������
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    ymx_gpio_init.Alternate = GPIO_AF12_SDIO1;            //����ΪSDIO
    HAL_GPIO_Init(GPIOC, &ymx_gpio_init);                 //��ʼ��

    //PD2
    ymx_gpio_init.Pin = GPIO_PIN_2;            
    HAL_GPIO_Init(GPIOD, &ymx_gpio_init);  //��ʼ��

    //SDMMC����Ĵ�������ΪĬ��ֵ
    SDMMC1->POWER  = 0x00000000;
    SDMMC1->CLKCR  = 0x00000000;
    SDMMC1->ARG    = 0x00000000;
    SDMMC1->CMD    = 0x00000000;
    SDMMC1->DTIMER = 0x00000000;
    SDMMC1->DLEN   = 0x00000000;
    SDMMC1->DCTRL  = 0x00000000;
    SDMMC1->ICR    = 0X1FE00FFF;
    SDMMC1->MASK   = 0x00000000;

    HAL_NVIC_SetPriority(SDMMC1_IRQn, 2, 0);  //����SDMMC1�жϣ���ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);          //ʹ��SDMMC1�ж�

    errorstatus = sd_power_on();       //SD���ϵ�
    if(errorstatus == SD_OK)
        errorstatus = sd_card_init();  //��ʼ��SD��
    if(errorstatus == SD_OK)
        errorstatus = sd_get_card_info(&SDCardInfo);                  //��ȡ����Ϣ
    if(errorstatus == SD_OK)
        errorstatus = sd_select_deselect((u32)(SDCardInfo.RCA<<16));  //ѡ��SD��
    if(errorstatus == SD_OK)
        errorstatus = sd_widebus_config(1);  //4λ���,�����MMC��,������4λģʽ
    if((errorstatus==SD_OK) || (MULTIMEDIA_CARD==CardType))
    {
        if(SDCardInfo.CardType==STD_CAPACITY_SD_CARD_V1_1 || SDCardInfo.CardType==STD_CAPACITY_SD_CARD_V2_0)
        {
            clkdiv = SDMMC_TRANSFER_CLK_DIV+2;  //V1.1/V2.0�����������48/4=12Mhz
        }
        else
            clkdiv = SDMMC_TRANSFER_CLK_DIV;    //SDHC�����������������48/2=24Mhz
        sdmmc_clk_config(clkdiv);  //����ʱ��Ƶ��,SDMMCʱ�Ӽ��㹫ʽ:SDMMC_CKʱ��=SDMMCCLK/[clkdiv+2];����,SDMMCCLK�̶�Ϊ48Mhz 
    }
    return errorstatus;		 
}

/**
 * @brief SDMMCʱ�ӳ�ʼ������,ʱ��=sdmmc_ker_ck/[2*clkdiv];(sdmmc_ker_ck�ӹ̶�Ϊ400Mhz)
 * 
 * @param clkdiv ʱ�ӷ�Ƶϵ��
 */
void sdmmc_clk_config(u16 clkdiv)
{
    u32 tmpreg = SDMMC1->CLKCR;
    tmpreg &= 0XFFFFFC00;
    tmpreg |= clkdiv;
    SDMMC1->CLKCR = tmpreg;
}

/**
 * @brief SDMMC���������
 * 
 * @param cmdindex ��������,����λ��Ч
 * @param waitrsp  �ڴ�����Ӧ 00/10:����Ӧ 01:����Ӧ 11:����Ӧ
 * @param arg      ����
 */
void sdmmc_send_cmd(u8 cmdindex, u8 waitrsp, u32 arg)
{			
    u32 tmpreg  = 0;
    SDMMC1->ARG = arg;
    tmpreg |= cmdindex&0X3F;    //�����µ�index
    tmpreg |= (u32)waitrsp<<8;  //�����µ�wait rsp
    tmpreg |= 0<<10;            //�޵ȴ�
    tmpreg |= 1<<12;            //����ͨ��״̬��ʹ��
    SDMMC1->CMD=tmpreg;
}

/**
 * @brief SDMMC�����������ú���
 * 
 * @param datatimeout ��ʱʱ������
 * @param datalen     �������ݳ���,��25λ��Ч,����Ϊ���С��������
 * @param blksize     ���С.ʵ�ʴ�СΪ:2^blksize�ֽ�
 * @param dir         ���ݴ��䷽�� 0:���������� 1:����������
 */
void sdmmc_send_data_config(u32 datatimeout, u32 datalen, u8 blksize, u8 dir)
{
    u32 tmpreg;
    SDMMC1->DTIMER = datatimeout;
    SDMMC1->DLEN   = datalen&0X1FFFFFF;  //��25λ��Ч
    tmpreg  =SDMMC1->DCTRL; 
    tmpreg &= 0xFFFFFF00;                //���֮ǰ������
    tmpreg |= blksize<<4;                //���ÿ��С
    tmpreg |= 0<<2;                      //�����ݴ���
    tmpreg |= (dir&0X01)<<1;             //�������
    tmpreg |= 1<<0;                      //���ݴ���ʹ��,DPSM״̬��
    SDMMC1->DCTRL = tmpreg;		
}  

/**
 * @brief SD���ϵ�,��ѯ����SDMMC�ӿ��ϵĿ��豸,����ѯ���ѹ������ʱ��
 * 
 * @return SD_Error SD������״̬
 */
SD_Error sd_power_on(void)
{
    u8 i = 0;
    u32 tempreg = 0;
    SD_Error errorstatus = SD_OK;
    u32 response = 0;
    u32 count = 0;
    u32 validvoltage = 0;
    u32 SDType = SD_STD_CAPACITY;
    //����CLKCR�Ĵ���  
    tempreg |= 0<<12;  //PWRSAV=0,��ʡ��ģʽ 
    tempreg |= 0<<14;  //WIDBUS[1:0]=0,1λ���ݿ��
    tempreg |= 0<<16;  //NEGEDGE=0,SDMMCCK�½��ظ������������
    tempreg |= 0<<17;  //HWFC_EN=0,�ر�Ӳ��������
    SDMMC1->CLKCR = tempreg; 
    sdmmc_clk_config(SDMMC_INIT_CLK_DIV);  //����ʱ��Ƶ��(��ʼ����ʱ��,���ܳ���400Khz)
    SDMMC1->POWER = 0X03;  //�ϵ�״̬,������ʱ��
    for(i=0; i<74; i++)
    {
        sdmmc_send_cmd(SD_CMD_GO_IDLE_STATE, 0, 0);  //����CMD0����IDLE STAGEģʽ����
        errorstatus = cmd_error();
        if(errorstatus == SD_OK)
            break;
    }
    if(errorstatus)  //���ش���״̬
        return errorstatus;
    sdmmc_send_cmd(SD_SDMMC_SEND_IF_COND, 1, SD_CHECK_PATTERN);  //����CMD8,����Ӧ,���SD���ӿ�����
                                                                 //arg[11:8]:01,֧�ֵ�ѹ��Χ,2.7~3.6V
                                                                 //arg[7:0]:Ĭ��0XAA
                                                                 //������Ӧ7
    errorstatus = cmd_resp7_error();  //�ȴ�R7��Ӧ
    if(errorstatus == SD_OK)          //R7��Ӧ����
    {
        CardType = STD_CAPACITY_SD_CARD_V2_0;  //SD 2.0��
        SDType   = SD_HIGH_CAPACITY;           //��������
    }
    sdmmc_send_cmd(SD_CMD_APP_CMD, 1, 0);      //����CMD55,����Ӧ
    errorstatus = cmd_resp1_error(SD_CMD_APP_CMD);  //�ȴ�R1��Ӧ
    if(errorstatus == SD_OK)          //SD2.0/SD 1.1,����ΪMMC��
    {
        //SD��,����ACMD41 SD_APP_OP_COND,����Ϊ:0x80100000
        while((!validvoltage) && (count<SD_MAX_VOLT_TRIAL))
        {
            sdmmc_send_cmd(SD_CMD_APP_CMD, 1, 0);               //����CMD55,����Ӧ 
            errorstatus = cmd_resp1_error(SD_CMD_APP_CMD);      //�ȴ�R1��Ӧ
            if(errorstatus != SD_OK)                            //��Ӧ����
                return errorstatus;
            sdmmc_send_cmd(SD_CMD_SD_APP_OP_COND, 1, SD_VOLTAGE_WINDOW_SD|SDType);  //����ACMD41,����Ӧ
            errorstatus = cmd_resp3_error();                    //�ȴ�R3��Ӧ   
            if(errorstatus != SD_OK)                            //��Ӧ����
                return errorstatus;
            response = SDMMC1->RESP1;                           //�õ���Ӧ
            validvoltage = (((response>>31)==1)?1:0);           //�ж�SD���ϵ��Ƿ����
            count++;
        }
        if(count >= SD_MAX_VOLT_TRIAL)
        {
            errorstatus = SD_INVALID_VOLTRANGE;
            return errorstatus;
        }
        if(response &= SD_HIGH_CAPACITY)
            CardType = HIGH_CAPACITY_SD_CARD;
    }
    else  //MMC��
    {
        //MMC��,����CMD1 SDMMC_SEND_OP_COND,����Ϊ:0x80FF8000
        while((!validvoltage) && (count<SD_MAX_VOLT_TRIAL))
        {
            sdmmc_send_cmd(SD_CMD_SEND_OP_COND, 1, SD_VOLTAGE_WINDOW_MMC);  //����CMD1,����Ӧ
            errorstatus = cmd_resp3_error();  //�ȴ�R3��Ӧ
            if(errorstatus != SD_OK)  //��Ӧ����
                return errorstatus;
            response = SDMMC1->RESP1;  //�õ���Ӧ
            validvoltage = (((response>>31)==1)?1:0);
            count++;
        }
        if(count >= SD_MAX_VOLT_TRIAL)
        {
            errorstatus = SD_INVALID_VOLTRANGE;
            return errorstatus;
        }
        CardType = MULTIMEDIA_CARD;
    }  
    return(errorstatus);
}

/**
 * @brief SD���ϵ�
 * 
 * @return SD_Error SD������״̬
 */
SD_Error sd_power_off(void)
{
    SDMMC1->POWER &= ~(3<<0);  //SDMMC��Դ�ر�,ʱ��ֹͣ
    return SD_OK;
}

/**
 * @brief ��ʼ�����е�SD��,���������״̬
 * 
 * @return SD_Error SD������״̬
 */
SD_Error sd_card_init(void)
{
    SD_Error errorstatus = SD_OK;
    u16 rca = 0x01;
    if((SDMMC1->POWER&0X03) == 0)
        return SD_REQUEST_NOT_APPLICABLE;  //����Դ״̬,ȷ��Ϊ�ϵ�״̬
    if(SECURE_DIGITAL_IO_CARD != CardType)  //��SECURE_DIGITAL_IO_CARD
    {
        sdmmc_send_cmd(SD_CMD_ALL_SEND_CID, 3, 0);  //����CMD2,ȡ��CID,����Ӧ
        errorstatus = cmd_resp2_error();            //�ȴ�R2��Ӧ
        if(errorstatus != SD_OK)                    //��Ӧ����
            return errorstatus;
        CID_Tab[0] = SDMMC1->RESP1;
        CID_Tab[1] = SDMMC1->RESP2;
        CID_Tab[2] = SDMMC1->RESP3;
        CID_Tab[3] = SDMMC1->RESP4;
    }
    //�жϿ�����
    if((STD_CAPACITY_SD_CARD_V1_1==CardType) || (STD_CAPACITY_SD_CARD_V2_0==CardType) || (SECURE_DIGITAL_IO_COMBO_CARD==CardType) || (HIGH_CAPACITY_SD_CARD==CardType))
    {
        sdmmc_send_cmd(SD_CMD_SET_REL_ADDR, 1, 0);                 //����CMD3,����Ӧ
        errorstatus = cmd_resp6_error(SD_CMD_SET_REL_ADDR, &rca);  //�ȴ�R6��Ӧ
        if(errorstatus != SD_OK)                                   //��Ӧ����
            return errorstatus;
    }   
    if(MULTIMEDIA_CARD == CardType)
    {
        sdmmc_send_cmd(SD_CMD_SET_REL_ADDR, 1, (u32)(rca<<16));  //����CMD3,����Ӧ
        errorstatus = cmd_resp2_error();                         //�ȴ�R2��Ӧ
        if(errorstatus != SD_OK)                                 //��Ӧ����
            return errorstatus;
    }
    if(SECURE_DIGITAL_IO_CARD != CardType)  //��SECURE_DIGITAL_IO_CARD
    {
        RCA = rca;
        sdmmc_send_cmd(SD_CMD_SEND_CSD, 3, (u32)(rca<<16));  //����CMD9+��RCA,ȡ��CSD,����Ӧ
        errorstatus = cmd_resp2_error();                     //�ȴ�R2��Ӧ
        if(errorstatus != SD_OK)                             //��Ӧ����
            return errorstatus;
        CSD_Tab[0]=SDMMC1->RESP1;
        CSD_Tab[1]=SDMMC1->RESP2;
        CSD_Tab[2]=SDMMC1->RESP3;
        CSD_Tab[3]=SDMMC1->RESP4;
    }
    return SD_OK;//����ʼ���ɹ�
}

/**
 * @brief �õ�SD����Ϣ
 * 
 * @param cardinfo ����Ϣ�洢��
 * @return SD_Error SD������״̬
 */
SD_Error sd_get_card_info(SD_CardInfo *cardinfo)
{
    SD_Error errorstatus = SD_OK;
    u8 tmp = 0;
    cardinfo->CardType = (u8)CardType;                  //������
    cardinfo->RCA      = (u16)RCA;                      //��RCAֵ
    tmp = (u8)((CSD_Tab[0]&0xFF000000)>>24);
    cardinfo->SD_csd.CSDStruct        = (tmp&0xC0)>>6;  //CSD�ṹ
    cardinfo->SD_csd.SysSpecVersion   = (tmp&0x3C)>>2;  //2.0Э�黹û�����ⲿ��(Ϊ����),Ӧ���Ǻ���Э�鶨���
    cardinfo->SD_csd.Reserved1        = tmp&0x03;       //2������λ
    tmp = (u8)((CSD_Tab[0]&0x00FF0000)>>16);            //��1���ֽ�
    cardinfo->SD_csd.TAAC             = tmp;            //���ݶ�ʱ��1
    tmp = (u8)((CSD_Tab[0]&0x0000FF00)>>8);             //��2���ֽ�
    cardinfo->SD_csd.NSAC             = tmp;            //���ݶ�ʱ��2
    tmp = (u8)(CSD_Tab[0]&0x000000FF);                  //��3���ֽ�
    cardinfo->SD_csd.MaxBusClkFrec    = tmp;            //�����ٶ�
    tmp = (u8)((CSD_Tab[1]&0xFF000000)>>24);            //��4���ֽ�
    cardinfo->SD_csd.CardComdClasses  = tmp<<4;         //��ָ�������λ
    tmp = (u8)((CSD_Tab[1]&0x00FF0000)>>16);            //��5���ֽ�
    cardinfo->SD_csd.CardComdClasses |= (tmp&0xF0)>>4;  //��ָ�������λ
    cardinfo->SD_csd.RdBlockLen       = tmp&0x0F;       //����ȡ���ݳ���
    tmp = (u8)((CSD_Tab[1]&0x0000FF00)>>8);             //��6���ֽ�
    cardinfo->SD_csd.PartBlockRead    = (tmp&0x80)>>7;  //����ֿ��
    cardinfo->SD_csd.WrBlockMisalign  = (tmp&0x40)>>6;  //д���λ
    cardinfo->SD_csd.RdBlockMisalign  = (tmp&0x20)>>5;  //�����λ
    cardinfo->SD_csd.DSRImpl          = (tmp&0x10)>>4;
    cardinfo->SD_csd.Reserved2        = 0;              //����
    if((CardType==STD_CAPACITY_SD_CARD_V1_1) || (CardType==STD_CAPACITY_SD_CARD_V2_0) || (MULTIMEDIA_CARD==CardType))  //��׼1.1/2.0��/MMC��
    {
        cardinfo->SD_csd.DeviceSize          = (tmp&0x03)<<10;                   //C_SIZE(12λ)
        tmp = (u8)(CSD_Tab[1]&0x000000FF);                                       //��7���ֽ�
        cardinfo->SD_csd.DeviceSize         |= (tmp)<<2;
        tmp = (u8)((CSD_Tab[2]&0xFF000000)>>24);                                 //��8���ֽ�
        cardinfo->SD_csd.DeviceSize         |= (tmp&0xC0)>>6;
        cardinfo->SD_csd.MaxRdCurrentVDDMin  = (tmp&0x38)>>3;
        cardinfo->SD_csd.MaxRdCurrentVDDMax  = (tmp&0x07);
        tmp = (u8)((CSD_Tab[2]&0x00FF0000)>>16);                                 //��9���ֽ�
        cardinfo->SD_csd.MaxWrCurrentVDDMin  = (tmp&0xE0)>>5;
        cardinfo->SD_csd.MaxWrCurrentVDDMax  = (tmp&0x1C)>>2;
        cardinfo->SD_csd.DeviceSizeMul       = (tmp&0x03)<<1;                    //C_SIZE_MULT
        tmp = (u8)((CSD_Tab[2]&0x0000FF00)>>8);                                  //��10���ֽ�
        cardinfo->SD_csd.DeviceSizeMul      |= (tmp&0x80)>>7;
        cardinfo->CardCapacity               = (cardinfo->SD_csd.DeviceSize+1);  //���㿨����
        cardinfo->CardCapacity              *= (1<<(cardinfo->SD_csd.DeviceSizeMul+2));
        cardinfo->CardBlockSize              = 1<<(cardinfo->SD_csd.RdBlockLen);  //���С
        cardinfo->CardCapacity              *= cardinfo->CardBlockSize;
    }
    else if(CardType == HIGH_CAPACITY_SD_CARD)          //��������
    {
        tmp = (u8)(CSD_Tab[1]&0x000000FF);              //��7���ֽ�
        cardinfo->SD_csd.DeviceSize  = (tmp&0x3F)<<16;  //C_SIZE
        tmp = (u8)((CSD_Tab[2]&0xFF000000)>>24);        //��8���ֽ�
        cardinfo->SD_csd.DeviceSize |= (tmp<<8);
        tmp = (u8)((CSD_Tab[2]&0x00FF0000)>>16);        //��9���ֽ�
        cardinfo->SD_csd.DeviceSize |= (tmp);
        tmp = (u8)((CSD_Tab[2]&0x0000FF00)>>8);         //��10���ֽ�
        cardinfo->CardCapacity       = (long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;  //���㿨����
        cardinfo->CardBlockSize      = 512;             //���С�̶�Ϊ512�ֽ�
    }	  
    cardinfo->SD_csd.EraseGrSize = (tmp&0x40)>>6;
    cardinfo->SD_csd.EraseGrMul  = (tmp&0x3F)<<1;

    tmp = (u8)(CSD_Tab[2]&0x000000FF);  //��11���ֽ�
    cardinfo->SD_csd.EraseGrMul      |= (tmp&0x80)>>7;
    cardinfo->SD_csd.WrProtectGrSize  = (tmp&0x7F);

    tmp = (u8)((CSD_Tab[3]&0xFF000000)>>24);  //��12���ֽ�
    cardinfo->SD_csd.WrProtectGrEnable = (tmp&0x80)>>7;
    cardinfo->SD_csd.ManDeflECC        = (tmp&0x60)>>5;
    cardinfo->SD_csd.WrSpeedFact       = (tmp&0x1C)>>2;
    cardinfo->SD_csd.MaxWrBlockLen     = (tmp&0x03)<<2;

    tmp = (u8)((CSD_Tab[3]&0x00FF0000)>>16);  //��13���ֽ�
    cardinfo->SD_csd.MaxWrBlockLen       |= (tmp&0xC0)>>6;
    cardinfo->SD_csd.WriteBlockPaPartial  = (tmp&0x20)>>5;
    cardinfo->SD_csd.Reserved3            = 0;
    cardinfo->SD_csd.ContentProtectAppli  = (tmp&0x01);

    tmp = (u8)((CSD_Tab[3]&0x0000FF00)>>8);  //��14���ֽ�
    cardinfo->SD_csd.FileFormatGrouop = (tmp&0x80)>>7;
    cardinfo->SD_csd.CopyFlag         = (tmp&0x40)>>6;
    cardinfo->SD_csd.PermWrProtect    = (tmp&0x20)>>5;
    cardinfo->SD_csd.TempWrProtect    = (tmp&0x10)>>4;
    cardinfo->SD_csd.FileFormat       = (tmp&0x0C)>>2;
    cardinfo->SD_csd.ECC              = (tmp&0x03);

    tmp = (u8)(CSD_Tab[3]&0x000000FF);  //��15���ֽ�
    cardinfo->SD_csd.CSD_CRC   = (tmp&0xFE)>>1;
    cardinfo->SD_csd.Reserved4 = 1;

    tmp = (u8)((CID_Tab[0]&0xFF000000)>>24);   //��0���ֽ�
    cardinfo->SD_cid.ManufacturerID  = tmp;
    tmp = (u8)((CID_Tab[0]&0x00FF0000)>>16);   //��1���ֽ�
    cardinfo->SD_cid.OEM_AppliID     = tmp<<8;
    tmp = (u8)((CID_Tab[0]&0x000000FF00)>>8);  //��2���ֽ�
    cardinfo->SD_cid.OEM_AppliID    |= tmp;
    tmp = (u8)(CID_Tab[0]&0x000000FF);         //��3���ֽ�
    cardinfo->SD_cid.ProdName1       = tmp<<24;
    tmp = (u8)((CID_Tab[1]&0xFF000000)>>24);   //��4���ֽ�
    cardinfo->SD_cid.ProdName1      |= tmp<<16;
    tmp = (u8)((CID_Tab[1]&0x00FF0000)>>16);   //��5���ֽ�
    cardinfo->SD_cid.ProdName1      |= tmp<<8;
    tmp = (u8)((CID_Tab[1]&0x0000FF00)>>8);    //��6���ֽ�
    cardinfo->SD_cid.ProdName1      |= tmp;
    tmp = (u8)(CID_Tab[1]&0x000000FF);         //��7���ֽ�
    cardinfo->SD_cid.ProdName2       = tmp;
    tmp = (u8)((CID_Tab[2]&0xFF000000)>>24);   //��8���ֽ�
    cardinfo->SD_cid.ProdRev         = tmp;
    tmp = (u8)((CID_Tab[2]&0x00FF0000)>>16);   //��9���ֽ�
    cardinfo->SD_cid.ProdSN          = tmp<<24;
    tmp = (u8)((CID_Tab[2]&0x0000FF00)>>8);    //��10���ֽ�
    cardinfo->SD_cid.ProdSN         |= tmp<<16;
    tmp = (u8)(CID_Tab[2]&0x000000FF);         //��11���ֽ�
    cardinfo->SD_cid.ProdSN         |= tmp<<8;
    tmp = (u8)((CID_Tab[3]&0xFF000000)>>24);   //��12���ֽ�
    cardinfo->SD_cid.ProdSN         |= tmp;
    tmp = (u8)((CID_Tab[3]&0x00FF0000)>>16);   //��13���ֽ�
    cardinfo->SD_cid.Reserved1      |= (tmp&0xF0)>>4;
    cardinfo->SD_cid.ManufactDate    = (tmp&0x0F)<<8;
    tmp = (u8)((CID_Tab[3]&0x0000FF00)>>8);   //��14���ֽ�
    cardinfo->SD_cid.ManufactDate   |= tmp;
    tmp = (u8)(CID_Tab[3]&0x000000FF);        //��15���ֽ�
    cardinfo->SD_cid.CID_CRC         = (tmp&0xFE)>>1;
    cardinfo->SD_cid.Reserved2       = 1;
    return errorstatus;
}
 
/**
 * @brief ����SDMMC���߿��(MMC����֧��4bitģʽ)
 * 
 * @param wmode λ��ģʽ 0:1λ���ݿ�� 1:4λ���ݿ�� 2:8λ���ݿ��
 * @return SD_Error SD������״̬
 */
SD_Error sd_widebus_config(u32 wmode)
{
    SD_Error errorstatus=SD_OK;
    u16 clkcr = 0;
    if(MULTIMEDIA_CARD == CardType)  //MMC����֧��
        return SD_UNSUPPORTED_FEATURE;
    else if((STD_CAPACITY_SD_CARD_V1_1==CardType) || (STD_CAPACITY_SD_CARD_V2_0==CardType) || (HIGH_CAPACITY_SD_CARD==CardType))
    {
        if(wmode >= 2)  //��֧��8λģʽ
            return SD_UNSUPPORTED_FEATURE;
        else
        {
            errorstatus = sd_widebus_enable(wmode);
            if(SD_OK == errorstatus)
            {
                clkcr  = SDMMC1->CLKCR;   //��ȡCLKCR��ֵ
                clkcr &= ~(3<<14);        //���֮ǰ��λ������
                clkcr |= (u32)wmode<<14;  //1λ/4λ���߿��
                clkcr |= 0<<17;           //������Ӳ��������
                SDMMC1->CLKCR = clkcr;    //��������CLKCRֵ
            }
        }
    } 
    return errorstatus;
}

/**
 * @brief ѡ��SD��,����CMD7,ѡ����Ե�ַ(rca)Ϊaddr�Ŀ�,ȡ��������.���Ϊ0,�򶼲�ѡ��
 * 
 * @param addr ����RCA��ַ
 * @return SD_Error SD������״̬
 */
SD_Error sd_select_deselect(u32 addr)
{
    sdmmc_send_cmd(SD_CMD_SEL_DESEL_CARD, 1, addr);  //����CMD7,ѡ��,����Ӧ
    return cmd_resp1_error(SD_CMD_SEL_DESEL_CARD);
}

/**
 * @brief SD����ȡ����/�����
 * 
 * @param buf     �����ݻ�����
 * @param addr    ��ȡ��ַ
 * @param blksize ���С
 * @param nblks   Ҫ��ȡ�Ŀ���,1,��ʾ��ȡ������
 * @return SD_Error SD������״̬
 */
SD_Error sd_read_blocks(u8 *buf, long long addr, u16 blksize, u32 nblks)
{
    SD_Error errorstatus=SD_OK;
    u32 count     = 0;
    u32 timeout   = SDMMC_DATATIMEOUT;
    u32 *tempbuff = (u32*)buf;  //ת��Ϊu32ָ��
    SDMMC1->DCTRL = 0x0;        //���ݿ��ƼĴ�������(��DMA)
    if(CardType == HIGH_CAPACITY_SD_CARD)  //��������
    {
        blksize = 512;
        addr  >>= 9;
    }
    sdmmc_send_cmd(SD_CMD_SET_BLOCKLEN, 1, blksize);     //����CMD16+�������ݳ���Ϊblksize,����Ӧ
    errorstatus = cmd_resp1_error(SD_CMD_SET_BLOCKLEN);  //�ȴ�R1��Ӧ
    if(errorstatus != SD_OK)
    {
        printf("sdmmc_send_cmd=%d\r\n", errorstatus);
        return errorstatus;  //��Ӧ����
    }
    sdmmc_send_data_config(SD_DATATIMEOUT, nblks*blksize, 9, 1);  //nblks*blksize,���С��Ϊ512,����������
    SDMMC1->CMD |= 1<<6;  //CMDTRANS=1,����һ�����ݴ�������
    if(nblks > 1)         //����
    {
        sdmmc_send_cmd(SD_CMD_READ_MULT_BLOCK, 1, addr);        //����CMD18+��addr��ַ����ȡ����,����Ӧ
        errorstatus = cmd_resp1_error(SD_CMD_READ_MULT_BLOCK);  //�ȴ�R1��Ӧ
        if(errorstatus != SD_OK)
        {
            printf("SD_CMD_READ_MULT_BLOCK Error\r\n");
            return errorstatus;  //��Ӧ����
        }
    }
    else  //�����
    { 
        sdmmc_send_cmd(SD_CMD_READ_SINGLE_BLOCK, 1, addr);        //����CMD17+��addr��ַ����ȡ����,����Ӧ
        errorstatus = cmd_resp1_error(SD_CMD_READ_SINGLE_BLOCK);  //�ȴ�R1��Ӧ
        if(errorstatus != SD_OK)  //��Ӧ����
            return errorstatus;
    }
    INTX_DISABLE();  //�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDMMC��д����)
    while(!(SDMMC1->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8))))//������/CRC/��ʱ/���(��־)
    {
        if(SDMMC1->STA&(1<<15))  //����������,��ʾ���ٴ���8����
        {
            for(count=0; count<8; count++)  //ѭ����ȡ����
                *(tempbuff+count) = SDMMC1->FIFO;
            tempbuff += 8;
            timeout   = 0X7FFFFF;  //���������ʱ��
        }
        else  //����ʱ
        {
            if(timeout == 0)
                return SD_DATA_TIMEOUT;
            timeout--;
        }
    } 
    SDMMC1->CMD &= ~(1<<6);      //CMDTRANS=0,�������ݴ���
    INTX_ENABLE();               //�������ж�
    if(SDMMC1->STA&(1<<3))       //���ݳ�ʱ����
    {
        SDMMC1->ICR |= 1<<3;     //������־
        return SD_DATA_TIMEOUT;
    }
    else if(SDMMC1->STA&(1<<1))  //���ݿ�CRC����
    {
        SDMMC1->ICR |= 1<<1;     //������־
        if(nblks>1)              //��Կ��ܳ��ֵ�CRC����,����Ƕ���ȡ,���뷢�ͽ�����������
        {
            sdmmc_send_cmd(SD_CMD_STOP_TRANSMISSION, 1, 0);           //����CMD12+��������
            errorstatus = cmd_resp1_error(SD_CMD_STOP_TRANSMISSION);  //�ȴ�R1��Ӧ
        }
        return SD_DATA_CRC_FAIL;
    }
    else if(SDMMC1->STA&(1<<5))  //����fifo�������
    {
        SDMMC1->ICR |= 1<<5;     //������־
        return SD_RX_OVERRUN;
    }  
    if((SDMMC1->STA&(1<<8)) && (nblks>1))  //�����ս���,���ͽ���ָ��
    {
        if((STD_CAPACITY_SD_CARD_V1_1==CardType) || (STD_CAPACITY_SD_CARD_V2_0==CardType) || (HIGH_CAPACITY_SD_CARD==CardType))
        {
            sdmmc_send_cmd(SD_CMD_STOP_TRANSMISSION, 1, 0);           //����CMD12+��������
            errorstatus = cmd_resp1_error(SD_CMD_STOP_TRANSMISSION);  //�ȴ�R1��Ӧ
            if(errorstatus!=SD_OK)return errorstatus;
        }
    }
    SDMMC1->ICR = 0X1FE00FFF;  //������б��
    return errorstatus;
}
	
/**
 * @brief SD��д����/�����
 * 
 * @param buf ���ݻ�����
 * @param addr д��ַ
 * @param blksize ���С
 * @param nblks Ҫ��ȡ�Ŀ���,1,��ʾ��ȡ������
 * @return SD_Error SD������״̬
 */
SD_Error sd_write_blocks(u8 *buf, long long addr, u16 blksize, u32 nblks)
{
    SD_Error errorstatus = SD_OK;
    u8  cardstate        = 0;
    u32 timeout          = 0;
    u32 bytestransferred = 0;
    u32 cardstatus       = 0;
    u32 count            = 0;
    u32 restwords        = 0;
    u32 tlen             = nblks*blksize;  //�ܳ���(�ֽ�)
    u32 *tempbuff        = (u32*)buf;
    if(buf == NULL)  //��������
        return SD_INVALID_PARAMETER;
    SDMMC1->DCTRL = 0x0;  //���ݿ��ƼĴ�������(��DMA)
    if(CardType == HIGH_CAPACITY_SD_CARD)  //��������
    {
        blksize = 512;
        addr  >>= 9;
    }     
    sdmmc_send_cmd(SD_CMD_SET_BLOCKLEN, 1, blksize);     //����CMD16+�������ݳ���Ϊblksize,����Ӧ
    errorstatus = cmd_resp1_error(SD_CMD_SET_BLOCKLEN);  //�ȴ�R1��Ӧ
    if(errorstatus != SD_OK)  //��Ӧ����
        return errorstatus;
    if(nblks>1)  //���д
    {
        if(nblks*blksize > SD_MAX_DATA_LENGTH)
            return SD_INVALID_PARAMETER;
        if((STD_CAPACITY_SD_CARD_V1_1==CardType) || (STD_CAPACITY_SD_CARD_V2_0==CardType) || (HIGH_CAPACITY_SD_CARD==CardType))
        {
            //�������
            sdmmc_send_cmd(SD_CMD_APP_CMD, 1, (u32)RCA<<16);  //����ACMD55,����Ӧ
            errorstatus = cmd_resp1_error(SD_CMD_APP_CMD);    //�ȴ�R1��Ӧ
            if(errorstatus != SD_OK)
                return errorstatus;
            sdmmc_send_cmd(SD_CMD_SET_BLOCK_COUNT, 1, nblks);       //����CMD23,���ÿ�����,����Ӧ
            errorstatus = cmd_resp1_error(SD_CMD_SET_BLOCK_COUNT);  //�ȴ�R1��Ӧ
            if(errorstatus != SD_OK)
                return errorstatus;
        } 
        sdmmc_send_cmd(SD_CMD_WRITE_MULT_BLOCK, 1, addr);        //����CMD25,���дָ��,����Ӧ
        errorstatus = cmd_resp1_error(SD_CMD_WRITE_MULT_BLOCK);  //�ȴ�R1��Ӧ
    }
    else  //����д
    { 
        sdmmc_send_cmd(SD_CMD_SEND_STATUS, 1, (u32)RCA<<16);  //����CMD13,��ѯ����״̬,����Ӧ
        errorstatus = cmd_resp1_error(SD_CMD_SEND_STATUS);    //�ȴ�R1��Ӧ
        if(errorstatus != SD_OK)
            return errorstatus;
        cardstatus = SDMMC1->RESP1;
        timeout    = SD_DATATIMEOUT;
        while(((cardstatus&0x00000100)==0) && (timeout>0))  //���READY_FOR_DATAλ�Ƿ���λ
        {
            timeout--;
            sdmmc_send_cmd(SD_CMD_SEND_STATUS, 1, (u32)RCA<<16);  //����CMD13,��ѯ����״̬,����Ӧ
            errorstatus = cmd_resp1_error(SD_CMD_SEND_STATUS);    //�ȴ�R1��Ӧ
            if(errorstatus != SD_OK)
                return errorstatus;
            cardstatus = SDMMC1->RESP1;
        }
        if(timeout == 0)
            return SD_ERROR;
        sdmmc_send_cmd(SD_CMD_WRITE_SINGLE_BLOCK, 1, addr);        //����CMD24,д����ָ��,����Ӧ
        errorstatus = cmd_resp1_error(SD_CMD_WRITE_SINGLE_BLOCK);  //�ȴ�R1��Ӧ
    }
    if(errorstatus != SD_OK)
        return errorstatus;
    sdmmc_send_data_config(SD_DATATIMEOUT, nblks*blksize, 9, 0);  //blksize,���С��Ϊ512�ֽ�,����������
    SDMMC1->CMD |= 1<<6;                                          //CMDTRANS=1,����һ�����ݴ�������
    timeout = SDMMC_DATATIMEOUT; 
    INTX_DISABLE();  //�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDMMC��д����)
    while(!(SDMMC1->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3))))  //����/CRC/���ݽ���/��ʱ
    {
        if(SDMMC1->STA&(1<<14))  //���������,��ʾ���ٴ���8��(32�ֽ�)
        {
            if((tlen-bytestransferred) < SD_HALFFIFOBYTES)  //����32�ֽ���
            {
                restwords = ((tlen-bytestransferred)%4==0) ? ((tlen-bytestransferred)/4) : ((tlen-bytestransferred)/4+1);
                for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
                {
                    SDMMC1->FIFO=*tempbuff;
                }
            }
            else  //���������,���Է�������8��(32�ֽ�)����
            {
                for(count=0; count<SD_HALFFIFO; count++)
                {
                    SDMMC1->FIFO = *(tempbuff+count);
                }
                tempbuff         += SD_HALFFIFO;
                bytestransferred += SD_HALFFIFOBYTES;
            }
            timeout = 0X3FFFFFFF;  //д�������ʱ��
        }
        else
        {
            if(timeout==0)
                return SD_DATA_TIMEOUT;
            timeout--;
        }
    }
    SDMMC1->CMD &= ~(1<<6);      //CMDTRANS=0,�������ݴ���
    INTX_ENABLE();               //�������ж�
    if(SDMMC1->STA&(1<<3))       //���ݳ�ʱ����
    {
        SDMMC1->ICR |= 1<<3;     //������־
        return SD_DATA_TIMEOUT;
    }
    else if(SDMMC1->STA&(1<<1))  //���ݿ�CRC����
    {
        SDMMC1->ICR |= 1<<1;     //������־
        if(nblks > 1)            //��Կ��ܳ��ֵ�CRC����,����Ƕ���ȡ,���뷢�ͽ�����������
        {
            sdmmc_send_cmd(SD_CMD_STOP_TRANSMISSION, 1, 0);           //����CMD12+��������
            errorstatus = cmd_resp1_error(SD_CMD_STOP_TRANSMISSION);  //�ȴ�R1��Ӧ
        }	
        return SD_DATA_CRC_FAIL;
    }
    else if(SDMMC1->STA&(1<<4))  //����fifo�������
    {
        SDMMC1->ICR |= 1<<4;     //������־
        return SD_TX_UNDERRUN;
    }
    if((SDMMC1->STA&(1<<8)) && (nblks>1))  //��鷢�ͽ���,���ͽ���ָ��
    {
        if((STD_CAPACITY_SD_CARD_V1_1==CardType) || (STD_CAPACITY_SD_CARD_V2_0==CardType) || (HIGH_CAPACITY_SD_CARD==CardType))
        {
            sdmmc_send_cmd(SD_CMD_STOP_TRANSMISSION, 1, 0);           //����CMD12+��������
            errorstatus = cmd_resp1_error(SD_CMD_STOP_TRANSMISSION);  //�ȴ�R1��Ӧ
            if(errorstatus != SD_OK)
                return errorstatus;
        }
    }
    SDMMC1->ICR = 0X1FE00FFF;  //������б��
    errorstatus = is_card_programming(&cardstate);
    while((errorstatus==SD_OK) && ((cardstate==SD_CARD_PROGRAMMING) || (cardstate==SD_CARD_RECEIVING)))
    {
        errorstatus=is_card_programming(&cardstate);
    }
    return errorstatus;
}

/**
 * @brief ���CMD0��ִ��״̬
 * 
 * @return SD_Error SD������״̬
 */
SD_Error cmd_error(void)
{
    SD_Error errorstatus = SD_OK;
    u32 timeout          = SDMMC_CMD0TIMEOUT;
    while(timeout--)
    {
        if(SDMMC1->STA&(1<<7))  //�����ѷ���(������Ӧ)
            break;
    }	    
    if(timeout == 0)
        return SD_CMD_RSP_TIMEOUT;
    SDMMC1->ICR = 0X1FE00FFF;  //������
    return errorstatus;
}

/**
 * @brief ���R7��Ӧ����Ĵ���
 * 
 * @return SD_Error SD������״̬
 */
SD_Error cmd_resp7_error(void)
{
    SD_Error errorstatus = SD_OK;
    u32 status;
    u32 timeout = SDMMC_CMD0TIMEOUT;
    while(timeout--)
    {
        status = SDMMC1->STA;
        if(status&((1<<0)|(1<<2)|(1<<6)))   //CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
            break;
    }
    if((timeout==0) || (status&(1<<2)))     //��Ӧ��ʱ
    {
        errorstatus  = SD_CMD_RSP_TIMEOUT;  //��ǰ������2.0���ݿ�,���߲�֧���趨�ĵ�ѹ��Χ
        SDMMC1->ICR |= 1<<2;                //���������Ӧ��ʱ��־
        return errorstatus;
    }
    if(status&1<<6)                         //�ɹ����յ���Ӧ
    {
        errorstatus  = SD_OK;
        SDMMC1->ICR |= 1<<6;                //�����Ӧ��־
    }
    return errorstatus;
}

/**
 * @brief ���R1��Ӧ�Ĵ���״̬
 * 
 * @param cmd ��ǰ����
 * @return SD_Error SD������״̬
 */
SD_Error cmd_resp1_error(u8 cmd)
{	  
    u32 status; 
    while(1)
    {
        status=SDMMC1->STA;
        if(status&((1<<0)|(1<<2)|(1<<6)))  //CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
            break;
    }
    if(status&(1<<2))              //��Ӧ��ʱ
    {
        SDMMC1->ICR = 1<<2;        //���������Ӧ��ʱ��־
        SDMMC1->ICR = 0X1FE00FFF;  //������
        return SD_CMD_RSP_TIMEOUT;
    }
     if(status&(1<<0))             //CRC����
    {
        SDMMC1->ICR = 1<<0;        //�����־
        return SD_CMD_CRC_FAIL;
    }
    if(SDMMC1->RESPCMD!=cmd)
        return SD_ILLEGAL_CMD;     //���ƥ��
    SDMMC1->ICR=0X1FE00FFF;        //������
    return  (SD_Error)(SDMMC1->RESP1&SD_OCR_ERRORBITS);  //���ؿ���Ӧ
}

/**
 * @brief ���R3��Ӧ�Ĵ���״̬
 * 
 * @return SD_Error SD������״̬
 */
SD_Error cmd_resp3_error(void)
{
    u32 status;
    while(1)
    {
        status = SDMMC1->STA;
        if(status & ((1<<0)|(1<<2)|(1<<6)))  //CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
            break;
    }
     if(status&(1<<2))  //��Ӧ��ʱ
    {
        SDMMC1->ICR |= 1<<2;  //���������Ӧ��ʱ��־
        return SD_CMD_RSP_TIMEOUT;
    }
    SDMMC1->ICR = 0X1FE00FFF;  //������
    return SD_OK;
}

/**
 * @brief ���R2��Ӧ�Ĵ���״̬
 * 
 * @return SD_Error SD������״̬
 */
SD_Error cmd_resp2_error(void)
{
    SD_Error errorstatus = SD_OK;
    u32 status;
    u32 timeout = SDMMC_CMD0TIMEOUT;
     while(timeout--)
    {
        status = SDMMC1->STA;
        if(status & ((1<<0)|(1<<2)|(1<<6)))  //CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
            break;
    }
    if((timeout==0) || (status&(1<<2)))  //��Ӧ��ʱ
    {
        errorstatus  = SD_CMD_RSP_TIMEOUT; 
        SDMMC1->ICR |= 1<<2;  //���������Ӧ��ʱ��־
        return errorstatus;
    }
    if(status&1<<0)  //CRC����
    {
        errorstatus  = SD_CMD_CRC_FAIL;
        SDMMC1->ICR |= 1<<0;  //�����Ӧ��־
    }
    SDMMC1->ICR = 0X1FE00FFF;  //������
    return errorstatus;
}

/**
 * @brief ���R6��Ӧ�Ĵ���״̬
 * 
 * @param cmd  ֮ǰ���͵�����
 * @param prca �����ص�RCA��ַ
 * @return SD_Error SD������״̬
 */
SD_Error cmd_resp6_error(u8 cmd, u16*prca)
{
    SD_Error errorstatus = SD_OK;
    u32 status;
    u32 rspr1;
     while(1)
    {
        status=SDMMC1->STA;
        if(status&((1<<0)|(1<<2)|(1<<6)))  //CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
            break;
    }
    if(status&(1<<2))              //��Ӧ��ʱ
    {
        SDMMC1->ICR |= 1<<2;       //���������Ӧ��ʱ��־
        return SD_CMD_RSP_TIMEOUT;
    }
    if(status&1<<0)  //CRC����
    {
        SDMMC1->ICR |= 1<<0;       //�����Ӧ��־
        return SD_CMD_CRC_FAIL;
    }
    if(SDMMC1->RESPCMD != cmd)     //�ж��Ƿ���Ӧcmd����
    {
        return SD_ILLEGAL_CMD;
    }
    SDMMC1->ICR = 0X1FE00FFF;      //������б��
    rspr1       = SDMMC1->RESP1;   //�õ���Ӧ
    if(SD_ALLZERO == (rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
    {
        *prca = (u16)(rspr1>>16);  //����16λ�õ�,rca
        return errorstatus;
    }
    if(rspr1&SD_R6_GENERAL_UNKNOWN_ERROR)
        return SD_GENERAL_UNKNOWN_ERROR;
    if(rspr1&SD_R6_ILLEGAL_CMD)
        return SD_ILLEGAL_CMD;
    if(rspr1&SD_R6_COM_CRC_FAILED)
        return SD_COM_CRC_FAILED;
    return errorstatus;
}

/**
 * @brief SDMMCʹ�ܿ�����ģʽ
 * 
 * @param enx 0:��ʹ�� 1:ʹ��
 * @return SD_Error SD������״̬
 */
SD_Error sd_widebus_enable(u8 enx)
{
    SD_Error errorstatus = SD_OK;
    u32 scr[2] = {0,0};
    u8 arg     = 0X00;
    if(enx)
        arg = 0X02;
    else
        arg = 0X00;
    if(SDMMC1->RESP1&SD_CARD_LOCKED)   //SD������LOCKED״̬
        return SD_LOCK_UNLOCK_FAILED;
    errorstatus = find_scr(RCA, scr);  //�õ�SCR�Ĵ�������
    if(errorstatus != SD_OK)
        return errorstatus;
    if((scr[1]&SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)     //֧�ֿ�����
    {
        sdmmc_send_cmd(SD_CMD_APP_CMD, 1, (u32)RCA<<16);  //����CMD55+RCA,����Ӧ
        errorstatus = cmd_resp1_error(SD_CMD_APP_CMD);
        if(errorstatus != SD_OK)
            return errorstatus;
        sdmmc_send_cmd(SD_CMD_APP_SD_SET_BUSWIDTH, 1, arg);  //����ACMD6,����Ӧ,����:10,4λ;00,1λ
        errorstatus = cmd_resp1_error(SD_CMD_APP_SD_SET_BUSWIDTH);
        return errorstatus;
    }
    else
        return SD_REQUEST_NOT_APPLICABLE;  //��֧�ֿ���������
}

/**
 * @brief ��鿨�Ƿ�����ִ��д����
 * 
 * @param pstatus ��ǰ״̬
 * @return SD_Error SD������״̬
 */
SD_Error is_card_programming(u8 *pstatus)
{
    vu32 respR1 = 0;
    vu32 status = 0;
    sdmmc_send_cmd(SD_CMD_SEND_STATUS, 1, (u32)RCA<<16);  //����CMD13
    status = SDMMC1->STA;
    while(!(status&((1<<0)|(1<<6)|(1<<2))))  //�ȴ��������
        status = SDMMC1->STA;
    if(status&(1<<0))                        //CRC���ʧ��
    {
        SDMMC1->ICR |= 1<<0;                 //���������
        return SD_CMD_CRC_FAIL;
    }
    if(status&(1<<2))                        //���ʱ
    {
        SDMMC1->ICR |= 1<<2;                 //���������
        return SD_CMD_RSP_TIMEOUT;
    }
    if(SDMMC1->RESPCMD != SD_CMD_SEND_STATUS)
        return SD_ILLEGAL_CMD;
    SDMMC1->ICR = 0X1FE00FFF;                //������б��
    respR1      = SDMMC1->RESP1;
    *pstatus    = (u8)((respR1>>9)&0x0000000F);
    return SD_OK;
}

/**
 * @brief ��ȡ��ǰ��״̬
 * 
 * @param pcardstatus ��״̬
 * @return SD_Error SD������״̬
 */
SD_Error sd_send_status(uint32_t *pcardstatus)
{
    SD_Error errorstatus = SD_OK;
    if(pcardstatus == NULL)
    {
        errorstatus = SD_INVALID_PARAMETER;
        return errorstatus;
    }
    sdmmc_send_cmd(SD_CMD_SEND_STATUS, 1, RCA<<16);     //����CMD13,����Ӧ
    errorstatus = cmd_resp1_error(SD_CMD_SEND_STATUS);  //��ѯ��Ӧ״̬
    if(errorstatus != SD_OK)
        return errorstatus;
    *pcardstatus = SDMMC1->RESP1;                       //��ȡ��Ӧֵ
    return errorstatus;
}

/**
 * @brief ��ȡSD����״̬
 * 
 * @return SDCardState SD��״̬
 */
SDCardState sd_get_status(void)
{
    u32 resp1=0;
    if(sd_send_status(&resp1) != SD_OK)
        return SD_CARD_ERROR;
    else
        return (SDCardState)((resp1>>9) & 0x0F);
}

/**
 * @brief ����SD����SCR�Ĵ���ֵ
 * 
 * @param rca  ����Ե�ַ
 * @param pscr ���ݻ�����(�洢SCR����)
 * @return SD_Error SD������״̬
 */
SD_Error find_scr(u16 rca, u32 *pscr)
{  
    SD_Error errorstatus = SD_OK;
    u32 tempscr[2]       = {0,0};  
    sdmmc_send_cmd(SD_CMD_SET_BLOCKLEN, 1, 8);        //����CMD16,����Ӧ,����Block SizeΪ8�ֽ�
    errorstatus = cmd_resp1_error(SD_CMD_SET_BLOCKLEN);
    if(errorstatus != SD_OK)
        return errorstatus;
    sdmmc_send_cmd(SD_CMD_APP_CMD, 1, (u32)rca<<16);  //����CMD55,����Ӧ
    errorstatus = cmd_resp1_error(SD_CMD_APP_CMD);
    if(errorstatus != SD_OK)
        return errorstatus;
    sdmmc_send_data_config(SD_DATATIMEOUT, 8, 3, 1);  //8���ֽڳ���,blockΪ8�ֽ�,SD����SDMMC
    sdmmc_send_cmd(SD_CMD_SD_APP_SEND_SCR, 1, 0);     //����ACMD51,����Ӧ,����Ϊ0
    errorstatus=cmd_resp1_error(SD_CMD_SD_APP_SEND_SCR);
    if(errorstatus != SD_OK)
        return errorstatus;
    while(!(SDMMC1->STA&(SDMMC_STA_RXOVERR|SDMMC_STA_DCRCFAIL|SDMMC_STA_DTIMEOUT|SDMMC_STA_DBCKEND|SDMMC_STA_DATAEND)))
    {
        if(!(SDMMC1->STA&(1<<19)))      //����FIFO���ݿ���
        {
            tempscr[0] = SDMMC1->FIFO;  //��ȡFIFO����
            tempscr[1] = SDMMC1->FIFO;  //��ȡFIFO����
            break;
        }
    }
    if(SDMMC1->STA&(1<<3))       //�������ݳ�ʱ
    {
        SDMMC1->ICR |= 1<<3;     //������
        return SD_DATA_TIMEOUT;
    }
    else if(SDMMC1->STA&(1<<1))  //�ѷ���/���յ����ݿ�CRCУ�����
    {
        SDMMC1->ICR |= 1<<1;     //������
        return SD_DATA_CRC_FAIL;
    }
    else if(SDMMC1->STA&(1<<5))  //����FIFO���
    {
        SDMMC1->ICR |= 1<<5;     //������
        return SD_RX_OVERRUN;
    }
    SDMMC1->ICR = 0X1FE00FFF;    //������
    //������˳��8λΪ��λ������
    *(pscr+1) = ((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
    *(pscr)   = ((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
    return errorstatus;
}

/**
 * @brief ��SD��
 * 
 * @param buf    �����ݻ�����
 * @param sector ������ַ
 * @param cnt    ��������
 * @return u8 ����״̬ 0:���� ����:�������
 */
u8 sd_read_disk(u8*buf, u32 sector, u32 cnt)
{
    u8 sta = SD_OK;
    long long lsector = sector;
    u32 n;
    if(CardType != STD_CAPACITY_SD_CARD_V1_1)
        lsector <<= 9;
    if((u32)buf%4 != 0)
    {
        for(n=0; n<cnt; n++)
        {
            sta  = sd_read_blocks(SDMMC_DATA_BUFFER, lsector+512*n, 512, 1);  //����sector�Ķ�����
            memcpy(buf, SDMMC_DATA_BUFFER, 512);
            buf += 512;
        } 
    }
    else
        sta = sd_read_blocks(buf, lsector, 512, cnt);  //����/���sector
    return sta;
}

/**
 * @brief дSD��
 * 
 * @param buf    д���ݻ�����
 * @param sector ������ַ
 * @param cnt    ��������
 * @return u8 ����״̬ 0:���� ����:�������
 */
u8 sd_write_disk(u8*buf, u32 sector, u32 cnt)
{
    u8 sta = SD_OK;
    u32 n;
    long long lsector = sector;
    if(CardType != STD_CAPACITY_SD_CARD_V1_1)
        lsector <<= 9;
    if((u32)buf%4 != 0)
    {
         for(n=0; n<cnt; n++)
        {
            memcpy(SDMMC_DATA_BUFFER, buf, 512);
            sta  = sd_write_blocks(SDMMC_DATA_BUFFER, lsector+512*n, 512, 1);  //����sector��д����
            buf += 512;
        } 
    }
    else
        sta = sd_write_blocks(buf, lsector, 512, cnt);  //����/���sector   
    return sta;
}
