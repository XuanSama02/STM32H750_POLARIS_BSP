#include "jpegcodec.h"
#include "usart.h"
#include "malloc.h"

//JPEG�淶(ISO/IEC 10918-1��׼)������������,���ڻ�ȡJPEG����
const u8 JPEG_LUM_QuantTable[JPEG_QUANT_TABLE_SIZE] =
{
    16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
    14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
};
const u8 JPEG_ZIGZAG_ORDER[JPEG_QUANT_TABLE_SIZE] =
{
    0,   1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
}; 

/**
 * @brief JPEGӲ����������DMA���ú���
 * 
 * @param meminaddr JPEG����DMA�洢����ַ
 * @param meminsize ����DMA���ݳ���,0~262143,���ֽ�Ϊ��λ
 */
void jpeg_in_dma_init(u32 meminaddr, u32 meminsize)
{ 
    u32 regval   = 0;
    u32 addrmask = 0;
    RCC->AHB3ENR         |= 1<<0;             //ʹ��MDMAʱ�� 
    MDMA_Channel7->CCR    = 0;                //����MDMA����
    while(MDMA_Channel7->CCR&0X01);           //�ȴ�MDMA_Channel7�ر����
    MDMA_Channel7->CIFCR  = 0X1F;             //�жϱ�־����
    MDMA_Channel7->CCR   |= 1<<2;             //CTCIE=1,ʹ��ͨ����������ж�
    MDMA_Channel7->CCR   |= 2<<6;             //PL[1:0]=2,�����ȼ�
    MDMA_Channel7->CBNDTR = meminsize;        //���䳤��Ϊmeminsize
    MDMA_Channel7->CDAR   = (u32)&JPEG->DIR;  //Ŀ���ַΪ:JPEG->DIR
    MDMA_Channel7->CSAR   = meminaddr;        //meminaddr��ΪԴ��ַ
    regval  = 0<<28;                          //TRGM[1:0]=0,ÿ��MDMA���󴥷�һ��buffer����
    regval |= 1<<25;                          //PKE=1,���ʹ��
    regval |= (32-1)<<18;                     //TLEN[6:0]=31,buffer���䳤��Ϊ32�ֽ�.
    regval |= 4<<15;                          //DBURST[2:0]=4,Ŀ��ͻ�����䳤��Ϊ16
    regval |= 4<<12;                          //SBURST[2:0]=4,Դͻ�����䳤��Ϊ16
    regval |= 0<<8;                           //SINCOS[1:0]=0,Դ��ַ�仯��λΪ8λ(�ֽ�)
    regval |= 2<<6;                           //DSIZE[1:0]=2,Ŀ��λ��Ϊ32λ
    regval |= 0<<4;                           //SSIZE[1:0]=0,Դλ��Ϊ8λ
    regval |= 0<<2;                           //DINC[1:0]=0,Ŀ���ַ�̶�
    regval |= 2<<0;                           //SINC[1:0]=2,Դ��ַ����
    MDMA_Channel7->CTCR  = regval;            //����CTCR�Ĵ���
    MDMA_Channel7->CTBR  = 17<<0;             //MDMA��Ӳ������ͨ��17����inmdma,ͨ��17=JPEG input FIFO threshold,���<STM32H7xx�ο��ֲ�>550ҳ,table 91
    addrmask = meminaddr&0XFF000000;          //��ȡ����
    if(addrmask==0X20000000 || addrmask==0)   //ʹ��AHBS���߷���DTCM/ITCM
        MDMA_Channel7->CTBR |= 1<<16;
    HAL_NVIC_SetPriority(MDMA_IRQn,1,2);      //�����ж����ȼ�����ռ���ȼ�1�������ȼ�2
    HAL_NVIC_EnableIRQ(MDMA_IRQn);            //����MDMA�ж�
}  

/**
 * @brief JPEGӲ���������DMA����
 * 
 * @param memoutaddr JPEG���DMA�洢����ַ
 * @param memoutsize ���DMA���ݳ���,0~262143,���ֽ�Ϊ��λ
 */
void jpeg_out_dma_init(u32 memoutaddr, u32 memoutsize)
{ 
    u32 regval   = 0;
    u32 addrmask = 0;
    MDMA_Channel6->CCR    = 0;                //���MDMA����
    while(MDMA_Channel6->CCR&0X01);           //�ȴ�MDMA_Channel6�ر����
    MDMA_Channel6->CIFCR  = 0X1F;             //�жϱ�־����
    MDMA_Channel6->CCR   |= 3<<6;             //PL[1:0]=2,������ȼ�
    MDMA_Channel6->CCR   |= 1<<2;             //CTCIE=1,ʹ��ͨ����������ж�
    MDMA_Channel6->CBNDTR = memoutsize;       //���䳤��Ϊmeminsize
    MDMA_Channel6->CDAR   = memoutaddr;       //Ŀ���ַΪ:memoutaddr
    MDMA_Channel6->CSAR   = (u32)&JPEG->DOR;  //JPEG->DOR��ΪԴ��ַ
    regval  = 0<<28;                          //TRGM[1:0]=0,ÿ��MDMA���󴥷�һ��buffer����
    regval |= 1<<25;                          //PKE=1,���ʹ��
    regval |= (32-1)<<18;                     //TLEN[6:0]=31,buffer���䳤��Ϊ32�ֽ�.
    regval |= 4<<15;                          //DBURST[2:0]=4,Ŀ��ͻ�����䳤��Ϊ16
    regval |= 4<<12;                          //SBURST[2:0]=4,Դͻ�����䳤��Ϊ16
    regval |= 0<<10;                          //DINCOS[1:0]=0,Ŀ���ַ�仯��λΪ8λ(�ֽ�)
    regval |= 0<<6;                           //DSIZE[1:0]=0,Ŀ��λ��Ϊ8λ
    regval |= 2<<4;                           //SSIZE[1:0]=2,Դλ��Ϊ32λ
    regval |= 2<<2;                           //DINC[1:0]=2,Ŀ���ַ����
    regval |= 0<<0;                           //SINC[1:0]=0,Դ��ַ�̶�
    MDMA_Channel6->CTCR = regval;             //����CTCR�Ĵ���
    MDMA_Channel6->CTBR = 19<<0;              //MDMA��Ӳ������ͨ��17����inmdma,ͨ��17=JPEG input FIFO threshold,���<STM32H7xx�ο��ֲ�>550ҳ,table 91
    addrmask = memoutaddr&0XFF000000;         //��ȡ����
    if(addrmask==0X20000000 || addrmask==0)   //ʹ��AHBS���߷���DTCM/ITCM
        MDMA_Channel6->CTBR |= 1<<17;
    HAL_NVIC_SetPriority(MDMA_IRQn,1,2);      //�����ж����ȼ�����ռ���ȼ�1�������ȼ�2
    HAL_NVIC_EnableIRQ(MDMA_IRQn);            //����MDMA�ж�
}

void (*jpeg_in_callback)(void);   //JPEG DMA����ص�����
void (*jpeg_out_callback)(void);  //JPEG DMA��� �ص�����
void (*jpeg_eoc_callback)(void);  //JPEG ������� �ص�����
void (*jpeg_hdp_callback)(void);  //JPEG Header������� �ص�����

/**
 * @brief MDMA�жϷ�����,����Ӳ��JPEG����ʱ����/���������
 * 
 */
void MDMA_IRQHandler(void)
{
    if(MDMA_Channel7->CISR&(1<<1))  //CTCIF,ͨ��7�������(����)
    {
        MDMA_Channel7->CIFCR |= 1<<1;  //���ͨ����������ж�
        JPEG->CR &= ~(0X7E);           //�ر�JPEG�ж�,��ֹ�����
        jpeg_in_callback();            //ִ������ص�����,������ȡ����
        JPEG->CR |= 3<<5;              //ʹ��EOC��HPD�ж�
    }
    if(MDMA_Channel6->CISR&(1<<1))  //CTCIF,ͨ��6�������(���)
    {
        MDMA_Channel6->CIFCR |= 1<<1;  //���ͨ����������ж�
        JPEG->CR &= ~(0X7E);           //�ر�JPEG�ж�,��ֹ�����
        jpeg_out_callback();           //ִ������ص�����,������ת����RGB
        JPEG->CR |= 3<<5;              //ʹ��EOC��HPD�ж�
    }
}

/**
 * @brief JPEG�����жϷ�����
 * 
 */
void JPEG_IRQHandler(void)
{
    if(JPEG->SR&(1<<6))  //JPEG Header�������
    { 
        jpeg_hdp_callback();
        JPEG->CR  &= ~(1<<6);  //��ֹJpeg Header��������ж�
        JPEG->CFR |= 1<<6;     //���HPDFλ(header�������λ)
    }
    if(JPEG->SR&(1<<5))  //JPEG�������
    {
        jpeg_dma_stop();
        jpeg_eoc_callback();
        JPEG->CFR          |= 1<<5;     //���EOCλ(�������λ)
        MDMA_Channel6->CCR &= ~(1<<0);  //�ر�MDMAͨ��6
        MDMA_Channel7->CCR &= ~(1<<0);  //�ر�MDMAͨ��7
    }
}

/**
 * @brief ��ʼ��Ӳ��JPEG�ں�
 * 
 * @param tjpeg jpeg�������ƽṹ��
 * @return u8 0:�ɹ� ����:ʧ��
 */
u8 jpeg_core_init(jpeg_codec_typedef *tjpeg)
{
    u8 i;
    RCC->AHB3ENR |= 1<<5;  //ʹ��Ӳ��jpegʱ��
    for(i=0; i<JPEG_DMA_INBUF_NB; i++)
    {
        tjpeg->inbuf[i].buf = mymalloc(SRAMDTCM, JPEG_DMA_INBUF_LEN);  //�����ڴ�
        if(tjpeg->inbuf[i].buf == NULL)  //�ڴ����ʧ��,�˳�
        {
            jpeg_core_destroy(tjpeg);
            return 1;
        }
    }
    JPEG->CR      = 0;                    //������
    JPEG->CR     |= 1<<0;                 //ʹ��Ӳ��JPEG
    JPEG->CONFR0 &= ~(1<<0);              //ֹͣJPEG��������
    JPEG->CR     |= 1<<13;                //�������fifo
    JPEG->CR     |= 1<<14;                //������fifo
    JPEG->CFR     = 3<<5;                 //��ձ�־ 
    HAL_NVIC_SetPriority(JPEG_IRQn,1,3);  //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
    HAL_NVIC_EnableIRQ(JPEG_IRQn);        //����JPEG�ж� 
    JPEG->CONFR1 |= 1<<8;                 //ʹ��header����
    return 0;
}

/**
 * @brief �ر�Ӳ��JPEG�ں�,���ͷ��ڴ�
 * 
 * @param tjpeg jpeg�������ƽṹ��
 */
void jpeg_core_destroy(jpeg_codec_typedef *tjpeg)
{
    u8 i; 
    jpeg_dma_stop();  //ֹͣDMA����
    for(i=0; i<JPEG_DMA_INBUF_NB; i++)   //�ͷ��ڴ�
        myfree(SRAMDTCM, tjpeg->inbuf[i].buf);
    for(i=0; i<JPEG_DMA_OUTBUF_NB; i++)  //�ͷ��ڴ�
        myfree(SRAMIN, tjpeg->outbuf[i].buf);
}

/**
 * @brief ��ʼ��Ӳ��JPEG������
 * 
 * @param tjpeg jpeg�������ƽṹ��
 */
void jpeg_decode_init(jpeg_codec_typedef *tjpeg)
{ 
    u8 i;
    tjpeg->inbuf_read_ptr   = 0;
    tjpeg->inbuf_write_ptr  = 0;
    tjpeg->indma_pause      = 0;
    tjpeg->outbuf_read_ptr  = 0;
    tjpeg->outbuf_write_ptr = 0;
    tjpeg->outdma_pause     = 0;
    tjpeg->state = JPEG_STATE_NOHEADER;  //ͼƬ���������־
    for(i=0; i<JPEG_DMA_INBUF_NB; i++)
    {
        tjpeg->inbuf[i].sta  = 0;
        tjpeg->inbuf[i].size = 0;
    }
    for(i=0; i<JPEG_DMA_OUTBUF_NB; i++)
    {
        tjpeg->outbuf[i].sta  = 0;
        tjpeg->outbuf[i].size = 0;
    }
    MDMA_Channel6->CCR   = 0;           //MDMAͨ��6��ֹ
    MDMA_Channel7->CCR   = 0;           //MDMAͨ��7��ֹ
    MDMA_Channel6->CIFCR = 0X1F;        //�жϱ�־����
    MDMA_Channel7->CIFCR = 0X1F;        //�жϱ�־����
    JPEG->CONFR1        |= 1<<3;        //Ӳ��JPEG����ģʽ
    JPEG->CONFR0        &= ~(1<<0);     //ֹͣJPEG��������
    JPEG->CR            &= ~(0X3F<<1);  //�ر������ж�
    JPEG->CR            |= 1<<13;       //�������fifo
    JPEG->CR            |= 1<<14;       //������fifo
    JPEG->CR            |= 1<<6;        //ʹ��Jpeg Header��������ж�
    JPEG->CR            |= 1<<5;        //ʹ�ܽ�������ж�
    JPEG->CFR            = 3<<5;        //��ձ�־
    JPEG->CONFR0        |= 1<<0;        //ʹ��JPEG��������
}

/**
 * @brief ����JPEG IN DMA,��ʼ����JPEG
 * 
 */
void jpeg_in_dma_start(void)
{
    MDMA_Channel7->CCR |= 1<<0;  //ʹ��MDMAͨ��7�Ĵ���
}

/**
 * @brief ����JPEG OUT DMA,��ʼ���YUV����
 * 
 */
void jpeg_out_dma_start(void)
{
    MDMA_Channel6->CCR |= 1<<0;  //ʹ��MDMAͨ��6�Ĵ���
}

/**
 * @brief ֹͣJPEG DMA�������
 * 
 */
void jpeg_dma_stop(void)
{
    JPEG->CONFR0 &= ~(1<<0);     //ֹͣJPEG��������
    JPEG->CR     &= ~(0X3F<<1);  //�ر������ж�
    JPEG->CFR     = 3<<5;        //��ձ�־
}

/**
 * @brief �ָ�DMA IN����
 * 
 * @param memaddr �洢���׵�ַ
 * @param memlen  Ҫ�������ݳ���(���ֽ�Ϊ��λ)
 */
void jpeg_in_dma_resume(u32 memaddr, u32 memlen)
{  
    if(memlen%4)
        memlen += 4-memlen%4;         //��չ��4�ı���
    MDMA_Channel7->CIFCR  = 0X1F;     //�жϱ�־����
    MDMA_Channel7->CBNDTR = memlen;   //���䳤��Ϊmemlen
    MDMA_Channel7->CSAR   = memaddr;  //memaddr��ΪԴ��ַ
    MDMA_Channel7->CCR   |= 1<<0;     //ʹ��MDMAͨ��7�Ĵ���
}

/**
 * @brief �ָ�DMA OUT����
 * 
 * @param memaddr �洢���׵�ַ
 * @param memlen  Ҫ�������ݳ���(���ֽ�Ϊ��λ)
 */
void jpeg_out_dma_resume(u32 memaddr, u32 memlen)
{
    if(memlen%4)memlen    += 4-memlen%4;  //��չ��4�ı���
    MDMA_Channel6->CIFCR   = 0X1F;        //�жϱ�־����
    MDMA_Channel6->CBNDTR  = memlen;      //���䳤��Ϊmemlen
    MDMA_Channel6->CDAR    = memaddr;     //memaddr��ΪԴ��ַ
    MDMA_Channel6->CCR    |= 1<<0;        //ʹ��MDMAͨ��6�Ĵ���
}

/**
 * @brief ��ȡͼ����Ϣ
 * 
 * @param tjpeg jpeg����ṹ��
 */
void jpeg_get_info(jpeg_codec_typedef *tjpeg)
{ 
    u32 yblockNb;
    u32 cBblockNb;
    u32 cRblockNb;
    switch(JPEG->CONFR1&0X03)
    {
        case 0:  //grayscale,1 color component
            tjpeg->Conf.ColorSpace = JPEG_GRAYSCALE_COLORSPACE;
            break;
        case 2:  //YUV/RGB,3 color component
            tjpeg->Conf.ColorSpace = JPEG_YCBCR_COLORSPACE;
            break;
        case 3:  //CMYK,4 color component
            tjpeg->Conf.ColorSpace = JPEG_CMYK_COLORSPACE;
            break;
    }
    tjpeg->Conf.ImageHeight = (JPEG->CONFR1&0XFFFF0000)>>16;  //���ͼ��߶�
    tjpeg->Conf.ImageWidth  = (JPEG->CONFR3&0XFFFF0000)>>16;  //���ͼ����
    if((tjpeg->Conf.ColorSpace==JPEG_YCBCR_COLORSPACE) || (tjpeg->Conf.ColorSpace==JPEG_CMYK_COLORSPACE))
    {
        yblockNb  = (JPEG->CONFR4&(0XF<<4))>>4;
        cBblockNb = (JPEG->CONFR5&(0XF<<4))>>4;
        cRblockNb = (JPEG->CONFR6&(0XF<<4))>>4;
        if((yblockNb==1) && (cBblockNb==0) && (cRblockNb==0))  //16x8 block
            tjpeg->Conf.ChromaSubsampling = JPEG_422_SUBSAMPLING;
        else if((yblockNb==0) && (cBblockNb==0) && (cRblockNb==0))
            tjpeg->Conf.ChromaSubsampling = JPEG_444_SUBSAMPLING;
        else if((yblockNb==3) && (cBblockNb==0) && (cRblockNb==0))
            tjpeg->Conf.ChromaSubsampling = JPEG_420_SUBSAMPLING;
        else
            tjpeg->Conf.ChromaSubsampling = JPEG_444_SUBSAMPLING; 
    }
    else
        tjpeg->Conf.ChromaSubsampling = JPEG_444_SUBSAMPLING;  //Ĭ����4:4:4
    tjpeg->Conf.ImageQuality = 0;  //ͼ����������������ͼƬ����ĩβ,�տ�ʼ��ʱ��,���޷���ȡ��,����ֱ������Ϊ0
}

/**
 * @brief �õ�jpegͼ������,�ڽ�����ɺ�,���Ե��ò������ȷ�Ľ��
 * 
 * @return u8 ͼ������:0~100
 */
u8 jpeg_get_quality(void)
{
    u32 quality = 0;
    u32 quantRow;
    u32 quantVal;
    u32 scale;
    u32 i;
    u32 j;
    u32 *tableAddress = (u32*)JPEG->QMEM0;
    i = 0;
    while(i<JPEG_QUANT_TABLE_SIZE)
    {
        quantRow =* tableAddress;
        for(j=0; j<4; j++)
        {
            quantVal = (quantRow>>(8*j))&0xFF;
            if(quantVal==1)  //100%
                quality+=100;
            else
            {
                scale = (quantVal*100)/((u32)JPEG_LUM_QuantTable[JPEG_ZIGZAG_ORDER[i+j]]);
                if(scale<=100)
                    quality += (200-scale)/2;
                else
                    quality += 5000/scale;
            }
        }
        i += 4;
        tableAddress++;
    }
    return (quality/((u32)64));
}

/**
 * @brief ����DMA2D,��JPEG�����YUV����ת����RGB����,Ӳ������,�ٶȷǳ���
 * 
 * @param tjpeg jpeg����ṹ��
 * @param pdst  ��������׵�ַ
 * @return u8 0:��� 1:��ʱ�˳�
 */
u8 jpeg_dma2d_yuv2rgb_conversion(jpeg_codec_typedef *tjpeg, u32 *pdst)
{ 
    u32 regval=0;
    u32 cm          =0;  //������ʽ
    u32 destination =0;
    u32 timeout     =0;
    if(tjpeg->Conf.ChromaSubsampling ==JPEG_420_SUBSAMPLING)        //YUV420תRGB
        cm = DMA2D_CSS_420;
    if(tjpeg->Conf.ChromaSubsampling ==JPEG_422_SUBSAMPLING)        //YUV422תRGB
        cm = DMA2D_CSS_422;
    else if(tjpeg->Conf.ChromaSubsampling == JPEG_444_SUBSAMPLING)  //YUV444תRGB
        cm = DMA2D_NO_CSS;
    destination = (u32)pdst+(tjpeg->yuvblk_curheight*tjpeg->Conf.ImageWidth)*2;  //����Ŀ���ַ���׵�ַ
    RCC->AHB3ENR   |= 1<<4;          //ʹ��DMA2Dʱ��
    RCC->AHB3RSTR  |= 1<<4;          //��λDMA2D
    RCC->AHB3RSTR  &= ~(1<<4);       //������λ
    DMA2D->CR      &= ~(1<<0);       //��ֹͣDMA2D
    DMA2D->CR       = 1<<16;         //MODE[1:0]=01,�洢�����洢��,��PFCģʽ
    DMA2D->OPFCCR   = 2<<0;          //CM[2:0]=010,���ΪRGB565��ʽ
    DMA2D->OOR      = 0;             //������ƫ��Ϊ0
    DMA2D->IFCR    |= 1<<1;          //���������ɱ�־
    regval          = 11<<0;         //CM[3:0]=1011,��������ΪYCbCr��ʽ
    regval         |= cm<<18;        //CSS[1:0]=cm,Chroma Sub-Sampling:0,4:4:4;1,4:2:2;2,4:2:0
    DMA2D->FGPFCCR  = regval;        //����FGPCCR�Ĵ���
    DMA2D->FGOR     = 0;             //ǰ������ƫ��Ϊ0
    DMA2D->NLR      = tjpeg->yuvblk_height|(tjpeg->Conf.ImageWidth<<16);  //�趨�����Ĵ���
    DMA2D->OMAR     = destination;   //����洢����ַ
    DMA2D->FGMAR    = (u32)tjpeg->outbuf[tjpeg->outbuf_read_ptr].buf;  //Դ��ַ
    DMA2D->CR      |=1<<0;           //����DMA2D
    while((DMA2D->ISR&(1<<1)) == 0)  //�ȴ��������
    {
        timeout++;
        if(timeout > 0X1FFFFF)  //��ʱ�˳�
            break;
    } 
    tjpeg->yuvblk_curheight += tjpeg->yuvblk_height;  //ƫ�Ƶ���һ���ڴ��ַ
    //YUV2RGBת��������ڸ�λһ��DMA2D
    RCC->AHB3RSTR |= 1<<4;     //��λDMA2D
    RCC->AHB3RSTR &= ~(1<<4);  //������λ
    if(timeout > 0X1FFFFF)
        return 1;
    return 0;
}
