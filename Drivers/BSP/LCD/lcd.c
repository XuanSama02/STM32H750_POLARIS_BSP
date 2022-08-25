#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h"
#include "delay.h"
#include "ltdc.h"

u32 lcd_color_point  = 0xFF000000;  //������ɫ
u32 lcd_color_back   = 0xFFFFFFFF;  //����ɫ 

//����LCD��Ҫ����,Ĭ��Ϊ����
_lcd_dev lcddev;

/**
 * @brief ��GBR��ʽ��ɫת��ΪRGB��ʽ��ɫ,ILI93xx��������ΪGBR
 * 
 * @param c GBR��ʽ����ɫֵ
 * @return u16 RGB��ʽ����ɫֵ
 */
u16 lcd_bgr2rgb(u16 c)
{
    u16 r;
    u16 g;
    u16 b;
    u16 rgb;
    b   = (c>>0)&0x1f;
    g   = (c>>5)&0x3f;
    r   = (c>>11)&0x1f;
    rgb = (b<<11)+(g<<5)+(r<<0);
    return(rgb);
} 

/**
 * @brief �����㺯��
 * 
 * @param m ����
 * @param n �η�
 * @return u32 ������
 */
u32 lcd_pow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--)
        result *= m;
    return result;
}

/**
 * @brief ��ʼ��LCD
 * 
 */
void lcd_init(void)
{
    ltdc_init();               //��ʼ��RGB��
    printf("LCD ID:%x\r\n", lcddev.id);  //��ӡID  	
    lcd_display_direction(0);  //Ĭ������
    LCD_LED(1);                //��������
    lcd_clear(WHITE);          //��ʾ����
}

/**
 * @brief LCD������ʾ
 * 
 */
void lcd_display_on(void)
{
    ltdc_switch(1);  //����LCD
}

/**
 * @brief LCD�ر���ʾ
 * 
 */
void lcd_display_off(void)
{
    ltdc_switch(0);  //�ر�LCD
}

/**
 * @brief ����LCD��ʾ����
 * 
 * @param dir 0:���� 1:����
 */
void lcd_display_direction(u8 dir)
{
    lcddev.dir    = dir;          //д���µ���ʾ����
    ltdc_display_direction(dir);  //�����µ���ʾ����
    //��������ֱ���
    lcddev.width  = lcdltdc.width;
    lcddev.height = lcdltdc.height;
}

/**
 * @brief ��������
 * 
 * @param color ���������ɫ
 */
void lcd_clear(u32 color)
{
    ltdc_clear(color);
}

/**
 * @brief ��ȡ��ĳ�����ɫֵ
 * 
 * @param x ˮƽ����
 * @param y ��ֱ����
 * @return u32 �õ���ɫֵ
 */
u32 lcd_read_point(u16 x, u16 y)
{
    if(x>=lcddev.width || y>=lcddev.height)  //�����˷�Χ,ֱ�ӷ���
        return 0;
    return ltdc_read_point(x, y);
}

/**
 * @brief ���㺯��,ʹ�õ�ǰ������ɫ
 * 
 * @param x ˮƽ����
 * @param y ��ֱ����
 */
void lcd_draw_point(u16 x, u16 y)
{
    ltdc_draw_point(x, y, lcd_color_point);
}

/**
 * @brief ���㺯��,ʹ��ָ����ɫ
 * 
 * @param x     ˮƽ����
 * @param y     ��ֱ����
 * @param color ������ɫ
 */
void lcd_draw_point_fast(u16 x, u16 y, u32 color)
{
    ltdc_draw_point(x, y, color);
}

/**
 * @brief ��Բ����
 * 
 * @param x0 Բ��ˮƽ����
 * @param y0 Բ�Ĵ�ֱ����
 * @param r  Բ�뾶
 */
void lcd_draw_circle(u16 x0, u16 y0, u8 r)
{
    int a;
    int b;
    int di;
    a  = 0;
    b  = r;
    di = 3-(r<<1); //�ж��¸���λ�õı�־
    while(a <= b)
    {
        lcd_draw_point(x0+a, y0-b);  //5
        lcd_draw_point(x0+b, y0-a);  //0
        lcd_draw_point(x0+b, y0+a);  //4
        lcd_draw_point(x0+a, y0+b);  //6
        lcd_draw_point(x0-a, y0+b);  //1
        lcd_draw_point(x0-b, y0+a);
        lcd_draw_point(x0-a, y0-b);  //2
        lcd_draw_point(x0-b, y0-a);  //7
        a++;
        //ʹ��Bresenham�㷨��Բ
        if(di < 0)
            di += 4*a+6;
        else
        {
            di += 10+4*(a-b);
            b--;
        }
    }
}

/**
 * @brief ���ߺ���
 * 
 * @param x1 ���ˮƽ����
 * @param y1 ��㴹ֱ����
 * @param x2 �յ�ˮƽ����
 * @param y2 �յ㴹ֱ����
 */
void lcd_draw_line(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0;
    int yerr = 0;
    int delta_x;
    int delta_y;
    int distance;
    int incx;
    int incy;
    int urow;
    int ucol;
    delta_x = x2-x1;  //������������
    delta_y = y2-y1;
    urow = x1;
    ucol = y1;
    if(delta_x>0)  //���õ�������
        incx = 1;
    else if(delta_x == 0)  //��ֱ��
        incx = 0;
    else
    {
        incx    = -1;
        delta_x = -delta_x;
    }
    if(delta_y > 0)
        incy = 1;
    else if(delta_y == 0)  //ˮƽ��
        incy = 0;
    else
    {
        incy    = -1;
        delta_y = -delta_y;
    }
    if(delta_x > delta_y)  //ѡȡ��������������
        distance = delta_x;
    else
        distance = delta_y;
    for(t=0; t<=distance+1; t++)  //�������
    {
        lcd_draw_point(urow, ucol);  //����
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance)
        {
            xerr -= distance;
            urow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            ucol += incy;
        }
    }
}

/**
 * @brief �����κ���
 * 
 * @param x1 ���ˮƽ����
 * @param y1 ��㴹ֱ����
 * @param x2 �յ�ˮƽ����
 * @param y2 �յ㴹ֱ����
 */
void lcd_draw_rectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    lcd_draw_line(x1, y1, x2, y1);
    lcd_draw_line(x1, y1, x1, y2);
    lcd_draw_line(x1, y2, x2, y2);
    lcd_draw_line(x2, y1, x2, y2);
}

/**
 * @brief ��ָ���������������ɫ
 * 
 * @param sx ���ˮƽ����
 * @param sy ��㴹ֱ����
 * @param ex �յ�ˮƽ����
 * @param ey �յ㴹ֱ����
 * @param color �����ɫ
 */
void lcd_fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color)
{
    ltdc_fill(sx, sy, ex, ey, color);
}

/**
 * @brief ��ָ�������������ָ����ɫ(ʹ����ɫָ��)
 * 
 * @param sx ���ˮƽ����
 * @param sy ��㴹ֱ����
 * @param ex �յ�ˮƽ����
 * @param ey �յ㴹ֱ����
 * @param color �����ɫ
 */
void lcd_color_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
    ltdc_color_fill(sx, sy, ex, ey, color);
}

/**
 * @brief ��ʾ��������
 * 
 * @param x    ˮƽ����
 * @param y    ��ֱ����
 * @param num  ��ֵ
 * @param len  ����λ��
 * @param size �����С(12/16/24/32)
 */
void lcd_show_num(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t;
    u8 temp;
    u8 enshow = 0;
    for(t=0; t<len; t++)
    {
        temp = (num/lcd_pow(10, len-t-1))%10;
        if(enshow==0 && t<(len-1))
        {
            if(temp == 0)
            {
                lcd_show_char(x+(size/2)*t, y, ' ', size, 0);
                continue;
            }else enshow=1;
        }
        lcd_show_char(x+(size/2)*t, y, temp+'0', size, 0);
    }
}

/**
 * @brief ��ʾ���ִ�
 * 
 * @param x    ��ֱ����
 * @param y    ˮƽ����
 * @param num  ��ֵ
 * @param len  ���ִ�����
 * @param size �����С(12/16/24/32)
 * @param mode [7]     1:���0 0:�����
 *             [6]~[1] ����
 *             [0]     1:���ӷ�ʽ 0:�ǵ��ӷ�ʽ
 */
void lcd_show_xnum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{  
    u8 t;
    u8 temp;
    u8 enshow = 0;
    for(t=0; t<len; t++)
    {
        temp = (num/lcd_pow(10,len-t-1))%10;
        if(enshow == 0&&t<(len-1))
        {
            if(temp == 0)
            {
                if(mode&0X80)
                    lcd_show_char(x+(size/2)*t, y, '0', size, mode&0X01);
                else
                    lcd_show_char(x+(size/2)*t, y, ' ', size, mode&0X01);
                continue;
            }
            else
                enshow = 1;
        }
        lcd_show_char(x+(size/2)*t, y, temp+'0', size, mode&0X01);
    }
}

/**
 * @brief ��ʾ�����ַ�
 * 
 * @param x    ˮƽ����
 * @param y    ��ֱ����
 * @param num  Ҫ��ʾ���ַ�(" "->"~")
 * @param size �����С(12/16/24/32)
 * @param mode 1:���ӷ�ʽ 0:�ǵ��ӷ�ʽ
 */
void lcd_show_char(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp;
    u8 t1;
    u8 t;
    u16 y0 = y;
    u8 csize = (size/8+((size%8)?1:0))*(size/2);  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    num = num-' ';  //�õ�ƫ�ƺ��ֵ,ASCII�ֿ��Ǵӿո�ʼȡģ,����-' '���Ƕ�Ӧ�ַ����ֿ�
    for(t=0; t<csize; t++)
    {
        if(size == 12)       //����1206����
            temp = asc2_1206[num][t];
        else if(size == 16)  //����1608����
            temp = asc2_1608[num][t];
        else if(size == 24)  //����2412����
            temp = asc2_2412[num][t];
        else if(size == 32)  //����3216����
            temp = asc2_3216[num][t];
        else                 //û�е��ֿ�,�˳�
            return;
        for(t1=0; t1<8; t1++)
        {
            if(temp&0x80)
                lcd_draw_point_fast(x, y, lcd_color_point);
            else if(mode == 0)
                lcd_draw_point_fast(x, y, lcd_color_back);
            temp <<= 1;
            y++;
            if(y >= lcddev.height)  //������,�˳�
                return;
            if((y-y0) == size)
            {
                y = y0;
                x++;
                if(x >= lcddev.width)  //������,�˳�
                    return;
                break;
            }
        }
    }
}

/**
 * @brief 
 * 
 * @param x      ˮƽ����
 * @param y      ��ֱ����
 * @param width  ������
 * @param height ����߶�
 * @param size   �����С(12/16/24/32)
 * @param p      �ַ�����ʼ��ַ
 */
void lcd_show_string(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0   = x;
    width  += x;
    height += y;
    while((*p<='~') && (*p>=' '))  //�ַ��Ϸ��Լ��
    {
        if(x >= width)
        {
            x  = x0;
            y += size;
        }
        if(y >= height)  //�˳�
            break;
        lcd_show_char(x, y, *p, size, 0);
        x += size/2;
        p++;
    }
}
