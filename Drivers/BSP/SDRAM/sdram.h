#ifndef __SDRAM_H
#define __SDRAM_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板SDRAM驱动
 * @version:
 * 1.0：实现了北极星开发板SDRAM初始化与读写功能
 */

extern SDRAM_HandleTypeDef SDRAM_Handler;  //SDRAM句柄

#define Bank5_SDRAM_ADDR  ((u32)(0XC0000000))  //SDRAM开始地址

//SDRAM配置参数

#define SDRAM_MODEREG_BURST_LENGTH_1              ((u16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2              ((u16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4              ((u16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8              ((u16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL       ((u16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED      ((u16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2               ((u16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3               ((u16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD     ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED  ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE      ((u16)0x0200)

void sdram_init(void);
void sdram_init_sequence(SDRAM_HandleTypeDef *hsdram);
void sdram_mpu_config(void);
void sdram_write(u8 *pbuffer, u32 addr, u32 n);
void sdram_read(u8 *pbuffer, u32 addr, u32 n);
u8 sdram_send_cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval);

#endif
