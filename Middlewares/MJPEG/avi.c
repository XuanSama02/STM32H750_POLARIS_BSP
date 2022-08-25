#include "avi.h"
#include "mjpeg.h"
#include "usart.h"

AVI_INFO avix;  //avi文件相关信息 
u8*const AVI_VIDS_FLAG_TBL[2]={"00dc", "01dc"}; //视频编码标志字符串,00dc/01dc
u8*const AVI_AUDS_FLAG_TBL[2]={"00wb", "01wb"}; //音频编码标志字符串,00wb/01wb

/**
 * @brief AVI解码初始化
 * 
 * @param buf  输入缓冲区
 * @param size 缓冲区大小
 * @return AVISTATUS AVI_OK:AVI解析成功 其他:错误代码
 */
AVISTATUS avi_init(u8 *buf, u32 size)		 
{
    u16 offset;
    u8 *tbuf;
    AVISTATUS res=AVI_OK;
    AVI_HEADER *aviheader;
    LIST_HEADER *listheader;
    AVIH_HEADER *avihheader;
    STRH_HEADER *strhheader;

    STRF_BMPHEADER *bmpheader;
    STRF_WAVHEADER *wavheader;

    tbuf      = buf;
    aviheader = (AVI_HEADER*)buf;

    if(aviheader->RiffID != AVI_RIFF_ID)     //RIFF ID错误
        return AVI_RIFF_ERR;

    if(aviheader->AviID != AVI_AVI_ID)       //AVI ID错误
        return AVI_AVI_ERR;

    buf       += sizeof(AVI_HEADER);         //偏移
    listheader = (LIST_HEADER*)(buf);

    if(listheader->ListID != AVI_LIST_ID)    //LIST ID错误
        return AVI_LIST_ERR;

    if(listheader->ListType != AVI_HDRL_ID)  //HDRL ID错误
        return AVI_HDRL_ERR;

    buf       += sizeof(LIST_HEADER);        //偏移
    avihheader = (AVIH_HEADER*)(buf);

    if(avihheader->BlockID != AVI_AVIH_ID)   //AVIH ID错误
        return AVI_AVIH_ERR;

    avix.SecPerFrame  = avihheader->SecPerFrame;  //得到帧间隔时间
    avix.TotalFrame   = avihheader->TotalFrame;   //得到总帧数
    buf              += avihheader->BlockSize+8;  //偏移
    listheader        = (LIST_HEADER*)(buf);

    if(listheader->ListID != AVI_LIST_ID)    //LIST ID错误
        return AVI_LIST_ERR;

    if(listheader->ListType != AVI_STRL_ID)  //STRL ID错误
        return AVI_STRL_ERR;

    strhheader = (STRH_HEADER*)(buf+12);

    if(strhheader->BlockID != AVI_STRH_ID)   //STRH ID错误
        return AVI_STRH_ERR;

    if(strhheader->StreamType == AVI_VIDS_STREAM)					//视频帧在前
    {
        if(strhheader->Handler != AVI_FORMAT_MJPG)   //非MJPG视频流,不支持
            return AVI_FORMAT_ERR;

        avix.VideoFLAG = (u8*)AVI_VIDS_FLAG_TBL[0];  //视频流标记  "00dc"
        avix.AudioFLAG = (u8*)AVI_AUDS_FLAG_TBL[1];  //音频流标记  "01wb"
        bmpheader      = (STRF_BMPHEADER*)(buf+12+strhheader->BlockSize+8);  //strf

        if(bmpheader->BlockID != AVI_STRF_ID)        //STRF ID错误
            return AVI_STRF_ERR;

        avix.Width   = bmpheader->bmiHeader.Width;
        avix.Height  = bmpheader->bmiHeader.Height; 
        buf         += listheader->BlockSize+8;      //偏移
        listheader   = (LIST_HEADER*)(buf);
        if(listheader->ListID != AVI_LIST_ID)        //是不含有音频帧的视频文件
        {
            avix.SampleRate = 0;  //音频采样率
            avix.Channels   = 0;  //音频通道数
            avix.AudioType  = 0;  //音频格式
        }
        else
        {			
            if(listheader->ListType != AVI_STRL_ID)        //STRL ID错误
                return AVI_STRL_ERR;

            strhheader = (STRH_HEADER*)(buf+12);
            if(strhheader->BlockID != AVI_STRH_ID)         //STRH ID错误
                return AVI_STRH_ERR;

            if(strhheader->StreamType != AVI_AUDS_STREAM)  //格式错误
                return AVI_FORMAT_ERR;

            wavheader = (STRF_WAVHEADER*)(buf+12+strhheader->BlockSize+8);  //strf

            if(wavheader->BlockID != AVI_STRF_ID)          //STRF ID错误
                return AVI_STRF_ERR;
            avix.SampleRate = wavheader->SampleRate;       //音频采样率
            avix.Channels   = wavheader->Channels;         //音频通道数
            avix.AudioType  = wavheader->FormatTag;        //音频格式
        }
    }
    else if(strhheader->StreamType == AVI_AUDS_STREAM)  //音频帧在前
    { 
        avix.VideoFLAG = (u8*)AVI_VIDS_FLAG_TBL[1];     //视频流标记  "01dc"
        avix.AudioFLAG = (u8*)AVI_AUDS_FLAG_TBL[0];     //音频流标记  "00wb"
        wavheader      = (STRF_WAVHEADER*)(buf+12+strhheader->BlockSize+8);  //strf
        if(wavheader->BlockID != AVI_STRF_ID)           //STRF ID错误
            return AVI_STRF_ERR;

        avix.SampleRate  = wavheader->SampleRate;       //音频采样率
        avix.Channels    = wavheader->Channels;         //音频通道数
        avix.AudioType   = wavheader->FormatTag;        //音频格式
        buf             += listheader->BlockSize+8;     //偏移
        listheader       = (LIST_HEADER*)(buf);

        if(listheader->ListID != AVI_LIST_ID)
            return AVI_LIST_ERR;

        if(listheader->ListType != AVI_STRL_ID)         //STRL ID错误
            return AVI_STRL_ERR;

        strhheader = (STRH_HEADER*)(buf+12);

        if(strhheader->BlockID != AVI_STRH_ID)          //STRH ID错误
            return AVI_STRH_ERR;

        if(strhheader->StreamType != AVI_VIDS_STREAM)   //格式错误
            return AVI_FORMAT_ERR;

        bmpheader = (STRF_BMPHEADER*)(buf+12+strhheader->BlockSize+8);  //strf

        if(bmpheader->BlockID != AVI_STRF_ID)           //STRF ID错误
            return AVI_STRF_ERR;

        if(bmpheader->bmiHeader.Compression != AVI_FORMAT_MJPG)  //格式错误
            return AVI_FORMAT_ERR;

        avix.Width  = bmpheader->bmiHeader.Width;
        avix.Height = bmpheader->bmiHeader.Height;
    }
    offset = avi_srarch_id(tbuf, size, "movi");         //查找movi ID
    if(offset == 0)                                     //MOVI ID错误
        return AVI_MOVI_ERR;

    if(avix.SampleRate)  //有音频流,才查找
    {
        tbuf  += offset;
        offset = avi_srarch_id(tbuf, size, avix. AudioFLAG);  //查找音频流标记
        if(offset == 0)                                       //流错误
            return AVI_STREAM_ERR;
        tbuf              +=  offset+4;
        avix.AudioBufSize  =* ((u16*)tbuf);                   //得到音频流buf大小
    }
    printf("avi init ok\r\n");
    printf("avix.SecPerFrame:%d\r\n",   avix.SecPerFrame);
    printf("avix.TotalFrame:%d\r\n",    avix.TotalFrame);
    printf("avix.Width:%d\r\n",         avix.Width);
    printf("avix.Height:%d\r\n",        avix.Height);
    printf("avix.AudioType:%d\r\n",     avix.AudioType);
    printf("avix.SampleRate:%d\r\n",    avix.SampleRate);
    printf("avix.Channels:%d\r\n",      avix.Channels);
    printf("avix.AudioBufSize:%d\r\n",  avix.AudioBufSize);
    printf("avix.VideoFLAG:%s\r\n",     avix.VideoFLAG); 
    printf("avix.AudioFLAG:%s\r\n",     avix.AudioFLAG); 
    return res;
}

/**
 * @brief 查找 ID
 * 
 * @param buf  待查缓存区
 * @param size 缓存大小
 * @param id   要查找的id,必须是4字节长度
 * @return u32 0:查找失败 其他:movi ID偏移量
 */
u32 avi_srarch_id(u8* buf, u32 size, u8 *id)
{
    u32 i;
    u32 idsize = 0;
    size -= 4;
    for(i=0; i<size; i++)
    {
        if(buf[i] == id[0])
            if(buf[i+1] == id[1])
                if(buf[i+2] == id[2])	
                    if(buf[i+3] == id[3])
                    {
                        idsize = MAKEDWORD(buf+i+4);  //得到帧大小,必须大于16字节,才返回,否则不是有效数据
                        if(idsize > 0X10)             //找到"id"所在的位置
                            return i;
                    }
    }
    return 0;
}

/**
 * @brief 得到stream流信息
 * 
 * @param buf 流开始地址(必须是01wb/00wb/01dc/00dc开头)
 * @return AVISTATUS 
 */
AVISTATUS avi_get_streaminfo(u8* buf)
{
    avix.StreamID   = MAKEWORD(buf+2);        //得到流类型 
    avix.StreamSize = MAKEDWORD(buf+4);       //得到流大小  
    if(avix.StreamSize > AVI_MAX_FRAME_SIZE)  //帧大小太大了,直接返回错误
    {
        avix.StreamSize = 0;
        return AVI_STREAM_ERR;
    }
    if(avix.StreamSize%2)  //奇数加1(avix.StreamSize,必须是偶数)
        avix.StreamSize++;
    if(avix.StreamID==AVI_VIDS_FLAG || avix.StreamID==AVI_AUDS_FLAG)
        return AVI_OK;
    return AVI_STREAM_ERR;
}
