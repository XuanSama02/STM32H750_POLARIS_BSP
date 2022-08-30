#ifndef __LCD_IMAGE_H
#define __LCD_IMAGE_H

#include <stdint.h>

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/30
 * @brif:    BMP图片存储
 * @version:
 * 2022/08/30: 存储了BMP图片,供lcd_draw_image()函数使用
 *             阴码,逆向,逐行式,C51格式
 */

#define CDTU_SML 0
#define CDTU_MID 1
#define CDTU_BIG 0

#if(CDTU_SML == 1)
extern const uint8_t Image_CDTU_200x198[];
#endif

#if(CDTU_MID == 1)
extern const uint8_t Image_CDTU_300x297[];
#endif

#if(CDTU_BIG == 1)
extern const uint8_t Image_CDTU_400x396[];
#endif

#endif
