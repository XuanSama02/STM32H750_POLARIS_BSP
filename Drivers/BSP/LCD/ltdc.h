#ifndef _LCD_H
#define _LCD_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/20
 * @brif:    北极星开发板LTDC驱动
 * @version:
 * 2022/08/20: 实现了北极星开发板LTDC驱动
 */

//LCD显示配置:

//定义颜色像素格式,一般用RGB565
#define LCD_PIXFORMAT        LCD_PIXEL_FORMAT_RGB565	
//定义默认背景层颜色
#define LTDC_BACKLAYERCOLOR  0X00000000	
//LCD帧缓冲区首地址,这里定义在SDRAM里面,默认大小2MB
#define LCD_FRAME_BUF_ADDR   0XC0000000  
//LCD背光控制(PD13)
#define LCD_LED(n)  (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET))

//LCD LTDC重要参数集
typedef struct
{
    u32 pwidth;      //LCD面板的宽度,不随显示方向改变,如果为0,说明没有RGB屏接入
    u32 pheight;     //LCD面板的高度,不随显示方向改变
    u16 hsw;         //水平同步宽度
    u16 vsw;         //垂直同步宽度
    u16 hbp;         //水平后廊
    u16 vbp;         //垂直后廊
    u16 hfp;         //水平前廊
    u16 vfp;         //垂直前廊
    u8 activelayer;  //当前层编号:0/1
    u8 dir;          //0:竖屏 1:横屏
    u16 width;       //LCD宽度
    u16 height;      //LCD高度
    u32 pixsize;     //每个像素所占字节数
}_ltdc_dev;

extern _ltdc_dev lcdltdc;                  //管理LCD LTDC参数
extern LTDC_HandleTypeDef LTDC_Handler;    //LTDC句柄
extern DMA2D_HandleTypeDef DMA2D_Handler;  //DMA2D句柄
extern u32 *ltdc_framebuf[2];              //LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域

#define LCD_PIXEL_FORMAT_ARGB8888  0X00  //显示格式:ARGB8888
#define LCD_PIXEL_FORMAT_RGB888    0X01  //显示格式:RGB888
#define LCD_PIXEL_FORMAT_RGB565    0X02  //显示格式:RGB565
#define LCD_PIXEL_FORMAT_ARGB1555  0X03  //显示格式:ARGB1555
#define LCD_PIXEL_FORMAT_ARGB4444  0X04  //显示格式:ARGB4444
#define LCD_PIXEL_FORMAT_L8        0X05  //显示格式:L8
#define LCD_PIXEL_FORMAT_AL44      0X06  //显示格式:AL44
#define LCD_PIXEL_FORMAT_AL88      0X07  //显示格式:AL88

void ltdc_init(void);
u16 ltdc_panel_id(void);
u8 ltdc_clk_config(u32 pll3m, u32 pll3n, u32 pll3r);
void ltdc_switch(u8 sw);
void ltdc_layer_switch(u8 layerx, u8 sw);
void ltdc_layer_select(u8 layerx);
void ltdc_layer_parameter_config(u8 layerx, u32 bufaddr, u8 pixformat, u8 alpha, u8 alpha0, u8 bfac1, u8 bfac2, u32 bkcolor);
void ltdc_layer_window_config(u8 layerx, u16 sx, u16 sy, u16 width, u16 height);
void ltdc_display_direction(u8 dir);

void ltdc_draw_point(u16 x, u16 y, u32 color);
u32 ltdc_read_point(u16 x, u16 y);
void ltdc_fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color);
void ltdc_color_fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color);
void ltdc_clear(u32 color);

#endif 
