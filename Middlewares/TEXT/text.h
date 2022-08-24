#ifndef __TEXT_H
#define __TEXT_H

#include "fontupd.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ����庺����ʾ��������
 * @version:
 * 1.0��ʵ���˱����ǿ����庺����ʾ����
 */

void get_word_mat(unsigned char *code, unsigned char *mat, u8 size);              //�õ����ֵĵ�����
void show_font(u16 x, u16 y, u8 *font, u8 size, u8 mode);                         //��ָ��λ����ʾһ������
void show_string(u16 x, u16 y, u16 width, u16 height, u8*str, u8 size, u8 mode);  //��ָ��λ����ʾһ���ַ��� 
void show_string_middle(u16 x, u16 y, u8*str, u8 size, u8 len);                   //��ָ������м���ʾ����

#endif
