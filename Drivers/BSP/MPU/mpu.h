#ifndef __MPU_H
#define __MPU_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板MPU驱动
 * @version:
 * 2022/08/18: 实现了北极星开发板MPU驱动
 */ 	

u8 mpu_protection_config(u32 baseaddr, u32 size, u32 rnum, u8 ap, u8 sen, u8 cen, u8 ben);
void mpu_memory_protection(void);

#endif
