#include "ltdc.h"
#include "lcd.h"

LTDC_HandleTypeDef  LTDC_Handler;   //LTDC���
DMA2D_HandleTypeDef DMA2D_Handler;  //DMA2D���

//���ݲ�ͬ����ɫ��ʽ,����֡��������

#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
    u32 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));  //����������ֱ���ʱ,LCD�����֡���������С
#else
    u16 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));  //����������ֱ���ʱ,LCD�����֡���������С
#endif

u32 *ltdc_framebuf[2];  //LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ�����
_ltdc_dev lcdltdc;      //����LCD LTDC����Ҫ����

/**
 * @brief ��ʼ��LTDC
 * 
 */
void ltdc_init(void)
{   
    u16 lcd_id = 0;
    lcd_id = ltdc_panel_id();  //��ȡLCD���ID
    //���ݶ�ȡ����LCD����LTDC��������
    if(lcd_id == 0X4384)  //4.3��800*480 RGB��
    {
        lcdltdc.pwidth  = 800;  //�����,��λ:����
        lcdltdc.pheight = 480;  //���߶�,��λ:����
        lcdltdc.hbp     = 88;   //ˮƽ����
        lcdltdc.hfp     = 40;   //ˮƽǰ��
        lcdltdc.hsw     = 48;   //ˮƽͬ�����
        lcdltdc.vbp     = 32;   //��ֱ����
        lcdltdc.vfp     = 13;   //��ֱǰ��
        lcdltdc.vsw     = 3;    //��ֱͬ�����
        ltdc_clk_config(5, 160, 24);  //��������ʱ�� 33M
        //������������
    }
    #if(LCD_70_1024_600_ENABLE == 1)
    else if(lcd_id == 0X7016)  //7��1024*600 RGB��
    {
        lcdltdc.pwidth  = 1024;  //�����,��λ:����
        lcdltdc.pheight = 600;   //���߶�,��λ:����
        lcdltdc.hsw     = 20;    //ˮƽͬ�����
        lcdltdc.vsw     = 3;     //��ֱͬ�����
        lcdltdc.hbp     = 140;   //ˮƽ����
        lcdltdc.vbp     = 20;    //��ֱ����
        lcdltdc.hfp     = 160;   //ˮƽǰ��
        lcdltdc.vfp     = 12;    //��ֱǰ��
        ltdc_clk_config(5, 160, 18);  //��������ʱ�� 45Mhz
        //������������
    }
    #endif
    #if(LCD_43_480_272_ENABLE == 1)
    else if(lcd_id == 0X4342)  //4.3��480*272 RGB��
    {
        lcdltdc.pwidth  = 480;  //�����,��λ:����
        lcdltdc.pheight = 272;  //���߶�,��λ:����
        lcdltdc.hsw     = 1;    //ˮƽͬ�����
        lcdltdc.vsw     = 1;    //��ֱͬ�����
        lcdltdc.hbp     = 40;   //ˮƽ����
        lcdltdc.vbp     = 8;    //��ֱ����
        lcdltdc.hfp     = 5;    //ˮƽǰ��
        lcdltdc.vfp     = 8;    //��ֱǰ��
        ltdc_clk_config(5, 160, 88);  //��������ʱ��:9Mhz
        //������������
    }
    #endif
    #if(LCD_70_800_480_ENABLE == 1)
    else if(lcd_id == 0X7084)  //7��800*480 RGB��
    {
        lcdltdc.pwidth  = 800;  //�����,��λ:����
        lcdltdc.pheight = 480;  //���߶�,��λ:����
        lcdltdc.hsw     = 1;    //ˮƽͬ�����
        lcdltdc.vsw     = 1;    //��ֱͬ�����
        lcdltdc.hbp     = 46;   //ˮƽ����
        lcdltdc.vbp     = 23;   //��ֱ����
        lcdltdc.hfp     = 210;  //ˮƽǰ��
        lcdltdc.vfp     = 22;   //��ֱǰ��
        ltdc_clk_config(5, 160, 24);  //��������ʱ�� 33M(�����˫��,��Ҫ����DCLK��:18.75Mhz, pll3r=43,�ȽϺ�)
    }
    #endif
    #if(LCD_101_1280_800_ENABLE == 1)
    else if(lcd_id == 0X1018)  //10.1��1280*800 RGB��
    {
        lcdltdc.pwidth  = 1280;  //�����,��λ:����
        lcdltdc.pheight = 800;   //���߶�,��λ:����
        lcdltdc.hbp     = 140;   //ˮƽ����
        lcdltdc.hfp     = 10;    //ˮƽǰ��
        lcdltdc.hsw     = 10;    //ˮƽͬ�����
        lcdltdc.vbp     = 10;    //��ֱ����
        lcdltdc.vfp     = 10;    //��ֱǰ��
        lcdltdc.vsw     = 3;     //��ֱͬ�����
        ltdc_clk_config(5, 160, 16);  //��������ʱ�� 50MHz
    }
    #endif
    #if(LCD_VGA_ENBALE == 1)
    else if(lcd_id == 0XA001)  //��VGA��ʾ��: 1366*768
    {
        lcdltdc.pwidth  = 1366;  //���� ��,��λ:����
        lcdltdc.pheight = 768;   //���߶�,��λ:����
        lcdltdc.hsw     = 143;   //ˮƽͬ����� 
        lcdltdc.hfp     = 70;    //ˮƽǰ��
        lcdltdc.hbp     = 213;   //ˮƽ����
        lcdltdc.vsw     = 3;     //��ֱͬ�����
        lcdltdc.vbp     = 24;    //��ֱ����
        lcdltdc.vfp     = 3;     //��ֱǰ��
        ltdc_clk_config(5, 160, 28);  //��������ʱ�� 28.5Mhz
    }
    else if(lcd_id == 0XA002)  //��VGA��ʾ��: 1280*800
    {
        lcdltdc.pwidth  = 1280;  //�����,��λ:����
        lcdltdc.pheight = 800;   //���߶�,��λ:����
        lcdltdc.hsw     = 32;    //ˮƽͬ�����
        lcdltdc.hfp     = 48;    //ˮƽǰ��
        lcdltdc.hbp     = 80;    //ˮƽ����
        lcdltdc.vsw     = 6;     //��ֱͬ�����
        lcdltdc.vbp     = 14;    //��ֱ����
        lcdltdc.vfp     = 3;     //��ֱǰ��
        ltdc_clk_config(5, 160, 22);  //��������ʱ�� 35.5Mhz
    }
    else if(lcd_id == 0XA003)  //��VGA��ʾ��: 1280*768
    {
        lcdltdc.pwidth  = 1280;  //�����,��λ:����
        lcdltdc.pheight = 768;   //���߶�,��λ:����
        lcdltdc.hsw     = 32;    //ˮƽͬ����� 
        lcdltdc.hbp     = 80;    //ˮƽ����
        lcdltdc.hfp     = 48;    //ˮƽǰ��
        lcdltdc.vsw     = 7;     //��ֱͬ�����
        lcdltdc.vbp     = 12;    //��ֱ����
        lcdltdc.vfp     = 3;     //��ֱǰ��
        ltdc_clk_config(5, 160, 23);  //��������ʱ��  34.25Mhz
    }
    else if(lcd_id == 0XA004)  //��VGA��ʾ��: 1024*768
    {
        lcdltdc.pwidth  = 1024;  //�����,��λ:����
        lcdltdc.pheight = 768;   //���߶�,��λ:����
        lcdltdc.hsw     = 136;   //ˮƽͬ����� 
        lcdltdc.hfp     = 24;    //ˮƽǰ��
        lcdltdc.hbp     = 160;   //ˮƽ����
        lcdltdc.vsw     = 6;     //��ֱͬ�����
        lcdltdc.vbp     = 29;    //��ֱ����
        lcdltdc.vfp     = 3;     //��ֱǰ��
        ltdc_clk_config(5, 160, 18);  //��������ʱ�� 43.25Mhz
    }
    else if(lcd_id == 0XA005)  //��VGA��ʾ��: 848*480
    {
        lcdltdc.pwidth  = 848;  //�����,��λ:����
        lcdltdc.pheight = 480;  //���߶�,��λ:����
        lcdltdc.hsw     = 112;  //ˮƽͬ�����
        lcdltdc.hbp     = 112;  //ˮƽ����
        lcdltdc.hfp     = 16;   //ˮƽǰ��
        lcdltdc.vsw     = 8;    //��ֱͬ�����
        lcdltdc.vbp     = 23;   //��ֱ����
        lcdltdc.vfp     = 6;    //��ֱǰ��
        ltdc_clk_config(5, 160, 47);//��������ʱ�� 17Mhz
    }
    else if(lcd_id == 0XA006)  //��VGA��ʾ��: 800*600
    {
        lcdltdc.pwidth  = 800;  //�����,��λ:����
        lcdltdc.pheight = 600;  //���߶�,��λ:����
        lcdltdc.hsw     = 128;  //ˮƽͬ�����
        lcdltdc.hbp     = 88;   //ˮƽ����
        lcdltdc.hfp     = 40;   //ˮƽǰ��
        lcdltdc.vsw     = 4;    //��ֱͬ�����
        lcdltdc.vbp     = 23;   //��ֱ����
        lcdltdc.vfp     = 1;    //��ֱǰ��
        ltdc_clk_config(5, 160, 40);  //��������ʱ�� 20mz
    }
    else if(lcd_id == 0XA007)  //��VGA��ʾ��: 640*480
    {
        lcdltdc.pwidth  = 640;  //�����,��λ:����
        lcdltdc.pheight = 480;  //���߶�,��λ:����
        lcdltdc.hsw     = 96;   //ˮƽͬ�����
        lcdltdc.hfp     = 8;    //ˮƽǰ��
        lcdltdc.hbp     = 56;   //ˮƽ����
        lcdltdc.vsw     = 2;    //��ֱͬ�����
        lcdltdc.vbp     = 41;   //��ֱ����
        lcdltdc.vfp     = 2;    //��ֱǰ��
        ltdc_clk_config(5, 160, 62);  //��������ʱ�� 12.75Mhz
    }
    #endif
    //ȷ����ʾ�ĳ�������
    lcddev.width  = lcdltdc.pwidth;
    lcddev.height = lcdltdc.pheight;

#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
    ltdc_framebuf[0] = (u32*)&ltdc_lcd_framebuf;
    lcdltdc.pixsize  = 4;  //ÿ������ռ4���ֽ�
#else
    ltdc_framebuf[0] = (u32*)&ltdc_lcd_framebuf;
    lcdltdc.pixsize  = 2;  //ÿ������ռ2���ֽ�
#endif

    //LTDC����
    LTDC_Handler.Instance = LTDC;
    LTDC_Handler.Init.HSPolarity = LTDC_HSPOLARITY_AL;   //ˮƽͬ������
    LTDC_Handler.Init.VSPolarity = LTDC_VSPOLARITY_AL;   //��ֱͬ������
    LTDC_Handler.Init.DEPolarity = LTDC_DEPOLARITY_AL;   //����ʹ�ܼ���
    LTDC_Handler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;  //����ʱ�Ӽ���
    if(lcd_id == 0X1018)  //10.1��RGB��,����ʱ�Ӽ����෴
        LTDC_Handler.Init.PCPolarity = LTDC_PCPOLARITY_IIPC;
    LTDC_Handler.Init.HorizontalSync     = lcdltdc.hsw -1;  //ˮƽͬ�����
    LTDC_Handler.Init.VerticalSync       = lcdltdc.vsw -1;  //��ֱͬ�����
    LTDC_Handler.Init.AccumulatedHBP     = lcdltdc.hsw + lcdltdc.hbp -1;  //ˮƽͬ�����ؿ��
    LTDC_Handler.Init.AccumulatedVBP     = lcdltdc.vsw + lcdltdc.vbp -1;  //��ֱͬ�����ظ߶�
    LTDC_Handler.Init.AccumulatedActiveW = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth  -1;  //��Ч���
    LTDC_Handler.Init.AccumulatedActiveH = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight -1;  //��Ч�߶�
    LTDC_Handler.Init.TotalWidth         = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth  + lcdltdc.hfp -1;  //�ܿ��
    LTDC_Handler.Init.TotalHeigh         = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight + lcdltdc.vfp -1;  //�ܸ߶�
    LTDC_Handler.Init.Backcolor.Red      = 0;  //��Ļ�������ɫ����
    LTDC_Handler.Init.Backcolor.Green    = 0;  //��Ļ��������ɫ����
    LTDC_Handler.Init.Backcolor.Blue     = 0;  //��Ļ����ɫ��ɫ����
    HAL_LTDC_Init(&LTDC_Handler);
    //layer����
    ltdc_layer_parameter_config(0, (u32)ltdc_framebuf[0], LCD_PIXFORMAT, 255, 0, 6, 7, 0X000000);  //���������
    ltdc_layer_window_config(0, 0, 0, lcdltdc.pwidth, lcdltdc.pheight);  //�㴰������,��LCD�������ϵΪ��׼
    //LTDC��ʼ����
    ltdc_display_direction(0);  //Ĭ����������LCD_Init������������
    ltdc_layer_select(0);       //ѡ���1��
    LCD_LED(1);                 //��������
    ltdc_clear(0XFFFFFFFF);     //����
}

/**
 * @brief LTDC�ײ�����,��ʼ��IO,ʱ��ʹ��
 * 
 * @param hltdc LTDC���
 */
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
    GPIO_InitTypeDef ymx_gpio_init;
    //ʹ��ʱ��
    __HAL_RCC_LTDC_CLK_ENABLE();   //ʹ��LTDCʱ��
    __HAL_RCC_DMA2D_CLK_ENABLE();  //ʹ��DMA2Dʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();  //ʹ��GPIOBʱ��
    __HAL_RCC_GPIOI_CLK_ENABLE();  //ʹ��GPIOIʱ��
    __HAL_RCC_GPIOJ_CLK_ENABLE();  //ʹ��GPIOJʱ��
    __HAL_RCC_GPIOK_CLK_ENABLE();  //ʹ��GPIOKʱ��
    //��ʼ��PB5����������
    ymx_gpio_init.Pin   = GPIO_PIN_5;                 //PB5��������
    ymx_gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;        //�������
    ymx_gpio_init.Pull  = GPIO_PULLUP;                //����
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(GPIOB, &ymx_gpio_init);
    //��ʼ��PI12,13,14
    ymx_gpio_init.Pin       = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
    ymx_gpio_init.Mode      = GPIO_MODE_AF_PP;            //��������
    ymx_gpio_init.Pull      = GPIO_NOPULL;                //����������
    ymx_gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    ymx_gpio_init.Alternate = GPIO_AF14_LTDC;             //����ΪLTDC
    HAL_GPIO_Init(GPIOI, &ymx_gpio_init);
    //��ʼ��PJ2~6,9~11,15
    ymx_gpio_init.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|\
                      GPIO_PIN_6|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                      GPIO_PIN_15;
    HAL_GPIO_Init(GPIOJ, &ymx_gpio_init);
    //��ʼ��PK0~7
    ymx_gpio_init.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|\
                      GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    HAL_GPIO_Init(GPIOK, &ymx_gpio_init);
}

/**
 * @brief ��ȡLCD������
 *        PJ6=R7(M0) PK2=G7(M1) PK6=B7(M2)
 *        M2:M1:M0
 *        0 :0 :0  //4.3��480*272 RGB��,ID=0X4342
 *        0 :0 :1  //7��800*480 RGB��,ID=0X7084
 *        0 :1 :0  //7��1024*600 RGB��,ID=0X7016
 *        0 :1 :1  //7��1280*800 RGB��,ID=0X7018
 *        1 :0 :0  //4.3��800*480 RGB��,ID=0X4384
 *        1 :0 :1  //10.1��1280*800 RGB��,ID=0X1018
 * 
 * @return u16 LCD��ID, 0:�Ƿ�
 */
u16 ltdc_panel_id(void)
{
    u8 lcd_id = 0;
    GPIO_InitTypeDef ymx_gpio_init;
    //ʹ��ʱ��
    __HAL_RCC_GPIOJ_CLK_ENABLE();  //ʹ��GPIOJʱ��
    __HAL_RCC_GPIOK_CLK_ENABLE();  //ʹ��GPIOKʱ��
    //����GPIO
    ymx_gpio_init.Pin   = GPIO_PIN_6;                 //PJ6
    ymx_gpio_init.Mode  = GPIO_MODE_INPUT;            //����
    ymx_gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //����
    HAL_GPIO_Init(GPIOJ, &ymx_gpio_init);

    ymx_gpio_init.Pin   = GPIO_PIN_2|GPIO_PIN_6;  //PK2,6
    HAL_GPIO_Init(GPIOK, &ymx_gpio_init);
    //��ȡLCD��ID
    lcd_id  = (u8)HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_6);     //��ȡM0
    lcd_id |= (u8)HAL_GPIO_ReadPin(GPIOK, GPIO_PIN_2)<<1;  //��ȡM1
    lcd_id |= (u8)HAL_GPIO_ReadPin(GPIOK, GPIO_PIN_6)<<2;  //��ȡM2
    if(lcd_id == 0)       //4.3����,480*272�ֱ���
        return 0X4342;
    else if(lcd_id == 1)  //7����,800*480�ֱ���
        return 0X7084;
    else if(lcd_id == 2)  //7����,1024*600�ֱ���
        return 0X7016;
    else if(lcd_id == 3)  //7����,1280*800�ֱ���(�Ѿ���)
        return 0X7018;
    else if(lcd_id == 4)  //4.3����,800*480�ֱ���
        return 0X4384;
    else if(lcd_id == 5)  //10.1����,1280*800�ֱ���
        return 0X1018;
    else
        return 0;
}

/**
 * @brief LTDCʱ������
 *        PLL3_VCO_in  = HSE/PLL3M
 *        PLL3_VCO_out = PLL3_VCO_in*PLL3N
 *        PLL_LCD_CLK  = PLL3_VCO_out/PLL3R
 * 
 * @param pll3m PLL3M(��Ƶ)
 * @param pll3n PLL3N(��Ƶ)
 * @param pll3r PLL3R(��Ƶ)
 * @return u8 1:ʧ�� 0:�ɹ�
 */
u8 ltdc_clk_config(u32 pll3m, u32 pll3n, u32 pll3r)
{
    RCC_PeriphCLKInitTypeDef ymx_periph_clk_init;
    ymx_periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    ymx_periph_clk_init.PLL3.PLL3M = pll3m;
    ymx_periph_clk_init.PLL3.PLL3N = pll3n;
    ymx_periph_clk_init.PLL3.PLL3P = 2;
    ymx_periph_clk_init.PLL3.PLL3Q = 2;
    ymx_periph_clk_init.PLL3.PLL3R = pll3r;
    if(HAL_RCCEx_PeriphCLKConfig(&ymx_periph_clk_init) == HAL_OK)  //��������ʱ�ӣ���������Ϊʱ��Ϊ18.75MHZ
        return 0;  //�ɹ�
    else
        return 1;  //ʧ��
}

/**
 * @brief LTDCʹ�ܿ���
 * 
 * @param sw 1:���� 0:�ر�
 */
void ltdc_switch(u8 sw)
{
    if(sw == 1)
        __HAL_LTDC_ENABLE(&LTDC_Handler);
    else if(sw == 0)
        __HAL_LTDC_DISABLE(&LTDC_Handler);
}

/**
 * @brief LTDCָ��layer����
 * 
 * @param layerx layer���,Ĭ��0
 * @param sw     1:�� 0:�ر�
 */
void ltdc_layer_switch(u8 layerx, u8 sw)
{
    if(sw == 1)
        __HAL_LTDC_LAYER_ENABLE(&LTDC_Handler, layerx);
    else if(sw == 0)
        __HAL_LTDC_LAYER_DISABLE(&LTDC_Handler, layerx);
    __HAL_LTDC_RELOAD_CONFIG(&LTDC_Handler);
}

/**
 * @brief ѡ��layer
 * 
 * @param layerx ����,Ĭ��0
 */
void ltdc_layer_select(u8 layerx)
{
    lcdltdc.activelayer = layerx;
}

/**
 * @brief LTDC������������,�˺���������ltdc_layer_window_config֮ǰ����
 * 
 * @param layerx    ����,Ĭ��0
 * @param bufaddr   ����ɫ֡������ʼ��ַ
 * @param pixformat ��ɫ��ʽ 0:ARGB8888 1:RGB888 2:RGB565 3:ARGB1555 4:ARGB4444 5:L8 6:AL44 7:AL88
 * @param alpha     ����ɫAlphaֵ 0:ȫ͸�� 255:��͸��
 * @param alpha0    Ĭ����ɫAlphaֵ 0:ȫ͸�� 255:��͸��
 * @param bfac1     ���ϵ��1 4(100),�㶨��Alpha 6(101),����Alpha*�㶨Alpha
 * @param bfac2     ���ϵ��2 5(101),�㶨��Alpha 7(111),����Alpha*�㶨Alpha
 * @param bkcolor   ��Ĭ����ɫ 32λ,��24λ��Ч,RGB888��ʽ
 */
void ltdc_layer_parameter_config(u8 layerx, u32 bufaddr, u8 pixformat, u8 alpha, u8 alpha0, u8 bfac1, u8 bfac2, u32 bkcolor)
{
    LTDC_LayerCfgTypeDef ymx_layer_config;
    //����layer
    ymx_layer_config.WindowX0        = 0;                             //������ʼX����
    ymx_layer_config.WindowY0        = 0;                             //������ʼY����
    ymx_layer_config.WindowX1        = lcdltdc.pwidth;                //������ֹX����
    ymx_layer_config.WindowY1        = lcdltdc.pheight;               //������ֹY����
    ymx_layer_config.PixelFormat     = pixformat;                     //���ظ�ʽ
    ymx_layer_config.Alpha           = alpha;                         //Alphaֵ���ã�0~255,255Ϊ��ȫ��͸��
    ymx_layer_config.Alpha0          = alpha0;                        //Ĭ��Alphaֵ
    ymx_layer_config.BlendingFactor1 = (u32)bfac1<<8;                 //���ò���ϵ��
    ymx_layer_config.BlendingFactor2 = (u32)bfac2<<8;                 //���ò���ϵ��
    ymx_layer_config.FBStartAdress   = bufaddr;                       //���ò���ɫ֡������ʼ��ַ
    ymx_layer_config.ImageWidth      = lcdltdc.pwidth;                //������ɫ֡�������Ŀ��
    ymx_layer_config.ImageHeight     = lcdltdc.pheight;               //������ɫ֡�������ĸ߶�
    ymx_layer_config.Backcolor.Red   = (u8)(bkcolor&0X00FF0000)>>16;  //������ɫ��ɫ����
    ymx_layer_config.Backcolor.Green = (u8)(bkcolor&0X0000FF00)>>8;   //������ɫ��ɫ����
    ymx_layer_config.Backcolor.Blue  = (u8)bkcolor&0X000000FF;        //������ɫ��ɫ����
    HAL_LTDC_ConfigLayer(&LTDC_Handler, &ymx_layer_config,layerx);    //������ѡ�еĲ�
}

/**
 * @brief LTDC�㴰������,��LCD�������ϵΪ��׼,�˺���������ltdc_layer_parameter_config֮��������
 * 
 * @param layerx ����,Ĭ��0
 * @param sx     ��ʼˮƽ����
 * @param sy     ��ʼ��ֱ����
 * @param width  ��ʾ���
 * @param height ��ʾ�߶�
 */
void ltdc_layer_window_config(u8 layerx, u16 sx, u16 sy, u16 width, u16 height)
{
    HAL_LTDC_SetWindowPosition(&LTDC_Handler, sx, sy, layerx);     //���ô��ڵ�λ��
    HAL_LTDC_SetWindowSize(&LTDC_Handler, width, height, layerx);  //���ô��ڴ�С
}

/**
 * @brief ����LTDC��ʾ����
 * 
 * @param dir 0:���� 1:����
 */
void ltdc_display_direction(u8 dir)
{
    lcdltdc.dir = dir;  //��ʾ����
    if(dir == 0)        //����
    {
        lcdltdc.width  = lcdltdc.pheight;
        lcdltdc.height = lcdltdc.pwidth;
    }else if(dir == 1)  //����
    {
        lcdltdc.width  = lcdltdc.pwidth;
        lcdltdc.height = lcdltdc.pheight;
    }
}

/**
 * @brief ���㺯��
 * 
 * @param x     ˮƽ����
 * @param y     ��ֱ����
 * @param color ��ɫֵ
 */
void ltdc_draw_point(u16 x, u16 y, u32 color)
{ 
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
    if(lcdltdc.dir)  //����
        *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x)) = color;
    else             //����
        *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+y)) = color;
#else
    if(lcdltdc.dir)  //����
        *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x)) = color;
    else             //����
        *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+y)) = color; 
#endif
}

/**
 * @brief ���㺯��
 * 
 * @param x ˮƽ����
 * @param y ��ֱ����
 * @return u32 ��ɫֵ
 */
u32 ltdc_read_point(u16 x, u16 y)
{ 
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
    if(lcdltdc.dir)  //����
        return *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x));
    else             //����
        return *(u32*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+y));
#else
    if(lcdltdc.dir)  //����
        return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*y+x));
    else             //����
        return *(u16*)((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*(lcdltdc.pheight-x-1)+y));
#endif 
}

/**
 * @brief LTDC������,ʹ��DMA2Dʵ��,(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
 *        ע��: sx��ex <= lcddev.width-1 sy��ey <= lcddev.height-1
 * 
 * @param sx    ���ˮƽ����
 * @param sy    ��㴹ֱ����
 * @param ex    �յ�ˮƽ����
 * @param ey    �յ㴹ֱ����
 * @param color ��ɫֵ
 */
void ltdc_fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color)
{
    //��LCD���Ϊ��׼������ϵ,����������仯���仯
    u32 psx;
    u32 psy;
    u32 pex;
    u32 pey;
    u32 timeout = 0;
    u32 addr;
    u16 offline;
    //����ϵת��
    if(lcdltdc.dir)  //����
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }else            //����
    {
        psx = sy;
        psy = lcdltdc.pheight-ex-1;
        pex = ey;
        pey = lcdltdc.pheight-sx-1;
    }
    offline = lcdltdc.pwidth-(pex-psx+1);
    addr    = ((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
    //ʹ��DMA2D������
    RCC->AHB1ENR |= 1<<23;                          //ʹ��DM2Dʱ��
    DMA2D->CR     = 3<<16;                          //�Ĵ������洢��ģʽ
    DMA2D->OPFCCR = LCD_PIXFORMAT;                  //������ɫ��ʽ
    DMA2D->OOR    = offline;                        //������ƫ��
    DMA2D->CR    &= ~(1<<0);                        //��ֹͣDMA2D
    DMA2D->OMAR   = addr;                           //����洢����ַ
    DMA2D->NLR    = (pey-psy+1)|((pex-psx+1)<<16);  //�趨�����Ĵ���
    DMA2D->OCOLR  = color;                          //�趨�����ɫ�Ĵ���
    DMA2D->CR    |= 1<<0;                           //����DMA2D
    while((DMA2D->ISR&(1<<1)) == 0)  //�ȴ��������
    {
        timeout++;
        if(timeout > 0X1FFFFF)       //��ʱ�˳�
            break;
    }  
    DMA2D->IFCR |= 1<<1;  //���������ɱ�־ 	
}

/**
 * @brief ��ָ�����������ָ����ɫ��,DMA2D���,�˺�����֧��u16,RGB565��ʽ����ɫ�������
 *        (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
 *        ע��: sx��ex <= lcddev.width-1 sy��ey <= lcddev.height-1
 * 
 * @param sx  ���ˮƽ����
 * @param sy  ��㴹ֱ����
 * @param ex  �յ�ˮƽ����
 * @param ey  �յ㴹ֱ����
 * @param color ��ɫֵ
 */
void ltdc_color_fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    //��LCD���Ϊ��׼������ϵ,����������仯���仯
    u32 psx;
    u32 psy;
    u32 pex;
    u32 pey;
    u32 timeout = 0;
    u32 addr;
    u16 offline;
    //����ϵת��
    if(lcdltdc.dir)  //����
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }else            //����
    {
        psx = sy;
        psy = lcdltdc.pheight-ex-1;
        pex = ey;
        pey = lcdltdc.pheight-sx-1;
    }
    offline = lcdltdc.pwidth-(pex-psx+1);
    addr    = ((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
    //ʹ��DMA2D������
    RCC->AHB1ENR  |= 1<<23;                          //ʹ��DM2Dʱ��
    DMA2D->CR      = 0<<16;                          //�洢�����洢��ģʽ
    DMA2D->FGPFCCR = LCD_PIXFORMAT;                  //������ɫ��ʽ
    DMA2D->FGOR    = 0;                              //ǰ������ƫ��Ϊ0
    DMA2D->OOR     = offline;                        //������ƫ��
    DMA2D->CR     &= ~(1<<0);                        //��ֹͣDMA2D
    DMA2D->FGMAR   = (u32)color;                     //Դ��ַ
    DMA2D->OMAR    = addr;                           //����洢����ַ
    DMA2D->NLR     = (pey-psy+1)|((pex-psx+1)<<16);  //�趨�����Ĵ���
    DMA2D->CR     |= 1<<0;                           //����DMA2D
    while((DMA2D->ISR&(1<<1)) == 0)  //�ȴ��������
    {
        timeout++;
        if(timeout > 0X1FFFFF)       //��ʱ�˳�
            break;
    }
    DMA2D->IFCR |= 1<<1;  //���������ɱ�־
} 

/**
 * @brief LCD����
 * 
 * @param color ��ɫֵ
 */
void ltdc_clear(u32 color)
{
    ltdc_fill(0, 0, lcdltdc.width-1, lcdltdc.height-1, color);
}
