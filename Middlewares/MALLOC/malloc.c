#include "malloc.h"

//�ڴ��(64Byte����)

__align(64) u8 mem1base[MEM1_MAX_SIZE];                                  // (448KB/512KB) �ڲ�SRAM�ڴ��
__align(64) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0XC01F4000)));  // ( 62MB/ 64MB) �ⲿSDRAM�ڴ��,ǰ��2M�̶������LTDCʹ��(0xC0100000 ~ 0XC01F4000)
__align(64) u8 mem3base[MEM3_MAX_SIZE] __attribute__((at(0x30000000)));  // (240KB/256KB) �ڲ�SRAM1+SRAM2�ڴ��
__align(64) u8 mem4base[MEM4_MAX_SIZE] __attribute__((at(0x38000000)));  // ( 60KB/ 64KB) �ڲ�SRAM4�ڴ��
__align(64) u8 mem5base[MEM5_MAX_SIZE] __attribute__((at(0x20000000)));  // (120KB/128KB) �ڲ�DTCM�ڴ��
__align(64) u8 mem6base[MEM6_MAX_SIZE] __attribute__((at(0x00000000)));  // ( 60KB/ 64KB) �ڲ�ITCM�ڴ�� 

//�ڴ�����

u32 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                  //�ڲ�SRAM�ڴ��MAP
u32 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0XC01F4000 + MEM2_MAX_SIZE)));  //�ⲿSDRAM�ڴ��MAP
u32 mem3mapbase[MEM3_ALLOC_TABLE_SIZE] __attribute__((at(0x30000000 + MEM3_MAX_SIZE)));  //�ڲ�SRAM1+SRAM2�ڴ��MAP
u32 mem4mapbase[MEM4_ALLOC_TABLE_SIZE] __attribute__((at(0x38000000 + MEM4_MAX_SIZE)));  //�ڲ�SRAM4�ڴ��MAP
u32 mem5mapbase[MEM5_ALLOC_TABLE_SIZE] __attribute__((at(0x20000000 + MEM5_MAX_SIZE)));  //�ڲ�DTCM�ڴ��MAP
u32 mem6mapbase[MEM6_ALLOC_TABLE_SIZE] __attribute__((at(0x00000000 + MEM6_MAX_SIZE)));  //�ڲ�ITCM�ڴ��MAP

//�ڴ�������

const u32 memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE, MEM3_ALLOC_TABLE_SIZE,  //�ڴ���С
                                  MEM4_ALLOC_TABLE_SIZE, MEM5_ALLOC_TABLE_SIZE, MEM6_ALLOC_TABLE_SIZE};
const u32 memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE, MEM3_BLOCK_SIZE,                    //�ڴ�ֿ��С
                                  MEM4_BLOCK_SIZE, MEM5_BLOCK_SIZE, MEM6_BLOCK_SIZE};
const u32 memsize[SRAMBANK]    = {MEM1_MAX_SIZE, MEM2_MAX_SIZE, MEM3_MAX_SIZE,                          //�ڴ��ܴ�С
                                  MEM4_MAX_SIZE, MEM5_MAX_SIZE, MEM6_MAX_SIZE};

//�ڴ���������
struct _m_mallco_dev mallco_dev=
{
    my_mem_init,     //�ڴ��ʼ��
    my_mem_perused,  //�ڴ�ʹ����
    mem1base,    mem2base,    mem3base,    mem4base,    mem5base,    mem6base,     //�ڴ��
    mem1mapbase, mem2mapbase, mem3mapbase, mem4mapbase, mem5mapbase, mem6mapbase,  //�ڴ����״̬��
    0,           0,           0,           0,           0,           0,            //�ڴ����Ĭ��δ����
};

/**
 * @brief �����ڴ�
 * 
 * @param des Ŀ�ĵ�ַ
 * @param src Դ��ַ
 * @param n   ��Ҫ���Ƶ��ڴ泤��(Byte)
 */
void mymemcpy(void *des, void *src, u32 n)
{
    u8 *xdes = des;
    u8 *xsrc = src;
    while(n--)
        *xdes++ = *xsrc++;
}

/**
 * @brief �����ڴ�
 * 
 * @param s     �ڴ��׵�ַ
 * @param c     Ҫ���õ�ֵ
 * @param count ��Ҫ���õ��ڴ��С(Byte)
 */
void mymemset(void *s, u8 c, u32 count)
{
    u8 *xs = s;
    while(count--)
        *xs++ = c;
}

/**
 * @brief �ڴ�����ʼ��
 * 
 * @param memx �����ڴ��
 */
void my_mem_init(u8 memx)
{
    mymemset(mallco_dev.memmap[memx], 0, memtblsize[memx]*4);  //�ڴ�״̬����������
    mallco_dev.memrdy[memx] = 1;                               //�ڴ�������
}

/**
 * @brief ��ʼ�������ǿ�����ȫ���ڴ����
 * 
 */
void my_mem_init_all(void)
{
    my_mem_init(SRAMIN);    //��ʼ���ڲ��ڴ��(AXI)
    my_mem_init(SRAMEX);    //��ʼ���ⲿ�ڴ��(SDRAM)
    my_mem_init(SRAM12);    //��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
    my_mem_init(SRAM4);     //��ʼ��SRAM4�ڴ��(SRAM4)
    my_mem_init(SRAMDTCM);  //��ʼ��DTCM�ڴ��(DTCM)
    my_mem_init(SRAMITCM);  //��ʼ��ITCM�ڴ��(ITCM)
}

/**
 * @brief ��ȡ�ڴ�ʹ����
 * 
 * @param memx �����ڴ��
 * @return u16 ʹ����(������10��,0~1000,����0.0%~100.0%)
 */
u16 my_mem_perused(u8 memx)
{
    u32 used = 0;
    u32 i;
    for(i=0; i<memtblsize[memx]; i++)
    {
        if(mallco_dev.memmap[memx][i])
            used++;
    }
    return (used*1000)/(memtblsize[memx]);
}

/**
 * @brief �ڴ����(�ڲ�����)
 * 
 * @param memx �����ڴ��
 * @param size Ҫ������ڴ��С(Byte)
 * @return u32 0XFFFFFFFF:���� ����:�ڴ�ƫ�Ƶ�ַ
 */
u32 my_mem_malloc(u8 memx, u32 size)
{  
    signed long offset=0;
    u32 nmemb;      //��Ҫ���ڴ����
    u32 cmemb = 0;  //�������ڴ����
    u32 i;
    if(!mallco_dev.memrdy[memx])    //δ��ʼ��,��ִ�г�ʼ��
        mallco_dev.init(memx);
    if(size == 0)                   //����Ҫ����
        return 0XFFFFFFFF;
    nmemb = size/memblksize[memx];  //��ȡ��Ҫ����������ڴ����
    if(size%memblksize[memx])
        nmemb++;
    for(offset=memtblsize[memx]-1; offset>=0; offset--)  //���������ڴ������
    {
        if(!mallco_dev.memmap[memx][offset])   //�������ڴ��������
            cmemb++;
        else                                   //�����ڴ������
            cmemb=0;
        if(cmemb == nmemb)                     //�ҵ�������nmemb�����ڴ��
        {
            for(i=0;i<nmemb;i++)               //��ע�ڴ��ǿ�
                mallco_dev.memmap[memx][offset+i] = nmemb;
            return (offset*memblksize[memx]);  //����ƫ�Ƶ�ַ
        }
    }
    return 0XFFFFFFFF;  //δ�ҵ����Ϸ����������ڴ��
}

/**
 * @brief �ͷ��ڴ�(�ڲ�����)
 * 
 * @param memx   �����ڴ��
 * @param offset �ڴ��ַƫ��
 * @return u8 0:�ͷųɹ� 1:�ͷ�ʧ�� 2:ƫ�Ƴ���
 */
u8 my_mem_free(u8 memx, u32 offset)  
{
    int i;
    if(!mallco_dev.memrdy[memx])  //δ��ʼ��,��ִ�г�ʼ��
    {
        mallco_dev.init(memx);
        return 1;  //δ��ʼ��
    }
    if(offset<memsize[memx])      //ƫ�����ڴ����
    {
        int index = offset/memblksize[memx];         //ƫ�������ڴ�����
        int nmemb = mallco_dev.memmap[memx][index];  //�ڴ������
        for(i=0; i<nmemb; i++)                       //�ڴ������
            mallco_dev.memmap[memx][index+i] = 0;
        return 0;
    }
    else
        return 2;  //ƫ�Ƴ���
}

/**
 * @brief �ͷ��ڴ�(�ⲿ����)
 * 
 * @param memx �����ڴ��
 * @param ptr  �ڴ��׵�ַ
 */
void myfree(u8 memx, void *ptr)  
{  
    u32 offset;
    if(ptr==NULL)  //��ַΪ0
        return;
    offset = (u32)ptr - (u32)mallco_dev.membase[memx];
    my_mem_free(memx,offset);  //�ͷ��ڴ�
}

/**
 * @brief �����ڴ�(�ⲿ����)
 * 
 * @param memx �����ڴ��
 * @param size �ڴ��С(Byte)
 * @return void* ���䵽���ڴ��׵�ַ
 */
void *mymalloc(u8 memx, u32 size)
{
    u32 offset;
    offset = my_mem_malloc(memx, size);
    if(offset == 0XFFFFFFFF)
        return NULL;
    else
        return (void*)((u32)mallco_dev.membase[memx]+offset);
}

/**
 * @brief ���·����ڴ�(�ⲿ����)
 * 
 * @param memx �����ڴ��
 * @param ptr  ���ڴ��׵�ַ
 * @param size Ҫ������ڴ��С(Byte)
 * @return void* �·��䵽���ڴ��׵�ַ
 */
void *myrealloc(u8 memx, void *ptr, u32 size)  
{  
    u32 offset;
    offset = my_mem_malloc(memx, size);
    if(offset == 0XFFFFFFFF)
        return NULL;
    else
    {
        mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset), ptr, size);  //�������ڴ����ݵ����ڴ�
        myfree(memx, ptr);                                                   //�ͷž��ڴ�
        return (void*)((u32)mallco_dev.membase[memx]+offset);                //�������ڴ��׵�ַ
    }
}
