#include "wavplay.h" 
#include "audioplay.h"
#include "usart.h" 
#include "delay.h" 
#include "malloc.h"
#include "ff.h"
#include "sai.h"
#include "es8388.h"
#include "key.h"
#include "led.h"

__wavctrl wavctrl;       //WAV控制结构体
vu8 wavtransferend = 0;  //sai传输完成标志
vu8 wavwitchbuf = 0;     //saibufx指示标志

/**
 * @brief WAV解析初始化
 * 
 * @param fname 文件路径+文件名
 * @param wavx  wav信息存放结构体指针
 * @return u8 0:成功 1:打开文件失败 2:非WAV文件 3:DATA区域未找到
 */
u8 wav_decode_init(u8* fname, __wavctrl* wavx)
{
    FIL*ftemp;
    u8 *buf; 
    u32 br = 0;
    u8 res = 0;

    ChunkRIFF *riff;
    ChunkFMT *fmt;
    ChunkFACT *fact;
    ChunkDATA *data;
    ftemp = (FIL*)mymalloc(SRAMIN, sizeof(FIL));
    buf   = mymalloc(SRAMIN, 512);
    if(ftemp&&buf)  //内存申请成功
    {
        res = f_open(ftemp, (TCHAR*)fname, FA_READ);  //打开文件
        if(res == FR_OK)
        {
            f_read(ftemp, buf, 512, &br);   //读取512字节在数据
            riff = (ChunkRIFF *)buf;        //获取RIFF块
            if(riff->Format == 0X45564157)  //是WAV文件
            {
                fmt  = (ChunkFMT *)(buf+12);  //获取FMT块 
                fact = (ChunkFACT *)(buf+12+8+fmt->ChunkSize);  //读取FACT块
                if(fact->ChunkID==0X74636166 || fact->ChunkID==0X5453494C)  //具有fact/LIST块的时候(未测试)
                    wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;
                else
                    wavx->datastart=12+8+fmt->ChunkSize;
                data = (ChunkDATA *)(buf+wavx->datastart);  //读取DATA块
                if(data->ChunkID == 0X61746164)             //解析成功
                {
                    wavx->audioformat = fmt->AudioFormat;    //音频格式
                    wavx->nchannels   = fmt->NumOfChannels;  //通道数
                    wavx->samplerate  = fmt->SampleRate;     //采样率
                    wavx->bitrate     = fmt->ByteRate*8;     //得到位速
                    wavx->blockalign  = fmt->BlockAlign;     //块对齐
                    wavx->bps         = fmt->BitsPerSample;  //位数,16/24/32位

                    wavx->datasize    = data->ChunkSize;     //数据块大小
                    wavx->datastart   = wavx->datastart+8;   //数据流开始的地方. 

                    printf("\r\n");
                    printf("wavx->audioformat:%d\r\n", wavx->audioformat);
                    printf("wavx->nchannels:%d\r\n",   wavx->nchannels);
                    printf("wavx->samplerate:%d\r\n",  wavx->samplerate);
                    printf("wavx->bitrate:%d\r\n",     wavx->bitrate);
                    printf("wavx->blockalign:%d\r\n",  wavx->blockalign);
                    printf("wavx->bps:%d\r\n",         wavx->bps);
                    printf("wavx->datasize:%d\r\n",    wavx->datasize);
                    printf("wavx->datastart:%d\r\n",   wavx->datastart);
                }
                else  //data区域未找到
                    res = 3;
            }
            else  //非wav文件
                res = 2;
        }
        else  //打开文件错误
            res = 1;
    }
    f_close(ftemp);
    myfree(SRAMIN, ftemp);  //释放内存
    myfree(SRAMIN, buf);
    return 0;
}

/**
 * @brief 填充buf
 * 
 * @param buf  数据区
 * @param size 填充数据量
 * @param bits 位数(16/24)
 * @return u32 读到的数据个数
 */
u32 wav_buffill(u8 *buf, u16 size, u8 bits)
{
    u16 readlen = 0;
    u32 bread;
    u16 i;
    u32 *p;
    u32 *pbuf;
    if(bits == 24)  //24bit音频,需要处理一下
    {
        readlen = (size/4)*3;  //此次要读取的字节数
        f_read(audiodev.file, audiodev.tbuf, readlen, (UINT*)&bread);  //读取数据
        pbuf = (u32*)buf;
        for(i=0; i<size/4; i++)
        {
            p = (u32*)(audiodev.tbuf+i*3);
            pbuf[i] = p[0];
        }
        bread = (bread*4)/3;  //填充后的大小
    }
    else
    {
        f_read(audiodev.file, buf, size, (UINT*)&bread);  //16bit音频,直接读取数据
        if(bread < size) //不够数据了,补充0
            for(i=bread; i<size-bread; i++)
                buf[i] = 0;
    }
    return bread;
}

/**
 * @brief WAV播放时,SAI DMA传输回调函数
 * 
 */
void wav_sai_dma_tx_callback(void) 
{
    u16 i;
    if(DMA1_Stream5->CR&(1<<19))
    {
        wavwitchbuf = 0;
        if((audiodev.status&0X01) == 0)
        {
            for(i=0; i<WAV_SAI_TX_DMA_BUFSIZE; i++)  //暂停
                audiodev.saibuf1[i] = 0;  //填充0
        }
    }
    else
    {
        wavwitchbuf = 1;
        if((audiodev.status&0X01) == 0)
        {
            for(i=0; i<WAV_SAI_TX_DMA_BUFSIZE; i++)  //暂停
            {
                audiodev.saibuf2[i] = 0;  //填充0
            }
        }
    }
    wavtransferend = 1;
}

/**
 * @brief 得到当前播放时间
 * 
 * @param fx   文件指针
 * @param wavx wav播放控制器
 */
void wav_get_curtime(FIL*fx, __wavctrl *wavx)
{
    long long fpos;
    wavx->totsec = wavx->datasize/(wavx->bitrate/8);  //歌曲总长度(单位:秒)
    fpos         = fx->fptr-wavx->datastart;          //得到当前文件播放到的地方
    wavx->cursec = fpos*wavx->totsec/wavx->datasize;  //当前播放到第多少秒了
}

/**
 * @brief 播放某个WAV文件
 * 
 * @param fname wav文件路径
 * @return u8 KEY0_PRES:下一曲
 *            KEY1_PRES:上一曲
 *            Other:    错误
 */
u8 wav_play_song(u8* fname)
{
    u8 key;
    u8 t = 0;
    u8 res;
    u32 fillnum;
    audiodev.file    = (FIL*)mymalloc(SRAMIN, sizeof(FIL));
    audiodev.saibuf1 = mymalloc(SRAMIN, WAV_SAI_TX_DMA_BUFSIZE);
    audiodev.saibuf2 = mymalloc(SRAMIN, WAV_SAI_TX_DMA_BUFSIZE);
    audiodev.tbuf    = mymalloc(SRAMIN, WAV_SAI_TX_DMA_BUFSIZE);
    if(audiodev.file && audiodev.saibuf1 && audiodev.saibuf2 && audiodev.tbuf)
    { 
        res = wav_decode_init(fname, &wavctrl);  //得到文件的信息
        if(res == 0)  //解析文件成功
        {
            if(wavctrl.bps == 16)
            {
                es8388_i2s_config(0, 3);  //飞利浦标准,16位数据长度
                saia_init(SAI_MODEMASTER_TX, SAI_CLOCKSTROBING_RISINGEDGE, SAI_DATASIZE_16);
                saia_samplerate_config(wavctrl.samplerate);  //设置采样率
                saia_tx_dma_init(audiodev.saibuf1, audiodev.saibuf2, WAV_SAI_TX_DMA_BUFSIZE/2, 1);  //配置TX DMA,16位
            }
            else if(wavctrl.bps == 24)
            {
                es8388_i2s_config(0, 0);  //飞利浦标准,24位数据长度
                saia_init(SAI_MODEMASTER_TX, SAI_CLOCKSTROBING_RISINGEDGE, SAI_DATASIZE_24);
                saia_samplerate_config(wavctrl.samplerate);  //设置采样率
                saia_tx_dma_init(audiodev.saibuf1, audiodev.saibuf2, WAV_SAI_TX_DMA_BUFSIZE/4,2);  //配置TX DMA,32位
            }
            sai_tx_callback = wav_sai_dma_tx_callback;  //回调函数指wav_sai_dma_callback
            audio_stop();
            res = f_open(audiodev.file, (TCHAR*)fname, FA_READ);  //打开文件
            if(res == 0)
            {
                f_lseek(audiodev.file, wavctrl.datastart);  //跳过文件头
                fillnum = wav_buffill(audiodev.saibuf1, WAV_SAI_TX_DMA_BUFSIZE, wavctrl.bps);
                fillnum = wav_buffill(audiodev.saibuf2, WAV_SAI_TX_DMA_BUFSIZE, wavctrl.bps);
                audio_start();
                while(res == 0)
                { 
                    while(wavtransferend == 0);  //等待wav传输完成
                    wavtransferend = 0;
                    if(fillnum != WAV_SAI_TX_DMA_BUFSIZE)  //播放结束?
                    {
                        res = KEY0_PRES;
                        break;
                    }
                    if(wavwitchbuf)
                        fillnum = wav_buffill(audiodev.saibuf2, WAV_SAI_TX_DMA_BUFSIZE, wavctrl.bps);  //填充buf2
                    else
                        fillnum = wav_buffill(audiodev.saibuf1, WAV_SAI_TX_DMA_BUFSIZE, wavctrl.bps);  //填充buf1
                    while(1)
                    {
                        key = key_scan(0); 
                        if(key == WKUP_PRES)  //暂停
                        {
                            if(audiodev.status&0X01)
                                audiodev.status &= ~(1<<0);
                            else
                                audiodev.status |= 0X01;
                        }
                        if(key==KEY2_PRES || key==KEY0_PRES)  //下一曲/上一曲
                        {
                            res = key;
                            break;
                        }
                        wav_get_curtime(audiodev.file, &wavctrl);  //得到总时间和当前播放的时间
                        audio_msg_show(wavctrl.totsec, wavctrl.cursec, wavctrl.bitrate);
                        t++;
                        if(t == 20)
                        {
                            t = 0;
                            LED0_Toggle();
                        }
                        if((audiodev.status&0X01) == 0)
                            delay_ms(10);
                        else
                            break;
                    }
                }
                audio_stop();
            }
            else
                res = 0XFF;
        }
        else
            res = 0XFF;
    }
    else
        res = 0XFF;
    //释放内存
    myfree(SRAMIN, audiodev.tbuf);
    myfree(SRAMIN, audiodev.saibuf1);
    myfree(SRAMIN, audiodev.saibuf2);
    myfree(SRAMIN, audiodev.file);
    return res;
}
