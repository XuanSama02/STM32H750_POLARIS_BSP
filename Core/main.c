#include "main.h"

int main(void)
{
    //ִ�г�ʼ������
    stm32_cache_enable();             //��L1-Cache
    HAL_Init();                       //��ʼ��HAL��
    stm32_clock_init(160, 5, 2, 4);   //����ʱ��,400Mhz
    delay_init(400);                  //��ʱ��ʼ��
    uart1_init(115200);               //���ڳ�ʼ��
    led_init();                       //��ʼ��LED
    key_init();                       //��ʼ������
    sdram_init();                     //��ʼ��SDRAM
    lcd_init();                       //��ʼ��LCD
    w25qxx_init();                    //��ʼ��W25Q256
    usmart_dev.init(100);             //��ʼ��USMART
    es8388_init();                    //ES8388��ʼ��
    es8388_adda_config(1, 0);         //����DAC�ر�ADC
    es8388_output_config(1, 1);       //DACѡ��ͨ�����
    es8388_headphone_vol_config(10);  //���ڶ�������
    es8388_speaker_vol_config(4);     //������������
    my_mem_init_all();                //��ʼ��Ƭ���ڴ�������ڴ�
    exfuns_init();                    //Ϊfatfs��ر��������ڴ�
    printf("Init Sequence Success!\r\n");

    //ִ�д洢��������
    f_mount(fs[0], "0:", 1);          //����SD��
    f_mount(fs[1], "1:", 1);          //����SPI FLASH
    printf("Mount Sequence Success!\r\n");

    //����LCD������ɫ: ��ɫ
    lcd_color_point = RED;

    //����ֿ�
    while(font_init())  //���ʧ�ܻ���˸10����ʾ
    {
        lcd_show_string(30, 50, 200, 16, 16, "Font Error!");
        delay_ms(200);
        lcd_fill(30, 50, 240, 66, WHITE);  //�����ʾ
        delay_ms(200);
    }

    //��������LTDCΪ����ģʽ,��ʱLCD��ȻΪ����ģʽ
    ltdc_display_direction(1);  //LTDC����ģʽ

    //��ӡ��Ϣ
    show_string(60,  50, 200, 16, "������H750/F750������",  16, 0);
    show_string(60,  70, 200, 16, "��Ƶ������ʵ��",         16, 0);
    show_string(60,  90, 200, 16, "����ԭ��@ALIENTEK",      16, 0);
    show_string(60, 110, 200, 16, "2019��5��16��",          16, 0);
    show_string(60, 130, 200, 16, "KEY0:NEXT   KEY2:PREV", 16, 0);
    show_string(60, 150, 200, 16, "KEY_UP:FF   KEY1:REW",  16, 0);
    delay_ms(1500);

    //��ʼ��TIM3���ڴ�ӡ��Ƶ֡��
    tim3_init(10000-1, 20000-1);  //10Khz����,1�����ж�һ��
    delay_ms(1500);

    //������Ƶ
    while(1)
    {
        video_play();
    }
}
