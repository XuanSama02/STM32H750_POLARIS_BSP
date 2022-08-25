#ifndef __FATTESTER_H
#define __FATTESTER_H

#include "sys.h"	   
#include "ff.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板FatFs测试代码
 * @version:
 * 1.0：实现了北极星开发板FatFs文件系统测试
 */
  
u8 mf_mount(u8* path, u8 mt);
u8 mf_open(u8*path, u8 mode);
u8 mf_close(void);
u8 mf_read(u16 len);
u8 mf_write(u8*dat, u16 len);
u8 mf_opendir(u8* path);
u8 mf_closedir(void);
u8 mf_readdir(void);
u8 mf_scan_files(u8 * path);
u32 mf_showfree(u8 *drv);
u8 mf_lseek(u32 offset);
u32 mf_tell(void);
u32 mf_size(void);
u8 mf_mkdir(u8*pname);
u8 mf_fmkfs(u8* path, u8 opt, u16 au);
u8 mf_unlink(u8 *pname);
u8 mf_rename(u8 *oldname, u8* newname);
void mf_getlabel(u8 *path);
void mf_setlabel(u8 *path); 
void mf_gets(u16 size);
u8 mf_putc(u8 c);
u8 mf_puts(u8*c);

#endif
