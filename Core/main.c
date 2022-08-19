#include "main.h"

int main(void)
{
    stm32_cache_enable();                  //��L1-Cache
    HAL_Init();                            //��ʼ��HAL��
    stm32_clock_init(160, 5, 2, 4);        //����ʱ��,400Mhz
    delay_init(400);                       //��ʱ��ʼ��
    uart_init(115200);                     //���ڳ�ʼ��
    led_init();                            //��ʼ��LED
    key_init();                            //��ʼ������
    SDRAM_Init();                          //��ʼ��SDRAM
    LCD_Init();                            //��ʼ��LCD
    W25QXX_Init();                         //��ʼ��W25Q256
	usmart_dev.init(100);                  //��ʼ��USMART
    ES8388_Init();                         //ES8388��ʼ��
    ES8388_ADDA_Cfg(1, 0);                 //����DAC�ر�ADC
    ES8388_Output_Cfg(1, 1);               //DACѡ��ͨ�����
    ES8388_HPvol_Set(25);                  //���ڶ�������
    ES8388_SPKvol_Set(15);                 //������������
    my_mem_init(SRAMIN);                   //��ʼ���ڲ��ڴ��(AXI)
    my_mem_init(SRAMEX);                   //��ʼ���ⲿ�ڴ��(SDRAM)
    my_mem_init(SRAM12);                   //��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
    my_mem_init(SRAM4);                    //��ʼ��SRAM4�ڴ��(SRAM4)
    my_mem_init(SRAMDTCM);                 //��ʼ��DTCM�ڴ��(DTCM)
    my_mem_init(SRAMITCM);                 //��ʼ��ITCM�ڴ��(ITCM)
    exfuns_init();                         //Ϊfatfs��ر��������ڴ�
    f_mount(fs[0], "0:", 1);               //����SD��
    f_mount(fs[1], "1:", 1);               //����SPI FLASH.
    f_mount(fs[2], "2:", 1);               //����NAND FLASH.
    POINT_COLOR=RED;
    while(font_init())                     //����ֿ�
    {
        LCD_ShowString(30, 50, 200, 16, 16, "Font Error!");
        delay_ms(200);
        LCD_Fill(30, 50, 240, 66, WHITE);  //�����ʾ
        delay_ms(200);
    }
    POINT_COLOR=RED;
    LTDC_Display_Dir(1);          //����ģʽ
    Show_Str(60,  50, 200, 16, "������H750/F750������",  16, 0);
    Show_Str(60,  70, 200, 16, "��Ƶ������ʵ��",         16, 0);
    Show_Str(60,  90, 200, 16, "����ԭ��@ALIENTEK",      16, 0);
    Show_Str(60, 110, 200, 16, "2019��5��16��",          16, 0);
    Show_Str(60, 130, 200, 16, "KEY0:NEXT   KEY2:PREV", 16, 0);
    Show_Str(60, 150, 200, 16, "KEY_UP:FF   KEY1:REW",  16, 0);
    delay_ms(1500);

    TIM3_Init(10000-1, 20000-1);  //10Khz����,1�����ж�һ��
    delay_ms(1500);

    while(1)
    {
        video_play();
    }
}
