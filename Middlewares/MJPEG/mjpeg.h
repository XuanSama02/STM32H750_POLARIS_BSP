#ifndef __MJPEG_H
#define __MJPEG_H

#include "sys.h"
#include "jpegcodec.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/23
 * @brif:    北极星开发板MJPEG视频解码驱动
 * @version:
 * 2022/08/23: 实现了北极星开发板MJPEG视频解码
 */

extern jpeg_codec_typedef mjpeg;

u8 mjpeg_jpeg_core_init(jpeg_codec_typedef *tjpeg);
void mjpeg_jpeg_core_destroy(jpeg_codec_typedef *tjpeg); 
void mjpeg_dma_in_callback(void);
void mjpeg_dma_out_callback(void);
void mjpeg_endofcovert_callback(void);
void mjpeg_hdrover_callback(void);
u8 mjpeg_init(u16 offx, u16 offy, u32 width, u32 height);
void mjpeg_free(void);
void mjpeg_ltdc_dma2d_yuv2rgb_fill(u16 sx, u16 sy, jpeg_codec_typedef *tjpeg);
void mjpeg_fill_color(u16 x, u16 y, u16 width, u16 height, u16 *color);
u8 mjpeg_decode(u8* buf, u32 bsize);

#endif
