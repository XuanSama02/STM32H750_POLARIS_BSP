#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h"
#include "delay.h"
#include "ltdc.h"

u32 lcd_color_point  = 0xFF000000;  //画笔颜色
u32 lcd_color_back   = 0xFFFFFFFF;  //背景色 

//管理LCD重要参数,默认为竖屏
_lcd_dev lcddev;

/**
 * @brief 将GBR格式颜色转化为RGB格式颜色,ILI93xx读出数据为GBR
 * 
 * @param c GBR格式的颜色值
 * @return u16 RGB格式的颜色值
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
 * @brief 幂运算函数
 * 
 * @param m 底数
 * @param n 次方
 * @return u32 运算结果
 */
u32 lcd_pow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--)
        result *= m;
    return result;
}

/**
 * @brief 初始化LCD
 * 
 */
void lcd_init(void)
{
    ltdc_init();               //初始化RGB屏
    printf("LCD ID:%x\r\n", lcddev.id);  //打印ID  	
    lcd_display_direction(0);  //默认竖屏
    LCD_LED(1);                //点亮背光
    lcd_clear(WHITE);          //显示白屏
}

/**
 * @brief LCD开启显示
 * 
 */
void lcd_display_on(void)
{
    ltdc_switch(1);  //开启LCD
}

/**
 * @brief LCD关闭显示
 * 
 */
void lcd_display_off(void)
{
    ltdc_switch(0);  //关闭LCD
}

/**
 * @brief 设置LCD显示方向
 * 
 * @param dir 0:竖屏 1:横屏
 */
void lcd_display_direction(u8 dir)
{
    lcddev.dir    = dir;          //写入新的显示方向
    ltdc_display_direction(dir);  //设置新的显示方向
    //交换长宽分辨率
    lcddev.width  = lcdltdc.width;
    lcddev.height = lcdltdc.height;
}

/**
 * @brief 清屏函数
 * 
 * @param color 清屏的填充色
 */
void lcd_clear(u32 color)
{
    ltdc_clear(color);
}

/**
 * @brief 读取个某点的颜色值
 * 
 * @param x 水平坐标
 * @param y 垂直坐标
 * @return u32 该点颜色值
 */
u32 lcd_read_point(u16 x, u16 y)
{
    if(x>=lcddev.width || y>=lcddev.height)  //超过了范围,直接返回
        return 0;
    return ltdc_read_point(x, y);
}

/**
 * @brief 画点函数,使用当前画笔颜色
 * 
 * @param x 水平坐标
 * @param y 垂直坐标
 */
void lcd_draw_point(u16 x, u16 y)
{
    ltdc_draw_point(x, y, lcd_color_point);
}

/**
 * @brief 画点函数,使用指定颜色
 * 
 * @param x     水平坐标
 * @param y     垂直坐标
 * @param color 画点颜色
 */
void lcd_draw_point_fast(u16 x, u16 y, u32 color)
{
    ltdc_draw_point(x, y, color);
}

/**
 * @brief 画圆函数
 * 
 * @param x0 圆心水平坐标
 * @param y0 圆心垂直坐标
 * @param r  圆半径
 */
void lcd_draw_circle(u16 x0, u16 y0, u8 r)
{
    int a;
    int b;
    int di;
    a  = 0;
    b  = r;
    di = 3-(r<<1); //判断下个点位置的标志
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
        //使用Bresenham算法画圆
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
 * @brief 画线函数
 * 
 * @param x1 起点水平坐标
 * @param y1 起点垂直坐标
 * @param x2 终点水平坐标
 * @param y2 终点垂直坐标
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
    delta_x = x2-x1;  //计算坐标增量
    delta_y = y2-y1;
    urow = x1;
    ucol = y1;
    if(delta_x>0)  //设置单步方向
        incx = 1;
    else if(delta_x == 0)  //垂直线
        incx = 0;
    else
    {
        incx    = -1;
        delta_x = -delta_x;
    }
    if(delta_y > 0)
        incy = 1;
    else if(delta_y == 0)  //水平线
        incy = 0;
    else
    {
        incy    = -1;
        delta_y = -delta_y;
    }
    if(delta_x > delta_y)  //选取基本增量坐标轴
        distance = delta_x;
    else
        distance = delta_y;
    for(t=0; t<=distance+1; t++)  //画线输出
    {
        lcd_draw_point(urow, ucol);  //画点
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
 * @brief 画矩形函数
 * 
 * @param x1 起点水平坐标
 * @param y1 起点垂直坐标
 * @param x2 终点水平坐标
 * @param y2 终点垂直坐标
 */
void lcd_draw_rectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    lcd_draw_line(x1, y1, x2, y1);
    lcd_draw_line(x1, y1, x1, y2);
    lcd_draw_line(x1, y2, x2, y2);
    lcd_draw_line(x2, y1, x2, y2);
}

/**
 * @brief 在指定矩形区域填充颜色
 * 
 * @param sx 起点水平坐标
 * @param sy 起点垂直坐标
 * @param ex 终点水平坐标
 * @param ey 终点垂直坐标
 * @param color 填充颜色
 */
void lcd_fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color)
{
    ltdc_fill(sx, sy, ex, ey, color);
}

/**
 * @brief 在指定矩形区域填充指定颜色(使用颜色指针)
 * 
 * @param sx 起点水平坐标
 * @param sy 起点垂直坐标
 * @param ex 终点水平坐标
 * @param ey 终点垂直坐标
 * @param color 填充颜色
 */
void lcd_color_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
    ltdc_color_fill(sx, sy, ex, ey, color);
}

/**
 * @brief 显示单个数字
 * 
 * @param x    水平坐标
 * @param y    垂直坐标
 * @param num  数值
 * @param len  数字位数
 * @param size 字体大小(12/16/24/32)
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
 * @brief 显示数字串
 * 
 * @param x    垂直坐标
 * @param y    水平坐标
 * @param num  数值
 * @param len  数字串长度
 * @param size 字体大小(12/16/24/32)
 * @param mode [7]     1:填充0 0:不填充
 *             [6]~[1] 保留
 *             [0]     1:叠加方式 0:非叠加方式
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
 * @brief 显示单个字符
 * 
 * @param x    水平坐标
 * @param y    垂直坐标
 * @param num  要显示的字符(" "->"~")
 * @param size 字体大小(12/16/24/32)
 * @param mode 1:叠加方式 0:非叠加方式
 */
void lcd_show_char(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp;
    u8 t1;
    u8 t;
    u16 y0 = y;
    u8 csize = (size/8+((size%8)?1:0))*(size/2);  //得到字体一个字符对应点阵集所占的字节数
    num = num-' ';  //得到偏移后的值,ASCII字库是从空格开始取模,所以-' '就是对应字符的字库
    for(t=0; t<csize; t++)
    {
        if(size == 12)       //调用1206字体
            temp = asc2_1206[num][t];
        else if(size == 16)  //调用1608字体
            temp = asc2_1608[num][t];
        else if(size == 24)  //调用2412字体
            temp = asc2_2412[num][t];
        else if(size == 32)  //调用3216字体
            temp = asc2_3216[num][t];
        else                 //没有的字库,退出
            return;
        for(t1=0; t1<8; t1++)
        {
            if(temp&0x80)
                lcd_draw_point_fast(x, y, lcd_color_point);
            else if(mode == 0)
                lcd_draw_point_fast(x, y, lcd_color_back);
            temp <<= 1;
            y++;
            if(y >= lcddev.height)  //超区域,退出
                return;
            if((y-y0) == size)
            {
                y = y0;
                x++;
                if(x >= lcddev.width)  //超区域,退出
                    return;
                break;
            }
        }
    }
}

/**
 * @brief 
 * 
 * @param x      水平坐标
 * @param y      垂直坐标
 * @param width  区域宽度
 * @param height 区域高度
 * @param size   字体大小(12/16/24/32)
 * @param p      字符串起始地址
 */
void lcd_show_string(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0   = x;
    width  += x;
    height += y;
    while((*p<='~') && (*p>=' '))  //字符合法性检查
    {
        if(x >= width)
        {
            x  = x0;
            y += size;
        }
        if(y >= height)  //退出
            break;
        lcd_show_char(x, y, *p, size, 0);
        x += size/2;
        p++;
    }
}
