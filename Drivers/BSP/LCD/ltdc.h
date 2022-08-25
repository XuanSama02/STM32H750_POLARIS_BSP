#ifndef _LCD_H
#define _LCD_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/20
 * @brif:    �����ǿ�����LTDC����
 * @version:
 * 2022/08/20: ʵ���˱����ǿ�����LTDC����
 */

//LCD��ʾ����:

//������ɫ���ظ�ʽ,һ����RGB565
#define LCD_PIXFORMAT        LCD_PIXEL_FORMAT_RGB565	
//����Ĭ�ϱ�������ɫ
#define LTDC_BACKLAYERCOLOR  0X00000000	
//LCD֡�������׵�ַ,���ﶨ����SDRAM����,Ĭ�ϴ�С2MB
#define LCD_FRAME_BUF_ADDR   0XC0000000  
//LCD�������(PD13)
#define LCD_LED(n)  (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET))

//LCD LTDC��Ҫ������
typedef struct
{
    u32 pwidth;      //LCD���Ŀ��,������ʾ����ı�,���Ϊ0,˵��û��RGB������
    u32 pheight;     //LCD���ĸ߶�,������ʾ����ı�
    u16 hsw;         //ˮƽͬ�����
    u16 vsw;         //��ֱͬ�����
    u16 hbp;         //ˮƽ����
    u16 vbp;         //��ֱ����
    u16 hfp;         //ˮƽǰ��
    u16 vfp;         //��ֱǰ��
    u8 activelayer;  //��ǰ����:0/1
    u8 dir;          //0:���� 1:����
    u16 width;       //LCD���
    u16 height;      //LCD�߶�
    u32 pixsize;     //ÿ��������ռ�ֽ���
}_ltdc_dev;

extern _ltdc_dev lcdltdc;                  //����LCD LTDC����
extern LTDC_HandleTypeDef LTDC_Handler;    //LTDC���
extern DMA2D_HandleTypeDef DMA2D_Handler;  //DMA2D���
extern u32 *ltdc_framebuf[2];              //LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ�����

#define LCD_PIXEL_FORMAT_ARGB8888  0X00  //��ʾ��ʽ:ARGB8888
#define LCD_PIXEL_FORMAT_RGB888    0X01  //��ʾ��ʽ:RGB888
#define LCD_PIXEL_FORMAT_RGB565    0X02  //��ʾ��ʽ:RGB565
#define LCD_PIXEL_FORMAT_ARGB1555  0X03  //��ʾ��ʽ:ARGB1555
#define LCD_PIXEL_FORMAT_ARGB4444  0X04  //��ʾ��ʽ:ARGB4444
#define LCD_PIXEL_FORMAT_L8        0X05  //��ʾ��ʽ:L8
#define LCD_PIXEL_FORMAT_AL44      0X06  //��ʾ��ʽ:AL44
#define LCD_PIXEL_FORMAT_AL88      0X07  //��ʾ��ʽ:AL88

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
