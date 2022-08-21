#ifndef __SAI_H
#define __SAI_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板SAI驱动
 * @version:
 * 1.0：实现了北极星开发板SAI驱动
 */

extern SAI_HandleTypeDef SAI1A_Handler;  //SAI1 Block A句柄

extern void (*sai_tx_callback)(void);    //SAI TX回调函数指针

void saia_init(u32 mode, u32 cpol, u32 datalen);
u8 saia_samplerate_config(u32 samplerate);
void saia_tx_dma_init(u8* buf0, u8 *buf1, u16 num, u8 width);
void saia_dma_enable(void);
void sai_play_start(void);
void sai_play_stop(void);

#endif
