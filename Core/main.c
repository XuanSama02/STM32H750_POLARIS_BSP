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
    es8388_init();                    //ES8388��ʼ��
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

    //LDC����ģʽ
    lcd_display_direction(1);

    //��ʾCDTUУ��
    lcd_color_point = BLUE;
    lcd_draw_image(40, 90, 300, 297, Image_CDTU_300x297);

    //��ʾչʾ��Ϣ
    lcd_color_point = BLACK;
    show_string(400, 180, 400, 32, "���˻�ȫϢͶӰչʾϵͳ", 32, 0);
    show_string(478, 230, 400, 24, "��������������Ϣ����1��", 24, 0);
    show_string(680, 260, 400, 24, "������", 24, 0);
    delay_ms(1500);
    while(key_scan(0) != WKUP_PRES)
    {
        lcd_color_point = RED;
        show_string(315, 400, 300, 24, "����WKUP����ʼͶӰ", 24, 0);
        delay_ms(500);
        lcd_color_point = WHITE;
        show_string(315, 400, 300, 24, "����WKUP����ʼͶӰ", 24, 0);
        delay_ms(500);
    }

    //��ʼ��TIM3���ڴ�ӡ��Ƶ֡��
    tim3_init(10000-1, 20000-1);  //10Khz����,1�����ж�һ��
    delay_ms(500);

    //������Ƶ
    while(1)
    {
        video_play();
    }
}
