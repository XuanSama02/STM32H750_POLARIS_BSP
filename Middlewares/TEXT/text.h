#ifndef __TEXT_H
#define __TEXT_H

#include "fontupd.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板汉字显示驱动驱动
 * @version:
 * 1.0：实现了北极星开发板汉字显示驱动
 */

void get_word_mat(unsigned char *code, unsigned char *mat, u8 size);              //得到汉字的点阵码
void show_font(u16 x, u16 y, u8 *font, u8 size, u8 mode);                         //在指定位置显示一个汉字
void show_string(u16 x, u16 y, u16 width, u16 height, u8*str, u8 size, u8 mode);  //在指定位置显示一个字符串 
void show_string_middle(u16 x, u16 y, u8*str, u8 size, u8 len);                   //在指定宽度中间显示汉字

#endif
