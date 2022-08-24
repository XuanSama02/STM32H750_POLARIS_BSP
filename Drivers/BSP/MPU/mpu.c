#include "mpu.h"
#include "led.h"
#include "delay.h"

/**
 * @brief 设置某个内存区域的MPU保护
 * 
 * @param baseaddr MPU保护区域的基址(首地址)
 * @param size     MPU保护区域的大小(必须是32的倍数,单位为字节),可设置的值参考:CORTEX_MPU_Region_Size
 * @param rnum     MPU保护区编号,范围:0~7,最大支持8个保护区域,可设置的值参考：CORTEX_MPU_Region_Number
 * @param ap       访问权限,访问关系如下:可设置的值参考：CORTEX_MPU_Region_Permission_Attributes
 * @param sen      是否允许共用  MPU_ACCESS_NOT_SHAREABLE:不允许 MPU_ACCESS_SHAREABLE:允许
 * @param cen      是否允许cache MPU_ACCESS_NOT_CACHEABLE:不允许 MPU_ACCESS_CACHEABLE:允许
 * @param ben      是否允许缓冲  MPU_ACCESS_NOT_BUFFERABLE:不允许 MPU_ACCESS_BUFFERABLE:允许
 * @return u8 0:成功 1:失败
 */
u8 mpu_protection_config(u32 baseaddr, u32 size, u32 rnum, u8 ap, u8 sen, u8 cen, u8 ben)
{
    MPU_Region_InitTypeDef ymx_mpu_init;
    
    HAL_MPU_Disable();  //配置MPU之前先关闭MPU,配置完成以后在使能MPU
    //配置MPU
    ymx_mpu_init.Enable           =MPU_REGION_ENABLE;              //使能该保护区域
    ymx_mpu_init.Number           =rnum;                           //设置保护区域
    ymx_mpu_init.BaseAddress      =baseaddr;                       //设置基址
    ymx_mpu_init.Size             =size;                           //设置保护区域大小
    ymx_mpu_init.SubRegionDisable =0X00;                           //禁止子区域
    ymx_mpu_init.TypeExtField     =MPU_TEX_LEVEL0;                 //设置类型扩展域为level0
    ymx_mpu_init.AccessPermission = (u8)ap;                        //设置访问权限,
    ymx_mpu_init.DisableExec      =MPU_INSTRUCTION_ACCESS_ENABLE;  //允许指令访问(允许读取指令)
    ymx_mpu_init.IsShareable      =sen;                            //是否共用?
    ymx_mpu_init.IsCacheable      =cen;                            //是否cache?
    ymx_mpu_init.IsBufferable     =ben;                            //是否缓冲?
    HAL_MPU_ConfigRegion(&ymx_mpu_init);     //配置MPU
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);  //开启MPU
    return 0;
}

/**
 * @brief 设置需要保护的内存段,必须要保护的部分,否则可能出现MCU屏不显示,摄像头采集数据出错等问题
 * 
 */
void mpu_memory_protection(void)
{
    //保护整个D1 SRAM 512KB
    mpu_protection_config(0x24000000,                 //基地址
                          MPU_REGION_SIZE_512KB,      //长度
                          MPU_REGION_NUMBER1,         //NUMER1
                          MPU_REGION_FULL_ACCESS,     //全访问
                          MPU_ACCESS_SHAREABLE,       //允许共享
                          MPU_ACCESS_CACHEABLE,       //允许cache
                          MPU_ACCESS_NOT_BUFFERABLE); //禁止缓冲
    
    //保护SDRAM区域,共64M字节  
    mpu_protection_config(0XC0000000,                 //基地址
                          MPU_REGION_SIZE_64MB,       //长度
                          MPU_REGION_NUMBER2,         //NUMER2
                          MPU_REGION_FULL_ACCESS,     //全访问
                          MPU_ACCESS_NOT_SHAREABLE,   //禁止共享
                          MPU_ACCESS_CACHEABLE,       //允许cache
                          MPU_ACCESS_BUFFERABLE);     //允许缓冲
}
