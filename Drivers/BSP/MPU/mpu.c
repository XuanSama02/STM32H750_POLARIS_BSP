#include "mpu.h"
#include "led.h"
#include "delay.h"

/**
 * @brief ����ĳ���ڴ������MPU����
 * 
 * @param baseaddr MPU��������Ļ�ַ(�׵�ַ)
 * @param size     MPU��������Ĵ�С(������32�ı���,��λΪ�ֽ�),�����õ�ֵ�ο�:CORTEX_MPU_Region_Size
 * @param rnum     MPU���������,��Χ:0~7,���֧��8����������,�����õ�ֵ�ο���CORTEX_MPU_Region_Number
 * @param ap       ����Ȩ��,���ʹ�ϵ����:�����õ�ֵ�ο���CORTEX_MPU_Region_Permission_Attributes
 * @param sen      �Ƿ�������  MPU_ACCESS_NOT_SHAREABLE:������ MPU_ACCESS_SHAREABLE:����
 * @param cen      �Ƿ�����cache MPU_ACCESS_NOT_CACHEABLE:������ MPU_ACCESS_CACHEABLE:����
 * @param ben      �Ƿ�������  MPU_ACCESS_NOT_BUFFERABLE:������ MPU_ACCESS_BUFFERABLE:����
 * @return u8 0:�ɹ� 1:ʧ��
 */
u8 mpu_protection_config(u32 baseaddr, u32 size, u32 rnum, u8 ap, u8 sen, u8 cen, u8 ben)
{
    MPU_Region_InitTypeDef ymx_mpu_init;
    
    HAL_MPU_Disable();  //����MPU֮ǰ�ȹر�MPU,��������Ժ���ʹ��MPU
    //����MPU
    ymx_mpu_init.Enable           =MPU_REGION_ENABLE;              //ʹ�ܸñ�������
    ymx_mpu_init.Number           =rnum;                           //���ñ�������
    ymx_mpu_init.BaseAddress      =baseaddr;                       //���û�ַ
    ymx_mpu_init.Size             =size;                           //���ñ��������С
    ymx_mpu_init.SubRegionDisable =0X00;                           //��ֹ������
    ymx_mpu_init.TypeExtField     =MPU_TEX_LEVEL0;                 //����������չ��Ϊlevel0
    ymx_mpu_init.AccessPermission = (u8)ap;                        //���÷���Ȩ��,
    ymx_mpu_init.DisableExec      =MPU_INSTRUCTION_ACCESS_ENABLE;  //����ָ�����(�����ȡָ��)
    ymx_mpu_init.IsShareable      =sen;                            //�Ƿ���?
    ymx_mpu_init.IsCacheable      =cen;                            //�Ƿ�cache?
    ymx_mpu_init.IsBufferable     =ben;                            //�Ƿ񻺳�?
    HAL_MPU_ConfigRegion(&ymx_mpu_init);     //����MPU
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);  //����MPU
    return 0;
}

/**
 * @brief ������Ҫ�������ڴ��,����Ҫ�����Ĳ���,������ܳ���MCU������ʾ,����ͷ�ɼ����ݳ��������
 * 
 */
void mpu_memory_protection(void)
{
    //��������D1 SRAM 512KB
    mpu_protection_config(0x24000000,                 //����ַ
                          MPU_REGION_SIZE_512KB,      //����
                          MPU_REGION_NUMBER1,         //NUMER1
                          MPU_REGION_FULL_ACCESS,     //ȫ����
                          MPU_ACCESS_SHAREABLE,       //������
                          MPU_ACCESS_CACHEABLE,       //����cache
                          MPU_ACCESS_NOT_BUFFERABLE); //��ֹ����
    
    //����SDRAM����,��64M�ֽ�  
    mpu_protection_config(0XC0000000,                 //����ַ
                          MPU_REGION_SIZE_64MB,       //����
                          MPU_REGION_NUMBER2,         //NUMER2
                          MPU_REGION_FULL_ACCESS,     //ȫ����
                          MPU_ACCESS_NOT_SHAREABLE,   //��ֹ����
                          MPU_ACCESS_CACHEABLE,       //����cache
                          MPU_ACCESS_BUFFERABLE);     //������
}
