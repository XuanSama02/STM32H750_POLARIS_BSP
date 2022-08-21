#include "main.h"

int main(void)
{
    stm32_cache_enable();             //��L1-Cache
    HAL_Init();                       //��ʼ��HAL��
    stm32_clock_init(160, 5, 2, 4);   //����ʱ��,400Mhz
    delay_init(400);                  //��ʱ��ʼ��
    uart_init(115200);                //���ڳ�ʼ��
    led_init();                       //��ʼ��LED
    key_init();                       //��ʼ������
    sdram_init();                     //��ʼ��SDRAM
    lcd_init();                       //��ʼ��LCD
    w25qxx_init();                    //��ʼ��W25Q256
    usmart_dev.init(100);             //��ʼ��USMART
    es8388_init();                    //ES8388��ʼ��
    es8388_adda_config(1, 0);         //����DAC�ر�ADC
    es8388_output_config(1, 1);       //DACѡ��ͨ�����
    es8388_headphone_vol_config(25);  //���ڶ�������
    es8388_speaker_vol_config(15);    //������������
    my_mem_init(SRAMIN);              //��ʼ���ڲ��ڴ��(AXI)
    my_mem_init(SRAMEX);              //��ʼ���ⲿ�ڴ��(SDRAM)
    my_mem_init(SRAM12);              //��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
    my_mem_init(SRAM4);               //��ʼ��SRAM4�ڴ��(SRAM4)
    my_mem_init(SRAMDTCM);            //��ʼ��DTCM�ڴ��(DTCM)
    my_mem_init(SRAMITCM);            //��ʼ��ITCM�ڴ��(ITCM)
    exfuns_init();                    //Ϊfatfs��ر��������ڴ�
    f_mount(fs[0], "0:", 1);          //����SD��
    f_mount(fs[1], "1:", 1);          //����SPI FLASH.
    f_mount(fs[2], "2:", 1);          //����NAND FLASH.

    lcd_color_point = RED;

    while(font_init())                     //����ֿ�
    {
        lcd_show_string(30, 50, 200, 16, 16, "Font Error!");
        delay_ms(200);
        lcd_fill(30, 50, 240, 66, WHITE);  //�����ʾ
        delay_ms(200);
    }

    lcd_color_point = RED;

    ltdc_display_direction(1);  //����ģʽ
    Show_Str(60,  50, 200, 16, "������H750/F750������",  16, 0);
    Show_Str(60,  70, 200, 16, "��Ƶ������ʵ��",         16, 0);
    Show_Str(60,  90, 200, 16, "����ԭ��@ALIENTEK",      16, 0);
    Show_Str(60, 110, 200, 16, "2019��5��16��",          16, 0);
    Show_Str(60, 130, 200, 16, "KEY0:NEXT   KEY2:PREV", 16, 0);
    Show_Str(60, 150, 200, 16, "KEY_UP:FF   KEY1:REW",  16, 0);
    delay_ms(1500);

    tim3_init(10000-1, 20000-1);  //10Khz����,1�����ж�һ��
    delay_ms(1500);

    while(1)
    {
        video_play();
    }
}
