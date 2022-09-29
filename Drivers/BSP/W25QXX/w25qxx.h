#ifndef __W25QXX_H
#define __W25QXX_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/20
 * @brif:    北极星开发板W25Qxx驱动
 * @version:
 * 2022/08/20: 实现了北极星开发板W25Qxx读写
 */

//W25Qxx系列芯片列表	   

#define W25Q08  0XEF13
#define W25Q16  0XEF14
#define W25Q32  0XEF15
#define W25Q64  0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

extern u16 W25QXX_TYPE;  //定义W25QXX芯片型号		   

//W25QXX的片选信号
#define W25QXX_CS(n) do{ n ? HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET); }while(0)

//W25x指令表

#define W25X_WriteEnable      0x06
#define W25X_WriteDisable     0x04
#define W25X_ReadStatusReg1   0x05
#define W25X_ReadStatusReg2   0x35
#define W25X_ReadStatusReg3   0x15
#define W25X_WriteStatusReg1  0x01
#define W25X_WriteStatusReg2  0x31
#define W25X_WriteStatusReg3  0x11
#define W25X_ReadData         0x03
#define W25X_FastReadData     0x0B
#define W25X_FastReadDual     0x3B
#define W25X_PageProgram      0x02
#define W25X_BlockErase       0xD8
#define W25X_SectorErase      0x20
#define W25X_ChipErase        0xC7
#define W25X_PowerDown        0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID         0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID    0x9F
#define W25X_Enable4ByteAddr  0xB7
#define W25X_Exit4ByteAddr    0xE9

void w25qxx_init(void);                 //初始化W25Qxx
u16 w25qxx_read_id(void);               //读取FLASH ID
u8 w25qxx_read_sr(u8 regno);            //读取状态寄存器
void w25qxx_write_sr(u8 regno, u8 sr);  //写状态寄存器

void w25qxx_4byteaddr_enable(void);  //使能4字节地址模式
void w25qxx_write_enable(void);      //写使能
void w25qxx_write_disable(void);     //写保护

void w25qxx_read(u8* pbuffer, u32 addr, u16 num);   //读取flash

void w25qxx_write(u8* pbuffer, u32 addr, u16 num);          //写入flash
void w25qxx_write_nocheck(u8* pbuffer, u32 addr, u16 num);  //无校验写入
void w25qxx_erase_chip(void);        //整片擦除
void w25qxx_erase_sector(u32 addr);  //扇区擦除

void w25qxx_wait_busy(void);  //等待空闲
void w25qxx_powerdown(void);  //进入掉电模式
void w25qxx_wakeup(void);     //唤醒

#endif
