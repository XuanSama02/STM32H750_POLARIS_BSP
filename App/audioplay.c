#include "audioplay.h"
#include "ff.h"
#include "malloc.h"
#include "usart.h"
#include "es8388.h"
#include "sai.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "key.h"
#include "exfuns.h"
#include "text.h"
#include "string.h"

//音乐播放控制器
__audiodev audiodev;

/**
 * @brief 开始音频播放
 * 
 */
void audio_start(void)
{
    audiodev.status = 3<<0;  //开始播放+非暂停
    sai_play_start();
}

/**
 * @brief 关闭音频播放
 * 
 */
void audio_stop(void)
{
    audiodev.status = 0;
    sai_play_stop();
}

/**
 * @brief 得到path路径下，目标文件的总个数
 * 
 * @param path 路径
 * @return u16 总有效文件数
 */
u16 audio_get_tnum(u8 *path)
{
    u8  res;
    u16 rval=0;
    DIR tdir;             //临时目录
    FILINFO* tfileinfo;   //临时文件信息
    tfileinfo = (FILINFO*)mymalloc(SRAMIN, sizeof(FILINFO));  //申请内存
    res = f_opendir(&tdir, (const TCHAR*)path); //打开目录
    if(res == FR_OK&&tfileinfo)
    {
        while(1)          //查询总的有效文件数
        {
            res = f_readdir(&tdir, tfileinfo);                //读取目录下的一个文件
            if(res!=FR_OK || tfileinfo->fname[0]==0)          //错误了/到末尾了,退出
                break;
            res = f_typetell((u8*)tfileinfo->fname);
            if((res&0XF0) == 0X40)              //取高四位,看看是不是音乐文件
                rval++;   //有效文件数增加1
        }
    }
    myfree(SRAMIN, tfileinfo);                  //释放内存
    return rval;
}

/**
 * @brief 显示曲目索引，总曲目数
 * 
 * @param index 当前索引
 * @param total 总文件数
 */
void audio_index_show(u16 index, u16 total)
{
    lcd_show_xnum(60+0, 230, index, 3, 16, 0X80);  //索引
    lcd_show_char(60+24, 230, '/', 16, 0);
    lcd_show_xnum(60+32, 230, total, 3, 16, 0X80);  //总曲目
}

/**
 * @brief 显示播放时间，比特率
 * 
 * @param totsec  音频文件总时间长度
 * @param cursec  当前播放时间
 * @param bitrate 比特率(位速)
 */
void audio_msg_show(u32 totsec, u32 cursec, u32 bitrate)
{
    static u16 playtime = 0XFFFF;//播放时间标记
    if(playtime != cursec) //需要更新显示时间
    {
        playtime=cursec;
        //显示播放时间
        lcd_show_xnum(60, 210, playtime/60, 2, 16, 0X80);       //分钟
        lcd_show_char(60+16, 210, ':', 16, 0);
        lcd_show_xnum(60+24, 210, playtime%60, 2, 16, 0X80);    //秒钟
        lcd_show_char(60+40, 210, '/', 16, 0);
        //显示总时间
        lcd_show_xnum(60+48, 210, totsec/60, 2, 16, 0X80);      //分钟
        lcd_show_char(60+64, 210, ':', 16, 0);
        lcd_show_xnum(60+72, 210, totsec%60, 2, 16, 0X80);      //秒钟
        //显示位率
        lcd_show_xnum(60+110, 210, bitrate/1000, 4, 16, 0X80);  //显示位率
        lcd_show_string(60+110+32, 210, 200, 16, 16, "Kbps");
    }
}

/**
 * @brief 播放音乐
 * 
 */
void audio_play(void)
{
    DIR wav_dir;      //目录
    u8  res;
    u8  key;          //键值
    u8 *pname;        //带路径的文件名
    u16 wav_tot_num;  //音乐文件总数
    u16 curindex;     //当前索引
    u32 temp;
    u32 *wav_offset_tbl;     //音乐offset索引表
    FILINFO *wav_file_info;  //文件信息

    es8388_adda_config(1, 0);    //开启DAC关闭ADC
    es8388_output_config(1, 1);  //DAC选择通道1输出

    while(f_opendir(&wav_dir,"0:/MUSIC"))      //打开音乐文件夹
    {
        show_string(60, 190, 240, 16, "MUSIC文件夹错误!", 16, 0);
        delay_ms(200);
        lcd_fill(60, 190, 240, 206, WHITE);    //清除显示
        delay_ms(200);
    }
    wav_tot_num = audio_get_tnum("0:/MUSIC");  //得到总有效文件数
    while(wav_tot_num == NULL)                 //音乐文件总数为0
    {
        show_string(60, 190, 240, 16, "没有音乐文件!", 16, 0);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE);    //清除显示
        delay_ms(200);
    }
    wav_file_info  = (FILINFO*)mymalloc(SRAMIN, sizeof(FILINFO));  //申请内存
    pname          = mymalloc(SRAMIN, FF_MAX_LFN*2+1);             //为带路径的文件名分配内存
    wav_offset_tbl = mymalloc(SRAMIN, 4*wav_tot_num);              //申请4*totwavnum个字节的内存,用于存放音乐文件off block索引
    while(!wav_file_info || !pname || !wav_offset_tbl)             //内存分配出错
    {
        show_string(60, 190, 240, 16, "内存分配失败!", 16, 0);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE);  //清除显示
        delay_ms(200);
    }
    //记录索引
    res = f_opendir(&wav_dir, "0:/MUSIC");  //打开目录
    if(res == FR_OK)
    {
        curindex = 0;  //当前索引为0
        while(1)       //全部查询一遍
        {
            temp = wav_dir.dptr;                          //记录当前index
            res  = f_readdir(&wav_dir, wav_file_info);    //读取目录下的一个文件
            if(res!=FR_OK || wav_file_info->fname[0]==0)  //错误了/到末尾了,退出
                break;
            res = f_typetell((u8*)wav_file_info->fname);
            if((res&0XF0) == 0X40)                        //取高四位,看看是不是音乐文件
            {
                wav_offset_tbl[curindex] = temp;          //记录索引
                curindex++;
            }
        }
    }
       curindex = 0;      //从0开始显示
       res = f_opendir(&wav_dir, (const TCHAR*)"0:/MUSIC");  //打开目录
    while(res == FR_OK)  //打开成功
    {
        dir_sdi(&wav_dir, wav_offset_tbl[curindex]);  //改变当前目录索引
        res = f_readdir(&wav_dir, wav_file_info);     //读取目录下的一个文件
        if(res!=FR_OK || wav_file_info->fname[0]==0)  //错误了/到末尾了,退出
            break;
        strcpy((char*)pname, "0:/MUSIC/");            //复制路径(目录)
        strcat((char*)pname, (const char*)wav_file_info->fname);  //将文件名接在后面
        lcd_fill(60, 190, lcddev.width-1, 190+16, WHITE);         //清除之前的显示
        show_string(60, 190, lcddev.width-60, 16, (u8*)wav_file_info->fname, 16, 0);  //显示歌曲名字
        audio_index_show(curindex+1, wav_tot_num);
        key = audio_play_song(pname);                 //播放这个音频文件
        if(key == KEY2_PRES)       //上一曲
        {
            if(curindex)
                curindex--;
            else
                curindex = wav_tot_num-1;
        }
        else if(key == KEY0_PRES)  //下一曲
        {
            curindex++;
            if(curindex >= wav_tot_num)  //到末尾的时候,自动从头开始
                curindex=0;
        }
        else  //产生了错误
            break;
    }
    //释放内存
    myfree(SRAMIN, wav_file_info);
    myfree(SRAMIN, pname);
    myfree(SRAMIN, wav_offset_tbl);
}

/**
 * @brief 播放某个音频文件
 * 
 * @param fname 音频文件
 * @return u8 0:成功 其他:失败
 */
u8 audio_play_song(u8* fname)
{
    u8 res;
    res = f_typetell(fname); 
    switch(res)
    {
        case T_WAV:
            res = wav_play_song(fname);
            break;
        default://其他文件,自动跳转到下一曲
            printf("can't play:%s\r\n",fname);
            res = KEY0_PRES;
            break;
    }
    return res;
}
