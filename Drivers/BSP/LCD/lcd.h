#ifndef __LCD_H
#define __LCD_H

#include "sys.h"
#include "stdlib.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ�����LCD����
 * @version:
 * 1.0��ʵ���˱����ǿ�����LCD����,��֧��RGB��,ȥ���˶�MCU����֧��
 */

//LCD��Ҫ������
typedef struct
{
    u16 width;    //LCD ���
    u16 height;   //LCD �߶�
    u16 id;       //LCD ID
    u8  dir;      //��ʾ���� 0:���� 1:����
    u16	wramcmd;  //��ʼдgramָ��
    u16 setxcmd;  //����x����ָ��
    u16 setycmd;  //����y����ָ�� 
}_lcd_dev;

extern _lcd_dev lcddev;  //����LCD��Ҫ����

extern u32  lcd_color_point;  //������ɫ,Ĭ�Ϻ�ɫ
extern u32  lcd_color_back;   //������ɫ,Ĭ�ϰ�ɫ

//������ɫ

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
#define BROWN    0XBC40 //��ɫ
#define BRRED    0XFC07 //�غ�ɫ
#define GRAY     0X8430 //��ɫ

//GUI��ɫ(PANEL����ɫ)

#define DARKBLUE   0X01CF  //����ɫ
#define LIGHTBLUE  0X7D7C  //ǳ��ɫ  
#define GRAYBLUE   0X5458  //����ɫ
 
#define LIGHTGREEN  0X841F //ǳ��ɫ
#define LIGHTGRAY   0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY       0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE   0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE      0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

void lcd_init(void);                 //��ʼ��LCD
void lcd_display_on(void);           //������ʾ
void lcd_display_off(void);          //�ر���ʾ
void lcd_display_direction(u8 dir);  //��ʾ��������

void lcd_clear(u32 color);          //����
u32  lcd_read_point(u16 x, u16 y);  //����
void lcd_draw_point(u16 x, u16 y);                        //����
void lcd_draw_point_fast(u16 x, u16 y, u32 color);        //����(����)
void lcd_draw_circle(u16 x0, u16 y0, u8 r);               //��Բ
void lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2);       //����
void lcd_draw_rectangle(u16 x1, u16 y1, u16 x2, u16 y2);  //������
void lcd_fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color);         //��䵥ɫ
void lcd_color_fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color);  //���ָ����ɫ

void lcd_show_num(u16 x, u16 y, u32 num, u8 len, u8 size);                  //��ʾ��������
void lcd_show_xnum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode);        //��ʾ���ִ�
void lcd_show_char(u16 x, u16 y, u8 num, u8 size, u8 mode);                 //��ʾ�����ַ�
void lcd_show_string(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p);  //��ʾ�ַ���,12/16����

#endif
