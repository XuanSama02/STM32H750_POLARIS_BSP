#ifndef __LCD_H
#define __LCD_H

#include "sys.h"
#include "stdlib.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板LCD驱动
 * @version:
 * 1.0：实现了北极星开发板LCD驱动,仅支持RGB屏,去除了对MCU屏的支持
 */

//LCD重要参数集
typedef struct
{
    u16 width;    //LCD 宽度
    u16 height;   //LCD 高度
    u16 id;       //LCD ID
    u8  dir;      //显示方向 0:竖屏 1:横屏
    u16	wramcmd;  //开始写gram指令
    u16 setxcmd;  //设置x坐标指令
    u16 setycmd;  //设置y坐标指令 
}_lcd_dev;

extern _lcd_dev lcddev;  //管理LCD重要参数

extern u32  lcd_color_point;  //画笔颜色,默认红色
extern u32  lcd_color_back;   //背景颜色,默认白色

//画笔颜色

#define WHITE    0xFFFF
#define BLACK    0x0000
#define BLUE     0x001F
#define BRED     0XF81F
#define GRED     0XFFE0
#define GBLUE    0X07FF
#define RED      0xF800
#define MAGENTA  0xF81F
#define GREEN    0x07E0
#define CYAN     0x7FFF
#define YELLOW   0xFFE0
#define BROWN    0XBC40 //棕色
#define BRRED    0XFC07 //棕红色
#define GRAY     0X8430 //灰色

//GUI颜色(PANEL的颜色)

#define DARKBLUE   0X01CF  //深蓝色
#define LIGHTBLUE  0X7D7C  //浅蓝色  
#define GRAYBLUE   0X5458  //灰蓝色
 
#define LIGHTGREEN  0X841F //浅绿色
#define LIGHTGRAY   0XEF5B //浅灰色(PANNEL)
#define LGRAY       0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE   0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE      0X2B12 //浅棕蓝色(选择条目的反色)

void lcd_init(void);                 //初始化LCD
void lcd_display_on(void);           //开启显示
void lcd_display_off(void);          //关闭显示
void lcd_display_direction(u8 dir);  //显示方向设置

void lcd_clear(u32 color);          //清屏
u32  lcd_read_point(u16 x, u16 y);  //读点
void lcd_draw_point(u16 x, u16 y);                        //画点
void lcd_draw_point_fast(u16 x, u16 y, u32 color);        //画点(快速)
void lcd_draw_circle(u16 x0, u16 y0, u8 r);               //画圆
void lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2);       //画线
void lcd_draw_rectangle(u16 x1, u16 y1, u16 x2, u16 y2);  //画矩形
void lcd_fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color);         //填充单色
void lcd_color_fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color);  //填充指定颜色

void lcd_show_num(u16 x, u16 y, u32 num, u8 len, u8 size);                  //显示单个数字
void lcd_show_xnum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode);        //显示数字串
void lcd_show_char(u16 x, u16 y, u8 num, u8 size, u8 mode);                 //显示单个字符
void lcd_show_string(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p);  //显示字符串,12/16字体

#endif
