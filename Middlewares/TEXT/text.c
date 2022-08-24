#include "sys.h" 
#include "fontupd.h"
#include "w25qxx.h"
#include "lcd.h"
#include "text.h"
#include "string.h"
#include "usart.h"

/**
 * @brief 从字库中查找出字模
 * 
 * @param code 字符串的开始地址,GBK码
 * @param mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小
 * @param size 字体大小
 */
void get_word_mat(unsigned char *code, unsigned char *mat, u8 size)
{
    unsigned char qh;
    unsigned char ql;
    unsigned char i;
    unsigned long foffset; 
    u8 csize = (size/8+((size%8)?1:0))*(size);      //得到字体一个字符对应点阵集所占的字节数
    qh =* code;
    ql =* (++code);
    if(qh<0x81 || ql<0x40 || ql==0xff || qh==0xff)  //非常用汉字
    {
        for(i=0; i<csize; i++)                      //填充满格
            *mat++=0x00;
        return;                                     //结束访问
    }
    if(ql < 0x7f)                                   //注意!
        ql -= 0x40;
    else
        ql -= 0x41;
    qh -= 0x81;
    foffset = ((unsigned long)190*qh+ql)*csize;     //得到字库中的字节偏移量
    switch(size)
    {
        case 12:
            w25qxx_read(mat, foffset+ftinfo.f12addr, csize);
            break;
        case 16:
            w25qxx_read(mat, foffset+ftinfo.f16addr, csize);
            break;
        case 24:
            w25qxx_read(mat, foffset+ftinfo.f24addr, csize);
            break;
        case 32:
            w25qxx_read(mat, foffset+ftinfo.f32addr, csize);
            break;
            
    }
}

/**
 * @brief 显示一个指定大小的汉字
 * 
 * @param x    汉字水平坐标
 * @param y    汉字垂直坐标
 * @param font 汉字GBK码
 * @param size 字体大小
 * @param mode 0:正常显示 1:叠加显示
 */
void show_font(u16 x, u16 y, u8 *font, u8 size, u8 mode)
{
    u8 temp;
    u8 t;
    u8 t1;
    u16 y0 = y;
    u8 dzk[128];
    u8 csize = (size/8+((size%8)?1:0))*(size);        //得到字体一个字符对应点阵集所占的字节数
    if(size!=12 && size!=16 && size!=24 && size!=32)  //不支持的size
        return;
    get_word_mat(font, dzk, size);                    //得到相应大小的点阵数据
    for(t=0; t<csize; t++)
    {
        temp = dzk[t];                                //得到点阵数据
        for(t1=0; t1<8; t1++)
        {
            if(temp&0x80)
                lcd_draw_point_fast(x, y, lcd_color_point);
            else if(mode == 0)
                lcd_draw_point_fast(x, y, lcd_color_back);
            temp <<= 1;
            y++;
            if((y-y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**
 * @brief 在指定位置开始显示一个字符串,支持自动换行
 * 
 * @param x      起点水平坐标
 * @param y      起点垂直坐标
 * @param width  区域宽度
 * @param height 区域高度
 * @param str    字符串
 * @param size   字体大小
 * @param mode   0:非叠加方式 1:叠加方式
 */
void show_string(u16 x, u16 y, u16 width, u16 height, u8*str, u8 size, u8 mode)
{
    u16 x0 = x;
    u16 y0 = y;
    u8 bHz = 0;  //字符或者中文
    while(*str != 0)  //数据未结束
    {
        if(!bHz)
        {
            if(*str > 0x80)  //中文
                bHz=1;
            else             //字符
            {
                if(x > (x0+width-size/2))  //换行
                {
                    y += size;
                    x  = x0;
                }
                if(y > (y0+height-size))  //越界返回
                    break;
                if(*str == 13)            //换行符号
                {
                    y += size;
                    x  = x0;
                    str++;
                }
                else
                    lcd_show_char(x, y, *str, size, mode);  //有效部分写入
                str++;
                x += size/2;  //字符,为全字的一半
            }
        }
        else  //中文
        {
            bHz = 0;  //有汉字库
            if(x > (x0+width-size))   //换行
            {
                y += size;
                x  = x0;
            }
            if(y > (y0+height-size))  //越界返回
                break;
            show_font(x, y, str, size, mode);  //显示这个汉字,空心显示
            str += 2;
            x   += size;  //下一个汉字偏移
        }
    }
}

/**
 * @brief 在指定宽度的中间显示字符串,如果字符长度超过了len,则用show_string显示
 * 
 * @param x    起点水平坐标
 * @param y    起点垂直坐标
 * @param str  字符串
 * @param size 字体大小
 * @param len  指定要显示的宽度
 */
void show_string_middle(u16 x, u16 y, u8*str, u8 size, u8 len)
{
    u16 strlenth=0;
    strlenth  = strlen((const char*)str);
    strlenth *= size/2;
    if(strlenth > len)
        show_string(x, y, lcddev.width, lcddev.height, str,size, 1);
    else
    {
        strlenth = (len-strlenth)/2;
        show_string(strlenth+x, y, lcddev.width, lcddev.height, str, size, 1);
    }
}
