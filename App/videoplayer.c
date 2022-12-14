#include "videoplayer.h"
#include "string.h"
#include "key.h"
#include "usart.h"
#include "delay.h"
#include "timer.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "malloc.h"
#include "sai.h"
#include "es8388.h"
#include "mjpeg.h"
#include "avi.h"
#include "exfuns.h"
#include "text.h"

extern u16 frame;
extern vu8 frameup;  //视频播放时隙控制变量,当等于1的时候,可以更新下一帧视频

vu8 saiplaybuf;  //即将播放的音频帧缓冲编号
u8 *saibuf[4];   //音频缓冲帧,共4帧,4*5K=20K

/**
 * @brief 音频数据SAI DMA传输回调函数
 * 
 */
void audio_sai_dma_callback(void) 
{      
    saiplaybuf++;
    if(saiplaybuf > 3)
        saiplaybuf = 0;
    if(DMA1_Stream5->CR&(1<<19)) 
        DMA1_Stream5->M0AR = (u32)saibuf[saiplaybuf];  //指向下一个buf
    else
        DMA1_Stream5->M1AR = (u32)saibuf[saiplaybuf];  //指向下一个buf
}

/**
 * @brief 得到path路径下,目标文件的总个数
 * 
 * @param path 路径
 * @return u16 总有效文件数
 */
u16 video_get_tnum(u8 *path)
{	  
    u8 res;
    u16 rval = 0;
    DIR tdir;            //临时目录
    FILINFO* tfileinfo;  //临时文件信息
    tfileinfo = (FILINFO*)mymalloc(SRAMIN, sizeof(FILINFO));  //申请内存
    res = f_opendir(&tdir,(const TCHAR*)path);  //打开目录
    if(res==FR_OK && tfileinfo)
    {
        while(1)  //查询总的有效文件数
        {
            res = f_readdir(&tdir, tfileinfo);        //读取目录下的一个文件
            if(res!=FR_OK || tfileinfo->fname[0]==0)  //错误了/到末尾了,退出
                break;
            res = f_typetell((u8*)tfileinfo->fname);
            if((res&0XF0) == T_AVI)  //取高四位,看看是不是AVI视频文件
            {
                rval++;  //有效文件数增加1
            }
        }
    }
    myfree(SRAMIN, tfileinfo);  //释放内存
    return rval;
}

/**
 * @brief 显示当前播放时间
 * 
 * @param favi    当前播放的视频文件
 * @param aviinfo AVI控制结构体
 */
void video_time_show(FIL *favi, AVI_INFO *aviinfo)
{	 
    #if(VIDEO_ONLY != 1)

    static u32 oldsec;  //上一次的播放时间
    u8* buf;
    u32 totsec = 0;  //video文件总时间
    u32 cursec;      //当前播放时间
    totsec  = (aviinfo->SecPerFrame/1000)*aviinfo->TotalFrame;  //歌曲总长度(单位:ms)
    totsec /= 1000;  //秒钟数
    cursec  = ((double)favi->fptr/favi->obj.objsize)*totsec;    //当前播放到多少秒了
    if(oldsec != cursec)  //需要更新显示时间
    {
        buf = mymalloc(SRAMIN, 100);  //申请100字节内存
        oldsec = cursec;
        lcd_color_point = BLUE;
        sprintf((char*)buf,"播放时间:%02d:%02d:%02d/%02d:%02d:%02d", cursec/3600,(cursec%3600)/60,cursec%60,totsec/3600,(totsec%3600)/60,totsec%60);
        show_string(10, 90, lcddev.width-10, 16, buf, 16, 0);  //显示歌曲名字
        myfree(SRAMIN, buf);
    }

    #endif
}

/**
 * @brief 显示当前视频文件的相关信息
 * 
 * @param aviinfo AVI控制结构体
 */
void video_info_show(AVI_INFO *aviinfo)
{
    #if(VIDEO_ONLY != 1)

    u8 *buf;
    buf = mymalloc(SRAMIN, 100);  //申请100字节内存
    lcd_color_point = RED;
    sprintf((char*)buf, "声道数:%d,采样率:%d", aviinfo->Channels, aviinfo->SampleRate*10);
    show_string(10, 50, lcddev.width-10, 16, buf, 16, 0);  //显示歌曲名字
    sprintf((char*)buf, "帧率:%d帧", 1000/(aviinfo->SecPerFrame/1000));
    show_string(10, 70, lcddev.width-10, 16, buf, 16, 0);  //显示歌曲名字
    myfree(SRAMIN, buf);

    #endif
}

/**
 * @brief 视频基本信息显示
 * 
 * @param name  视频名字
 * @param index 当前索引
 * @param total 总文件数
 */
void video_bmsg_show(u8* name, u16 index, u16 total)
{
    #if(VIDEO_ONLY != 1)

    u8* buf;
    buf = mymalloc(SRAMIN, 100);  //申请100字节内存
    lcd_color_point = RED;
    sprintf((char*)buf, "文件名:%s", name);
    show_string(10, 10, lcddev.width-10, 16, buf, 16, 0);  //显示文件名
    sprintf((char*)buf, "索引:%d/%d", index, total);
    show_string(10, 30, lcddev.width-10, 16, buf, 16, 0);  //显示索引
    myfree(SRAMIN, buf);

    #endif
}

/**
 * @brief 播放视频
 * 
 */
void video_play(void)
{
    u8 res;
    DIR vdir;            //目录
    FILINFO *vfileinfo;  //文件信息
    u8 *pname;           //带路径的文件名
    u16 totavinum;       //视频文件总数
    u16 curindex;        //视频文件当前索引
    u8 key;              //键值
    u32 temp;
    u32 *voffsettbl;     //视频文件off block索引表

    while(f_opendir(&vdir, "0:/VIDEO"))  //打开视频文件夹
    {
        printf("视频文件夹出错,视频文件目录:SD/VIDEO\r\n");
        delay_ms(1000);
    }
    totavinum = video_get_tnum("0:/VIDEO");  //得到总有效文件数
    while(totavinum == NULL)  //视频文件总数为0
    {
        printf("SD/VIDEO目录下没有视频文件\r\n");
        delay_ms(1000);
    }
    vfileinfo  = (FILINFO*)mymalloc(SRAMIN, sizeof(FILINFO));  //为长文件缓存区分配内存
    pname      = mymalloc(SRAMIN, 2*FF_MAX_LFN+1);             //为带路径的文件名分配内存
    voffsettbl = mymalloc(SRAMIN, 4*totavinum);                //申请4*totavinum个字节的内存,用于存放视频文件索引
    while(vfileinfo==NULL || pname==NULL || voffsettbl==NULL)  //内存分配出错
    {
        printf("视频文件内存分配出错\r\n");
        delay_ms(1000);
    }
    //记录索引
    res = f_opendir(&vdir, "0:/VIDEO");  //打开目录
    if(res == FR_OK)
    {
        curindex = 0;  //当前索引为0
        while(1)  //全部查询一遍
        {
            temp = vdir.dptr;                         //记录当前offset
            res = f_readdir(&vdir, vfileinfo);        //读取目录下的一个文件
            if(res!=FR_OK || vfileinfo->fname[0]==0)  //错误了/到末尾了,退出
                break;
            res = f_typetell((u8*)vfileinfo->fname);
            if((res&0XF0) == T_AVI)  //取高四位,看看是不是AVI视频文件
            {
                voffsettbl[curindex] = temp;  //记录索引
                curindex++;
            }
        }
    }
    curindex = 0;  //从0开始显示
    res = f_opendir(&vdir, (const TCHAR*)"0:/VIDEO");  //打开目录
    while(res == FR_OK)  //打开成功
    {
        dir_sdi(&vdir, voffsettbl[curindex]);     //改变当前目录索引
        res = f_readdir(&vdir, vfileinfo);        //读取目录下的一个文件
        if(res!=FR_OK || vfileinfo->fname[0]==0)  //错误了/到末尾了,退出
            break;
        strcpy((char*)pname, "0:/VIDEO/");        //复制路径(目录)
        strcat((char*)pname, (const char*)vfileinfo->fname);  //将文件名接在后面
        lcd_clear(WHITE);                         //先清屏
#if(VIDEO_ONLY != 1)
        video_bmsg_show((u8*)vfileinfo->fname, curindex+1, totavinum);  //显示名字,索引等信息
#endif
        key = video_play_mjpeg(pname);            //播放这个视频文件
        if(key == KEY2_PRES)  //上一曲
        {
            if(curindex)
                curindex--;
            else
                curindex = totavinum-1;
        }
        else if(key == KEY0_PRES)  //下一曲
        {
            curindex++;		   	
            if(curindex >= totavinum)  //到末尾的时候,自动从头开始
                curindex = 0;
        }
        else  //产生了错误
            break;
    }
    //释放内存
    myfree(SRAMIN, vfileinfo);
    myfree(SRAMIN, pname);
    myfree(SRAMIN, voffsettbl);
}

/**
 * @brief 播放一个mjpeg文件
 * 
 * @param pname 文件名
 * @return u8 KEY0_PRES:下一曲
 *            KEY1_PRES:上一曲
 *            Other:    错误
 */
u8 video_play_mjpeg(u8 *pname)
{   
    u8* framebuf;  //视频解码buf
    u8* pbuf;      //buf指针
    FIL *favi;
    u8  res = 0;
    u32 offset = 0;
    u32	nr;
    u8 key;
    u8 saisavebuf;
    saibuf[0] = mymalloc(SRAM4,  AVI_AUDIO_BUF_SIZE);  //申请音频内存
    saibuf[1] = mymalloc(SRAM4,  AVI_AUDIO_BUF_SIZE);  //申请音频内存
    saibuf[2] = mymalloc(SRAM4,  AVI_AUDIO_BUF_SIZE);  //申请音频内存
    saibuf[3] = mymalloc(SRAM4,  AVI_AUDIO_BUF_SIZE);  //申请音频内存
    framebuf  = mymalloc(SRAMIN, AVI_VIDEO_BUF_SIZE);  //申请视频buf
    favi      = (FIL*)mymalloc(SRAMIN, sizeof(FIL));   //申请favi内存
    memset(saibuf[0], 0, AVI_AUDIO_BUF_SIZE);
    memset(saibuf[1], 0, AVI_AUDIO_BUF_SIZE);
    memset(saibuf[2], 0, AVI_AUDIO_BUF_SIZE);
    memset(saibuf[3], 0, AVI_AUDIO_BUF_SIZE);
    if(!saibuf[3] || !framebuf || !favi)
    {
        printf("memory error!\r\n");
        res = 0XFF;
    }
    while(res == 0)
    {
        res = f_open(favi, (char *)pname, FA_READ);
        if(res == 0)
        {
            pbuf = framebuf;
            res = f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, &nr);  //开始读取
            if(res)
            {
                printf("fread error:%d\r\n", res);
                break;
            }
            //开始avi解析
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);  //avi解析
            if(res || avix.Width>lcddev.width)
            {
                printf("avi err:%d\r\n", res);
                res = KEY0_PRES;
                break;
            }
            video_info_show(&avix);
            tim6_init(avix.SecPerFrame/100-1, 20000-1);   //10Khz计数频率,加1是100us
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");  //寻找movi ID
            avi_get_streaminfo(pbuf+offset+4);  //获取流信息
            f_lseek(favi, offset+12);           //跳过标志ID,读地址偏移到流数据开始处
            res = mjpeg_init((lcddev.width-avix.Width)/2, (lcddev.height-avix.Height)/2, avix.Width, avix.Height);  //JPG解码初始化
            if(res)
            {
                mjpeg_free();
                break;
            }
            if(avix.SampleRate)  //有音频信息,才初始化
            {
                es8388_adda_config(1, 0);    //开启DAC关闭ADC
                es8388_output_config(1, 1);  //DAC选择通道1输出

                es8388_i2s_config(0,3);  //飞利浦标准,16位数据长度
                saia_init(SAI_MODEMASTER_TX, SAI_CLOCKSTROBING_RISINGEDGE, SAI_DATASIZE_16);  //设置SAI,主发送,16位数据
                saia_samplerate_config(avix.SampleRate);  //设置采样率
                saia_tx_dma_init(saibuf[1], saibuf[2], avix.AudioBufSize/2, 1);  //配置DMA

                sai_tx_callback = audio_sai_dma_callback;  //回调函数指向SAI_DMA_Callback
                saiplaybuf = 0;
                saisavebuf = 0;
                sai_play_start();  //开启sai播放
            }
            while(1)  //播放循环
            {
                if(avix.StreamID == AVI_VIDS_FLAG)  //视频流
                {
                    pbuf = framebuf; 
                    f_read(favi, pbuf, avix.StreamSize+8, &nr);  //读入整帧+下一数据流ID信息
                    res = mjpeg_decode(pbuf, avix.StreamSize);
                    if(res)
                        printf("decode error!\r\n");
                    while(frameup == 0);  //等待时间到达(在TIM6的中断里面设置为1)
                    frameup = 0;  //标志清零
                    frame++;
                }
                else if(avix.StreamID == AVI_AUDS_FLAG)  //音频流
                {
                    video_time_show(favi, &avix);  //显示当前播放时间
                    saisavebuf++;
                    if(saisavebuf > 3)
                        saisavebuf = 0;
                    do
                    {
                        nr = saiplaybuf;
                        if(nr)
                            nr--;
                        else
                            nr = 3;
                    }while(saisavebuf == nr);  //碰撞等待
                    f_read(favi, saibuf[saisavebuf], avix.StreamSize+8, &nr);  //填充saibuf
                    pbuf = saibuf[saisavebuf];
                }
                key = key_scan(0);
                if(key==KEY0_PRES || key==KEY2_PRES)  //KEY0/KEY2按下,播放下一个/上一个视频
                {
                    res = key;
                    break;
                }
                else if(key==KEY1_PRES || key==WKUP_PRES)
                {
                    sai_play_stop();  //关闭音频
                    video_seek(favi, &avix, framebuf);
                    pbuf = framebuf;
                    sai_play_start();  //开启DMA播放
                }
                if(avi_get_streaminfo(pbuf+avix.StreamSize))  //读取下一帧 流标志
                { 
                    pbuf = framebuf;
                    res = f_read(favi, pbuf, AVI_VIDEO_BUF_SIZE, &nr);  //开始读取
                    if(res==0 && nr==AVI_VIDEO_BUF_SIZE)  //读取成功,且读取了指定长度的数据
                    {
                        offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "00dc");  //寻找AVI_VIDS_FLAG,00dc
                        avi_get_streaminfo(pbuf+offset);  //获取流信息
                        if(offset)
                            f_lseek(favi, (favi->fptr-AVI_VIDEO_BUF_SIZE)+offset+8);
                    }
                    else
                    {
                        printf("frame error \r\n"); 
                        res = KEY0_PRES;
                        break;
                    }
                }
            }
            sai_play_stop();       //关闭音频
            TIM6->CR1 &= ~(1<<0);  //关闭定时器6
            mjpeg_free();          //释放内存
            f_close(favi);
        }
    }
    myfree(SRAM4,  saibuf[0]);
    myfree(SRAM4,  saibuf[1]);
    myfree(SRAM4,  saibuf[2]);
    myfree(SRAM4,  saibuf[3]);
    myfree(SRAMIN, framebuf);
    myfree(SRAMIN, favi);
    return res;
}

/**
 * @brief AVI文件查找
 * 
 * @param favi 
 * @param aviinfo 
 * @param mbuf 
 * @return u8 
 */
u8 video_seek(FIL *favi, AVI_INFO *aviinfo, u8 *mbuf)
{
    u32 fpos = favi->fptr;
    u8 *pbuf;
    u32 offset;
    u32 br;
    u32 delta;
    u32 totsec;
    u8 key;
    totsec  = (aviinfo->SecPerFrame/1000)*aviinfo->TotalFrame;
    totsec /= 1000;  //秒钟数
    delta   = (favi->obj.objsize/totsec)*5;  //每次前进5秒钟的数据量
    while(1)
    {
        key = key_scan(1);
        if(key == WKUP_PRES)  //快进
        {
            if(fpos < favi->obj.objsize)
                fpos += delta; 
            if(fpos > (favi->obj.objsize-AVI_VIDEO_BUF_SIZE))
                fpos = favi->obj.objsize-AVI_VIDEO_BUF_SIZE;
        }
        else if(key == KEY1_PRES)  //快退
        {
            if(fpos > delta)
                fpos -= delta;
            else
                fpos = 0; 
        }
        else if(avix.StreamID == AVI_VIDS_FLAG)
            break;
        f_lseek(favi, fpos);
        f_read(favi, mbuf, AVI_VIDEO_BUF_SIZE, &br);  //读入整帧+下一数据流ID信息 
        pbuf = mbuf;
        if(fpos == 0)  //从0开始,得先寻找movi ID
        {
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
        }
        else
            offset = 0;
        offset += avi_srarch_id(pbuf+offset, AVI_VIDEO_BUF_SIZE, avix.VideoFLAG);  //寻找视频帧
        avi_get_streaminfo(pbuf+offset);  //获取流信息
        f_lseek(favi, fpos+offset+8);     //跳过标志ID,读地址偏移到流数据开始处
        if(avix.StreamID == AVI_VIDS_FLAG)
        {
            f_read(favi, mbuf, avix.StreamSize+8, &br);  //读入整帧
            mjpeg_decode(mbuf, avix.StreamSize);         //显示视频帧
        }
        else 
            printf("error flag");
        video_time_show(favi, &avix);
    }
    return 0;
}
