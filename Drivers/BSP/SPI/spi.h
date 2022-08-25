#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    北极星开发板SPI驱动
 * @version:
 * 2022/08/18: 实现了北极星开发板SPI协议读写
 */

extern SPI_HandleTypeDef SPI2_Handler;  //SPI句柄

void spi2_init(void);
void spi2_set_speed(u32 SPI_BAUDRATEPRESCALER_x);
u8 spi2_read_write_byte(u8 txd);

#endif
