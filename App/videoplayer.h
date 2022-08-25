#ifndef __VIDEOPLAYER_H
#define __VIDEOPLAYER_H

#include <sys.h>
#include "avi.h"
#include "ff.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/25
 * @brif:    北极星开发板视频播放应用
 * @version:
 * 2022/08/25: 实现了视频播放功能
 */

#define AVI_AUDIO_BUF_SIZE  1024*5    //定义avi解码时,音频buf大小.
#define AVI_VIDEO_BUF_SIZE  1024*260  //定义avi解码时,视频buf大小.一般等于AVI_MAX_FRAME_SIZE的大小

void video_play(void);
u8 video_play_mjpeg(u8 *pname); 
void video_time_show(FIL *favi, AVI_INFO *aviinfo);
void video_info_show(AVI_INFO *aviinfo);
void video_bmsg_show(u8* name, u16 index, u16 total);
u16 video_get_tnum(u8 *path);
u8 video_seek(FIL *favi, AVI_INFO *aviinfo, u8 *mbuf);

#endif
