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

//���ֲ��ſ�����
__audiodev audiodev;

/**
 * @brief ��ʼ��Ƶ����
 * 
 */
void audio_start(void)
{
    audiodev.status = 3<<0;  //��ʼ����+����ͣ
    sai_play_start();
}

/**
 * @brief �ر���Ƶ����
 * 
 */
void audio_stop(void)
{
    audiodev.status = 0;
    sai_play_stop();
}

/**
 * @brief �õ�path·���£�Ŀ���ļ����ܸ���
 * 
 * @param path ·��
 * @return u16 ����Ч�ļ���
 */
u16 audio_get_tnum(u8 *path)
{
    u8  res;
    u16 rval=0;
    DIR tdir;             //��ʱĿ¼
    FILINFO* tfileinfo;   //��ʱ�ļ���Ϣ
    tfileinfo = (FILINFO*)mymalloc(SRAMIN, sizeof(FILINFO));  //�����ڴ�
    res = f_opendir(&tdir, (const TCHAR*)path); //��Ŀ¼
    if(res == FR_OK&&tfileinfo)
    {
        while(1)          //��ѯ�ܵ���Ч�ļ���
        {
            res = f_readdir(&tdir, tfileinfo);                //��ȡĿ¼�µ�һ���ļ�
            if(res!=FR_OK || tfileinfo->fname[0]==0)          //������/��ĩβ��,�˳�
                break;
            res = f_typetell((u8*)tfileinfo->fname);
            if((res&0XF0) == 0X40)              //ȡ����λ,�����ǲ��������ļ�
                rval++;   //��Ч�ļ�������1
        }
    }
    myfree(SRAMIN, tfileinfo);                  //�ͷ��ڴ�
    return rval;
}

/**
 * @brief ��ʾ��Ŀ����������Ŀ��
 * 
 * @param index ��ǰ����
 * @param total ���ļ���
 */
void audio_index_show(u16 index, u16 total)
{
    lcd_show_xnum(60+0, 230, index, 3, 16, 0X80);  //����
    lcd_show_char(60+24, 230, '/', 16, 0);
    lcd_show_xnum(60+32, 230, total, 3, 16, 0X80);  //����Ŀ
}

/**
 * @brief ��ʾ����ʱ�䣬������
 * 
 * @param totsec  ��Ƶ�ļ���ʱ�䳤��
 * @param cursec  ��ǰ����ʱ��
 * @param bitrate ������(λ��)
 */
void audio_msg_show(u32 totsec, u32 cursec, u32 bitrate)
{
    static u16 playtime = 0XFFFF;//����ʱ����
    if(playtime != cursec) //��Ҫ������ʾʱ��
    {
        playtime=cursec;
        //��ʾ����ʱ��
        lcd_show_xnum(60, 210, playtime/60, 2, 16, 0X80);       //����
        lcd_show_char(60+16, 210, ':', 16, 0);
        lcd_show_xnum(60+24, 210, playtime%60, 2, 16, 0X80);    //����
        lcd_show_char(60+40, 210, '/', 16, 0);
        //��ʾ��ʱ��
        lcd_show_xnum(60+48, 210, totsec/60, 2, 16, 0X80);      //����
        lcd_show_char(60+64, 210, ':', 16, 0);
        lcd_show_xnum(60+72, 210, totsec%60, 2, 16, 0X80);      //����
        //��ʾλ��
        lcd_show_xnum(60+110, 210, bitrate/1000, 4, 16, 0X80);  //��ʾλ��
        lcd_show_string(60+110+32, 210, 200, 16, 16, "Kbps");
    }
}

/**
 * @brief ��������
 * 
 */
void audio_play(void)
{
    DIR wav_dir;      //Ŀ¼
    u8  res;
    u8  key;          //��ֵ
    u8 *pname;        //��·�����ļ���
    u16 wav_tot_num;  //�����ļ�����
    u16 curindex;     //��ǰ����
    u32 temp;
    u32 *wav_offset_tbl;     //����offset������
    FILINFO *wav_file_info;  //�ļ���Ϣ

    es8388_adda_config(1, 0);    //����DAC�ر�ADC
    es8388_output_config(1, 1);  //DACѡ��ͨ��1���

    while(f_opendir(&wav_dir,"0:/MUSIC"))      //�������ļ���
    {
        show_string(60, 190, 240, 16, "MUSIC�ļ��д���!", 16, 0);
        delay_ms(200);
        lcd_fill(60, 190, 240, 206, WHITE);    //�����ʾ
        delay_ms(200);
    }
    wav_tot_num = audio_get_tnum("0:/MUSIC");  //�õ�����Ч�ļ���
    while(wav_tot_num == NULL)                 //�����ļ�����Ϊ0
    {
        show_string(60, 190, 240, 16, "û�������ļ�!", 16, 0);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE);    //�����ʾ
        delay_ms(200);
    }
    wav_file_info  = (FILINFO*)mymalloc(SRAMIN, sizeof(FILINFO));  //�����ڴ�
    pname          = mymalloc(SRAMIN, FF_MAX_LFN*2+1);             //Ϊ��·�����ļ��������ڴ�
    wav_offset_tbl = mymalloc(SRAMIN, 4*wav_tot_num);              //����4*totwavnum���ֽڵ��ڴ�,���ڴ�������ļ�off block����
    while(!wav_file_info || !pname || !wav_offset_tbl)             //�ڴ�������
    {
        show_string(60, 190, 240, 16, "�ڴ����ʧ��!", 16, 0);
        delay_ms(200);
        lcd_fill(60, 190, 240, 146, WHITE);  //�����ʾ
        delay_ms(200);
    }
    //��¼����
    res = f_opendir(&wav_dir, "0:/MUSIC");  //��Ŀ¼
    if(res == FR_OK)
    {
        curindex = 0;  //��ǰ����Ϊ0
        while(1)       //ȫ����ѯһ��
        {
            temp = wav_dir.dptr;                          //��¼��ǰindex
            res  = f_readdir(&wav_dir, wav_file_info);    //��ȡĿ¼�µ�һ���ļ�
            if(res!=FR_OK || wav_file_info->fname[0]==0)  //������/��ĩβ��,�˳�
                break;
            res = f_typetell((u8*)wav_file_info->fname);
            if((res&0XF0) == 0X40)                        //ȡ����λ,�����ǲ��������ļ�
            {
                wav_offset_tbl[curindex] = temp;          //��¼����
                curindex++;
            }
        }
    }
       curindex = 0;      //��0��ʼ��ʾ
       res = f_opendir(&wav_dir, (const TCHAR*)"0:/MUSIC");  //��Ŀ¼
    while(res == FR_OK)  //�򿪳ɹ�
    {
        dir_sdi(&wav_dir, wav_offset_tbl[curindex]);  //�ı䵱ǰĿ¼����
        res = f_readdir(&wav_dir, wav_file_info);     //��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK || wav_file_info->fname[0]==0)  //������/��ĩβ��,�˳�
            break;
        strcpy((char*)pname, "0:/MUSIC/");            //����·��(Ŀ¼)
        strcat((char*)pname, (const char*)wav_file_info->fname);  //���ļ������ں���
        lcd_fill(60, 190, lcddev.width-1, 190+16, WHITE);         //���֮ǰ����ʾ
        show_string(60, 190, lcddev.width-60, 16, (u8*)wav_file_info->fname, 16, 0);  //��ʾ��������
        audio_index_show(curindex+1, wav_tot_num);
        key = audio_play_song(pname);                 //���������Ƶ�ļ�
        if(key == KEY2_PRES)       //��һ��
        {
            if(curindex)
                curindex--;
            else
                curindex = wav_tot_num-1;
        }
        else if(key == KEY0_PRES)  //��һ��
        {
            curindex++;
            if(curindex >= wav_tot_num)  //��ĩβ��ʱ��,�Զ���ͷ��ʼ
                curindex=0;
        }
        else  //�����˴���
            break;
    }
    //�ͷ��ڴ�
    myfree(SRAMIN, wav_file_info);
    myfree(SRAMIN, pname);
    myfree(SRAMIN, wav_offset_tbl);
}

/**
 * @brief ����ĳ����Ƶ�ļ�
 * 
 * @param fname ��Ƶ�ļ�
 * @return u8 0:�ɹ� ����:ʧ��
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
        default://�����ļ�,�Զ���ת����һ��
            printf("can't play:%s\r\n",fname);
            res = KEY0_PRES;
            break;
    }
    return res;
}
