#ifndef __LCD_IMAGE_H
#define __LCD_IMAGE_H

#include <stdint.h>

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/30
 * @brif:    BMPͼƬ�洢
 * @version:
 * 2022/08/30: �洢��BMPͼƬ,��lcd_draw_image()����ʹ��
 *             ����,����,����ʽ,C51��ʽ
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
