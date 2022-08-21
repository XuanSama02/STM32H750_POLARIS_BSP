#ifndef __SAI_H
#define __SAI_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ�����SAI����
 * @version:
 * 1.0��ʵ���˱����ǿ�����SAI����
 */

extern SAI_HandleTypeDef SAI1A_Handler;  //SAI1 Block A���

extern void (*sai_tx_callback)(void);    //SAI TX�ص�����ָ��

void saia_init(u32 mode, u32 cpol, u32 datalen);
u8 saia_samplerate_config(u32 samplerate);
void saia_tx_dma_init(u8* buf0, u8 *buf1, u16 num, u8 width);
void saia_dma_enable(void);
void sai_play_start(void);
void sai_play_stop(void);

#endif
