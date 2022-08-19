#include "main.h"

int main(void)
{
    stm32_cache_enable();                  //打开L1-Cache
    HAL_Init();                            //初始化HAL库
    stm32_clock_init(160, 5, 2, 4);        //设置时钟,400Mhz
    delay_init(400);                       //延时初始化
    uart_init(115200);                     //串口初始化
    led_init();                            //初始化LED
    key_init();                            //初始化按键
    SDRAM_Init();                          //初始化SDRAM
    LCD_Init();                            //初始化LCD
    W25QXX_Init();                         //初始化W25Q256
	usmart_dev.init(100);                  //初始化USMART
    ES8388_Init();                         //ES8388初始化
    ES8388_ADDA_Cfg(1, 0);                 //开启DAC关闭ADC
    ES8388_Output_Cfg(1, 1);               //DAC选择通道输出
    ES8388_HPvol_Set(25);                  //调节耳机音量
    ES8388_SPKvol_Set(15);                 //调节喇叭音量
    my_mem_init(SRAMIN);                   //初始化内部内存池(AXI)
    my_mem_init(SRAMEX);                   //初始化外部内存池(SDRAM)
    my_mem_init(SRAM12);                   //初始化SRAM12内存池(SRAM1+SRAM2)
    my_mem_init(SRAM4);                    //初始化SRAM4内存池(SRAM4)
    my_mem_init(SRAMDTCM);                 //初始化DTCM内存池(DTCM)
    my_mem_init(SRAMITCM);                 //初始化ITCM内存池(ITCM)
    exfuns_init();                         //为fatfs相关变量申请内存
    f_mount(fs[0], "0:", 1);               //挂载SD卡
    f_mount(fs[1], "1:", 1);               //挂载SPI FLASH.
    f_mount(fs[2], "2:", 1);               //挂载NAND FLASH.
    POINT_COLOR=RED;
    while(font_init())                     //检查字库
    {
        LCD_ShowString(30, 50, 200, 16, 16, "Font Error!");
        delay_ms(200);
        LCD_Fill(30, 50, 240, 66, WHITE);  //清除显示
        delay_ms(200);
    }
    POINT_COLOR=RED;
    LTDC_Display_Dir(1);          //横屏模式
    Show_Str(60,  50, 200, 16, "北极星H750/F750开发板",  16, 0);
    Show_Str(60,  70, 200, 16, "视频播放器实验",         16, 0);
    Show_Str(60,  90, 200, 16, "正点原子@ALIENTEK",      16, 0);
    Show_Str(60, 110, 200, 16, "2019年5月16日",          16, 0);
    Show_Str(60, 130, 200, 16, "KEY0:NEXT   KEY2:PREV", 16, 0);
    Show_Str(60, 150, 200, 16, "KEY_UP:FF   KEY1:REW",  16, 0);
    delay_ms(1500);

    TIM3_Init(10000-1, 20000-1);  //10Khz计数,1秒钟中断一次
    delay_ms(1500);

    while(1)
    {
        video_play();
    }
}
