#ifndef __FONTUPD_H
#define __FONTUPD_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/23
 * @brif:    北极星开发板字库更新驱动
 * @version:
 * 2022/08/23: 实现了北极星开发板字库更新
 */

//Apollo STM32开发板是从25M地址以后开始存放字库
//前面25M被fatfs占用了.
//25M以后紧跟4个字库+UNIGBK.BIN,总大小6.01M,被字库占用了,不能动
//31.01M以后,用户可以自由使用.

//字体信息保存地址,占41个字节,第1个字节用于标记字库是否存在.后续每8个字节一组,分别保存起始地址和文件大小
extern u32 FONTINFOADDR;

//字库信息结构体定义,用来保存字库基本信息，地址，大小等
__packed typedef struct
{
    u8 fontok;      //字库存在标志，0XAA，字库正常；其他，字库不存在
    u32 ugbkaddr;   //unigbk的地址
    u32 ugbksize;   //unigbk的大小
    u32 f12addr;    //gbk12地址
    u32 gbk12size;  //gbk12的大小
    u32 f16addr;    //gbk16地址
    u32 gbk16size;  //gbk16的大小
    u32 f24addr;    //gbk24地址
    u32 gbk24size;  //gbk24的大小
    u32 f32addr;    //gbk32地址
    u32 gbk32size;  //gbk32的大小
}_font_info; 

extern _font_info ftinfo;  //字库信息结构体

u32 fupd_prog(u16 x, u16 y, u8 size, u32 fsize, u32 pos);   //显示更新进度
u8 updata_fontx(u16 x ,u16 y, u8 size, u8 *fxpath, u8 fx);  //更新指定字库
u8 update_font(u16 x, u16 y, u8 size, u8* src);             //更新全部字库
u8 font_init(void);                                         //初始化字库

#endif
