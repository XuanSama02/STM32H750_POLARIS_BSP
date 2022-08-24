#include "fontupd.h"
#include "ff.h"
#include "w25qxx.h"
#include "lcd.h"
#include "string.h"
#include "malloc.h"
#include "delay.h"
#include "usart.h"

//字库区域占用的总扇区数大小(4个字库+unigbk表+字库信息=6302984字节,约占1539个W25QXX扇区,一个扇区4K字节)
#define FONTSECSIZE  1539

//字库存放起始地址
#define FONTINFOADDR  1024*1024*25

//用来保存字库基本信息，地址，大小等
_font_info ftinfo;

//字库存放在磁盘中的路径
u8*const GBK_PATH[5]=
{
    "/SYSTEM/FONT/UNIGBK.BIN",  //UNIGBK.BIN的存放位置
    "/SYSTEM/FONT/GBK12.FON",   //GBK12的存放位置
    "/SYSTEM/FONT/GBK16.FON",   //GBK16的存放位置
    "/SYSTEM/FONT/GBK24.FON",   //GBK24的存放位置
    "/SYSTEM/FONT/GBK32.FON",   //GBK32的存放位置
};

//更新时的提示信息
u8*const UPDATE_REMIND_TBL[5]=
{
    "Updating UNIGBK.BIN",  //提示正在更新UNIGBK.bin
    "Updating GBK12.FON ",  //提示正在更新GBK12
    "Updating GBK16.FON ",  //提示正在更新GBK16
    "Updating GBK24.FON ",  //提示正在更新GBK24
    "Updating GBK32.FON ",  //提示正在更新GBK32
};

/**
 * @brief 显示当前字体更新进度
 * 
 * @param x     起点水平坐标
 * @param y     起点垂直坐标
 * @param size  字体大小
 * @param fsize 整个文件大小
 * @param pos   当前文件指针位置
 * @return u32 
 */
u32 fupd_prog(u16 x, u16 y, u8 size, u32 fsize, u32 pos)
{
    float prog;
    u8 t  = 0XFF;
    prog  = (float)pos/fsize;
    prog *= 100;
    if(t != prog)
    {
        lcd_show_string(x+3*size/2, y, 240, 320, size, "%");
        t = prog;
        if(t>100)
            t = 100;
        lcd_show_num(x, y, t, 3, size);  //显示数值
    }
    return 0;
}

/**
 * @brief 更新某一个
 * 
 * @param x      起点水平坐标
 * @param y      起点垂直坐标
 * @param size   字体大小
 * @param fxpath 路径
 * @param fx     更新的内容 0:ungbk 1:gbk12 2:gbk16 3:gbk24 4:gbk32
 * @return u8 0:成功 其他:错误代码
 */
u8 updata_fontx(u16 x, u16 y, u8 size, u8 *fxpath, u8 fx)
{
    u32 flashaddr = 0;
    FIL * fftemp;
    u8 *tempbuf;
    u8 res;
    u16 bread;
    u32 offx = 0;
    u8 rval  = 0;
    fftemp = (FIL*)mymalloc(SRAMIN, sizeof(FIL));  //分配内存
    if(fftemp == NULL)
        rval=1;
    tempbuf = mymalloc(SRAMIN, 4096);              //分配4096个字节空间
    if(tempbuf == NULL)
        rval=1;
    res = f_open(fftemp, (const TCHAR*)fxpath, FA_READ);
    if(res)
        rval=2;                                    //打开文件失败
    if(rval == 0)
    {
        switch(fx)
        {
        case 0:  //更新UNIGBK.BIN
            ftinfo.ugbkaddr = FONTINFOADDR+sizeof(ftinfo);  //信息头之后，紧跟UNIGBK转换码表
            ftinfo.ugbksize = fftemp->obj.objsize;          //UNIGBK大小
            flashaddr       = ftinfo.ugbkaddr;
            break;
        case 1:  //更新GBK12.BIN
            ftinfo.f12addr   = ftinfo.ugbkaddr+ftinfo.ugbksize;  //UNIGBK之后，紧跟GBK12字库
            ftinfo.gbk12size = fftemp->obj.objsize;              //GBK12字库大小
            flashaddr        = ftinfo.f12addr;                   //GBK12的起始地址
            break;
        case 2:  //更新GBK16.BIN
            ftinfo.f16addr   = ftinfo.f12addr+ftinfo.gbk12size;  //GBK12之后，紧跟GBK16字库
            ftinfo.gbk16size = fftemp->obj.objsize;              //GBK16字库大小
            flashaddr        = ftinfo.f16addr;                   //GBK16的起始地址
            break;
        case 3:  //更新GBK24.BIN
            ftinfo.f24addr   = ftinfo.f16addr+ftinfo.gbk16size;  //GBK16之后，紧跟GBK24字库
            ftinfo.gbk24size = fftemp->obj.objsize;              //GBK24字库大小
            flashaddr        = ftinfo.f24addr;                   //GBK24的起始地址
            break;
        case 4:  //更新GBK32.BIN
            ftinfo.f32addr   = ftinfo.f24addr+ftinfo.gbk24size;  //GBK24之后，紧跟GBK32字库
            ftinfo.gbk32size = fftemp->obj.objsize;              //GBK32字库大小
            flashaddr        = ftinfo.f32addr;                   //GBK32的起始地址
            break;
        }

        while(res == FR_OK)  //死循环执行
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);  //读取数据
            if(res != FR_OK)                                      //执行错误
                break;
            w25qxx_write(tempbuf, offx+flashaddr, 4096);          //从0开始写入4096个数据
            offx += bread;
            fupd_prog(x, y, size, fftemp->obj.objsize, offx);     //进度显示
            if(bread != 4096)                                     //读完了
                break;
        }
        f_close(fftemp);
    }
    myfree(SRAMIN, fftemp);   //释放内存
    myfree(SRAMIN, tempbuf);  //释放内存
    return res;
}

/**
 * @brief 更新字体文件,UNIGBK,GBK12,GBK16,GBK24,GBK32一起更新
 * 
 * @param x    起点水平坐标
 * @param y    起点垂直坐标
 * @param size 字体大小
 * @param src  字库来源磁盘 "0:":SD卡 "1:":FLASH盘 "2:":NAND盘 "3:":U盘
 * @return u8 0:更新成功 1:错误代码
 */
u8 update_font(u16 x, u16 y, u8 size, u8* src)
{
    u8 *pname;
    u32 *buf;
    u8 res = 0;
    u16 i,j;
    FIL *fftemp;
    u8 rval = 0;
    res = 0XFF;
    ftinfo.fontok = 0XFF;
    pname  = mymalloc(SRAMIN, 100);                //申请100字节内存
    buf    = mymalloc(SRAMIN, 4096);               //申请4K字节内存
    fftemp = (FIL*)mymalloc(SRAMIN, sizeof(FIL));  //分配内存
    if(buf==NULL || pname==NULL || fftemp==NULL)
    {
        myfree(SRAMIN, fftemp);
        myfree(SRAMIN, pname);
        myfree(SRAMIN, buf);
        return 5;  //内存申请失败
    }
    for(i=0; i<5; i++)	//先查找文件UNIGBK,GBK12,GBK16,GBK24,GBK32是否正常
    {
        strcpy((char*)pname, (char*)src);                    //copy src内容到pname
        strcat((char*)pname, (char*)GBK_PATH[i]);            //追加具体文件路径
        res = f_open(fftemp, (const TCHAR*)pname, FA_READ);  //尝试打开
        if(res)
        {
            rval |= 1<<7;  //标记打开文件失败
            break;         //出错了,直接退出
        }
    }
    myfree(SRAMIN, fftemp);  //释放内存
    if(rval == 0)  //字库文件都存在
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ");   //提示正在擦除扇区
        for(i=0; i<FONTSECSIZE; i++)                                    //先擦除字库区域,提高写入速度
        {
            fupd_prog(x+20*size/2, y, size, FONTSECSIZE, i);            //进度显示
            w25qxx_read((u8*)buf, ((FONTINFOADDR/4096)+i)*4096, 4096);  //读出整个扇区的内容
            for(j=0; j<1024; j++)                                       //校验数据
            {
                if(buf[j] != 0XFFFFFFFF)                                //需要擦除
                    break;
            }
            if(j != 1024)                                               //需要擦除的扇区
                w25qxx_erase_sector((FONTINFOADDR/4096)+i);
        }
        for(i=0; i<5; i++)                                              //依次更新UNIGBK,GBK12,GBK16,GBK24,GBK32
        {
            lcd_show_string(x, y, 240, 320, size, UPDATE_REMIND_TBL[i]);
            strcpy((char*)pname, (char*)src);                           //copy src内容到pname
            strcat((char*)pname, (char*)GBK_PATH[i]);                   //追加具体文件路径
            res = updata_fontx(x+20*size/2, y, size, pname,i);          //更新字库
            if(res)
            {
                myfree(SRAMIN, buf);
                myfree(SRAMIN, pname);
                return 1+i;
            }
        }
        //全部更新好了
        ftinfo.fontok = 0XAA;
        w25qxx_write((u8*)&ftinfo, FONTINFOADDR, sizeof(ftinfo));      //保存字库信息
    }
    myfree(SRAMIN, pname);  //释放内存
    myfree(SRAMIN, buf);
    return rval;  //无错误
}

/**
 * @brief 初始化字库
 * 
 * @return u8 0:字库完好 1:字库丢失
 */
u8 font_init(void)
{
    u8 t = 0;
    w25qxx_init();
    while(t<10)  //连续读取10次,都是错误,说明字库出现问题,需要修复
    {
        t++;
        w25qxx_read((u8*)&ftinfo, FONTINFOADDR, sizeof(ftinfo));  //读出ftinfo结构体数据
        if(ftinfo.fontok == 0XAA)
            break;
        delay_ms(20);
    }
    if(ftinfo.fontok != 0XAA)
        return 1;
    return 0;
}
