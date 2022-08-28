#include "main.h"

int main(void)
{
    //执行初始化序列
    stm32_cache_enable();             //打开L1-Cache
    HAL_Init();                       //初始化HAL库
    stm32_clock_init(160, 5, 2, 4);   //设置时钟,400Mhz
    delay_init(400);                  //延时初始化
    uart1_init(115200);               //串口初始化
    led_init();                       //初始化LED
    key_init();                       //初始化按键
    sdram_init();                     //初始化SDRAM
    lcd_init();                       //初始化LCD
    w25qxx_init();                    //初始化W25Q256
    usmart_dev.init(100);             //初始化USMART
    es8388_init();                    //ES8388初始化
    es8388_adda_config(1, 0);         //开启DAC关闭ADC
    es8388_output_config(1, 1);       //DAC选择通道输出
    es8388_headphone_vol_config(10);  //调节耳机音量
    es8388_speaker_vol_config(4);     //调节喇叭音量
    my_mem_init_all();                //初始化片上内存与板载内存
    exfuns_init();                    //为fatfs相关变量申请内存
    printf("Init Sequence Success!\r\n");

    //执行存储挂载序列
    f_mount(fs[0], "0:", 1);          //挂载SD卡
    f_mount(fs[1], "1:", 1);          //挂载SPI FLASH
    printf("Mount Sequence Success!\r\n");

    //设置LCD画笔颜色: 红色
    lcd_color_point = RED;

    //检查字库
    while(font_init())  //检查失败会闪烁10次提示
    {
        lcd_show_string(30, 50, 200, 16, 16, "Font Error!");
        delay_ms(200);
        lcd_fill(30, 50, 240, 66, WHITE);  //清除显示
        delay_ms(200);
    }

    //这里设置LTDC为横屏模式,此时LCD仍然为竖屏模式
    ltdc_display_direction(1);  //LTDC横屏模式

    //打印信息
    show_string(60,  50, 200, 16, "北极星H750/F750开发板",  16, 0);
    show_string(60,  70, 200, 16, "视频播放器实验",         16, 0);
    show_string(60,  90, 200, 16, "正点原子@ALIENTEK",      16, 0);
    show_string(60, 110, 200, 16, "2019年5月16日",          16, 0);
    show_string(60, 130, 200, 16, "KEY0:NEXT   KEY2:PREV", 16, 0);
    show_string(60, 150, 200, 16, "KEY_UP:FF   KEY1:REW",  16, 0);
    delay_ms(1500);

    //初始化TIM3用于打印视频帧率
    tim3_init(10000-1, 20000-1);  //10Khz计数,1秒钟中断一次
    delay_ms(1500);

    //播放视频
    while(1)
    {
        video_play();
    }
}
