#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H

#include "sys.h"
#include "ff.h"
#include "wavplay.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/25
 * @brif:    �����ǿ��������ֲ���Ӧ��
 * @version:
 * 2022/08/25: ʵ����WAV���ֲ��Ź���
 */

//���ֲ��ſ�����
typedef __packed struct
{
	u8 *saibuf1;  //SAI����BUF1
	u8 *saibuf2;  //SAI����BUF2
	u8 *tbuf;     //��ʱ����,����24bit�����ʱ����Ҫ�õ�
	FIL *file;    //��Ƶ�ļ�ָ��
	u8 status;    //bit0:0,��ͣ����;1,��������
                  //bit1:0,��������;1,�������� 
}__audiodev; 

extern __audiodev audiodev;  //���ֲ��ſ�����

void wav_sai_dma_callback(void);

void audio_start(void);
void audio_stop(void);
u16  audio_get_tnum(u8 *path);
void audio_index_show(u16 index, u16 total);
void audio_msg_show(u32 totsec, u32 cursec, u32 bitrate);
void audio_play(void);
u8   audio_play_song(u8* fname);
 
#endif
