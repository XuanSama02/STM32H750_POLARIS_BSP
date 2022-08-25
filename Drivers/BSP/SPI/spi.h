#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/18
 * @brif:    �����ǿ�����SPI����
 * @version:
 * 2022/08/18: ʵ���˱����ǿ�����SPIЭ���д
 */

extern SPI_HandleTypeDef SPI2_Handler;  //SPI���

void spi2_init(void);
void spi2_set_speed(u32 SPI_BAUDRATEPRESCALER_x);
u8 spi2_read_write_byte(u8 txd);

#endif
