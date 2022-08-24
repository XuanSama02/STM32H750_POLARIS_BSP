#include "sys.h" 
#include "fontupd.h"
#include "w25qxx.h"
#include "lcd.h"
#include "text.h"
#include "string.h"
#include "usart.h"

/**
 * @brief ���ֿ��в��ҳ���ģ
 * 
 * @param code �ַ����Ŀ�ʼ��ַ,GBK��
 * @param mat  ���ݴ�ŵ�ַ (size/8+((size%8)?1:0))*(size) bytes��С
 * @param size �����С
 */
void get_word_mat(unsigned char *code, unsigned char *mat, u8 size)
{
    unsigned char qh;
    unsigned char ql;
    unsigned char i;
    unsigned long foffset; 
    u8 csize = (size/8+((size%8)?1:0))*(size);      //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    qh =* code;
    ql =* (++code);
    if(qh<0x81 || ql<0x40 || ql==0xff || qh==0xff)  //�ǳ��ú���
    {
        for(i=0; i<csize; i++)                      //�������
            *mat++=0x00;
        return;                                     //��������
    }
    if(ql < 0x7f)                                   //ע��!
        ql -= 0x40;
    else
        ql -= 0x41;
    qh -= 0x81;
    foffset = ((unsigned long)190*qh+ql)*csize;     //�õ��ֿ��е��ֽ�ƫ����
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
 * @brief ��ʾһ��ָ����С�ĺ���
 * 
 * @param x    ����ˮƽ����
 * @param y    ���ִ�ֱ����
 * @param font ����GBK��
 * @param size �����С
 * @param mode 0:������ʾ 1:������ʾ
 */
void show_font(u16 x, u16 y, u8 *font, u8 size, u8 mode)
{
    u8 temp;
    u8 t;
    u8 t1;
    u16 y0 = y;
    u8 dzk[128];
    u8 csize = (size/8+((size%8)?1:0))*(size);        //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    if(size!=12 && size!=16 && size!=24 && size!=32)  //��֧�ֵ�size
        return;
    get_word_mat(font, dzk, size);                    //�õ���Ӧ��С�ĵ�������
    for(t=0; t<csize; t++)
    {
        temp = dzk[t];                                //�õ���������
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
 * @brief ��ָ��λ�ÿ�ʼ��ʾһ���ַ���,֧���Զ�����
 * 
 * @param x      ���ˮƽ����
 * @param y      ��㴹ֱ����
 * @param width  ������
 * @param height ����߶�
 * @param str    �ַ���
 * @param size   �����С
 * @param mode   0:�ǵ��ӷ�ʽ 1:���ӷ�ʽ
 */
void show_string(u16 x, u16 y, u16 width, u16 height, u8*str, u8 size, u8 mode)
{
    u16 x0 = x;
    u16 y0 = y;
    u8 bHz = 0;  //�ַ���������
    while(*str != 0)  //����δ����
    {
        if(!bHz)
        {
            if(*str > 0x80)  //����
                bHz=1;
            else             //�ַ�
            {
                if(x > (x0+width-size/2))  //����
                {
                    y += size;
                    x  = x0;
                }
                if(y > (y0+height-size))  //Խ�緵��
                    break;
                if(*str == 13)            //���з���
                {
                    y += size;
                    x  = x0;
                    str++;
                }
                else
                    lcd_show_char(x, y, *str, size, mode);  //��Ч����д��
                str++;
                x += size/2;  //�ַ�,Ϊȫ�ֵ�һ��
            }
        }
        else  //����
        {
            bHz = 0;  //�к��ֿ�
            if(x > (x0+width-size))   //����
            {
                y += size;
                x  = x0;
            }
            if(y > (y0+height-size))  //Խ�緵��
                break;
            show_font(x, y, str, size, mode);  //��ʾ�������,������ʾ
            str += 2;
            x   += size;  //��һ������ƫ��
        }
    }
}

/**
 * @brief ��ָ����ȵ��м���ʾ�ַ���,����ַ����ȳ�����len,����show_string��ʾ
 * 
 * @param x    ���ˮƽ����
 * @param y    ��㴹ֱ����
 * @param str  �ַ���
 * @param size �����С
 * @param len  ָ��Ҫ��ʾ�Ŀ��
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
