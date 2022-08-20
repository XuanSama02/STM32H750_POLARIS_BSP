#include "delay.h"
#include "sys.h"

//���ʹ��UCOS,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
    #include "includes.h"   //UCOS֧��
#endif

static u32 fac_us = 0;      //us��ʱ������

#if SYSTEM_SUPPORT_OS 	
    static u16 fac_ms = 0;  //ms��ʱ������,��os��,����ÿ�����ĵ�ms��
#endif

#if SYSTEM_SUPPORT_OS  //֧��OS,������UCOS
/*
��delay_us/delay_ms��Ҫ֧��OS��ʱ��,��Ҫ������OS��صĺ궨��ͺ�����֧��
3���궨��:
delay_osrunning:    ���ڱ�ʾOS��ǰ�Ƿ���������,�Ծ����Ƿ����ʹ����غ���
delay_ostickspersec:���ڱ�ʾOS�趨��ʱ�ӽ���,delay_init�����������������ʼ��systick
delay_osintnesting: ���ڱ�ʾOS�ж�Ƕ�׼���,�ж��ﲻ���Ե���,delay_msʹ�øò����������������
3������:
delay_osschedlock:  ��������OS�������,��ֹ����
delay_osschedunlock:���ڽ���OS�������,���¿�������
delay_ostimedly:    ����OS��ʱ,���������������

//Ŀǰ��֧��UCOSII��UCOSIII,����OS,��Ҫ������ֲ
*/

//֧��UCOSII
#ifdef  OS_CRITICAL_METHOD                    //OS_CRITICAL_METHOD������,˵����Ҫ֧��UCOSII
#define delay_osrunning     OSRunning         //OS�Ƿ����б��,0:������,1:������
#define delay_ostickspersec	OS_TICKS_PER_SEC  //OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting  OSIntNesting      //�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif

//֧��UCOSIII
#ifdef  CPU_CFG_CRITICAL_METHOD               //CPU_CFG_CRITICAL_METHOD������,˵����Ҫ֧��UCOSIII
#define delay_osrunning     OSRunning         //OS�Ƿ����б��,0:������,1:������
#define delay_ostickspersec OSCfg_TickRate_Hz //OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting  OSIntNestingCtr   //�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif

/**
 * @brief us����ʱʱ,�ر��������(��ֹ���us���ӳ�)
 * 
 */
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD  //ʹ��UCOSIII
    OS_ERR err;
    OSSchedLock(&err);          //UCOSIII�ķ�ʽ,��ֹ����,��ֹ���us��ʱ
#else                           //����UCOSII
    OSSchedLock();              //UCOSII�ķ�ʽ,��ֹ����,��ֹ���us��ʱ
#endif
}

/**
 * @brief us����ʱʱ,�ָ��������
 * 
 */
void delay_osschedunlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD  //ʹ��UCOSIII
    OS_ERR err;
    OSSchedUnlock(&err);        //UCOSIII�ķ�ʽ,�ָ�����
#else                           //����UCOSII
    OSSchedUnlock();            //UCOSII�ķ�ʽ,�ָ�����
#endif
}

/**
 * @brief ����OS�Դ�����ʱ������ʱ
 * 
 * @param ticks ��ʱ�Ľ�����
 */
void delay_ostimedly(u32 ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
    OS_ERR err;
    OSTimeDly(ticks, OS_OPT_TIME_PERIODIC, &err);  //UCOSIII��ʱ��������ģʽ
#else
    OSTimeDly(ticks);                              //UCOSII��ʱ
#endif
}

/**
 * @brief systick�жϷ�����,ʹ��OSʱ��Ҫʹ��
 * 
 */
void SysTick_Handler(void)
{	
    HAL_IncTick();
    if(delay_osrunning == 1)  //OS��ʼ����,��ִ�������ĵ��ȴ���
    {
        OSIntEnter();         //�����ж�
        OSTimeTick();         //����UCOS��ʱ�ӷ������               
        OSIntExit();          //���������л����ж�
    }
}
#endif /*SYSTEM_SUPPORT_OS*/

/**
 * @brief ��ʼ���ӳٺ���,��ʹ��UCOS��ʱ��,�˺������ʼ��UCOS��ʱ�ӽ���,SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
 * 
 * @param SYSCLK ϵͳʱ��Ƶ��
 */
void delay_init(u16 SYSCLK)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);  //SysTickƵ��ΪHCLK
    fac_us = SYSCLK;                            //�����Ƿ�ʹ��OS,fac_us����Ҫʹ��

#if SYSTEM_SUPPORT_OS                           //�����Ҫ֧��OS
    u32 reload;
    reload  = SYSCLK;                           //ÿ���ӵļ�������,��λΪK
    reload *= 1000000/delay_ostickspersec;      //����delay_ostickspersec�趨���ʱ��
                                                //reloadΪ24λ�Ĵ���,���ֵ:16777216,��216M��,Լ��77.7ms����
    fac_ms  = 1000/delay_ostickspersec;         //����OS������ʱ�����ٵ�λ
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  //����SysTick�ж�
    SysTick->LOAD  = reload; 					//ÿ1/OS_TICKS_PER_SEC���ж�һ��
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //����SysTick
#endif
}

#if SYSTEM_SUPPORT_OS  //�����Ҫ֧��OS
/**
 * @brief ΢�뼶��ʱ����
 * 
 * @param nus Ҫ��ʱ��us��,nus:0~204522252(��2^32/fac_us,@fac_us=21)
 */
void delay_us(u32 nus)
{
    u32 ticks;
    u32 told   = 0;
    u32 tnow   = 0;
    u32 tcnt   = 0;
    u32 reload = SysTick->LOAD;  //LOAD��ֵ	    	 
        ticks  = nus*fac_us;     //��Ҫ�Ľ����� 
    delay_osschedlock();         //��ֹOS���ȣ���ֹ���us��ʱ
        told   = SysTick->VAL;   //�ս���ʱ�ļ�����ֵ
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow < told)    //����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����
                tcnt += told-tnow;
            else
                tcnt += reload-tnow+told;
            told = tnow;
            if(tcnt >= ticks)  //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�
                break;
        }
    }
    delay_osschedunlock();     //�ָ�OS����
}

/**
 * @brief ���뼶��ʱ����
 * 
 * @param nms ��ʱ������
 */
void delay_ms(u16 nms)
{
    if(delay_osrunning&&delay_osintnesting == 0)  //���OS�Ѿ�������,�Ҳ������ж���(�ж����治���������)	    
    {
        if(nms >= fac_ms)                   //��ʱ��ʱ�����OS������ʱ������ 
        {
               delay_ostimedly(nms/fac_ms);    //OS��ʱ
        }
        nms %= fac_ms;                      //OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ
    }
    delay_us((u32)(nms*1000));              //��ͨ��ʽ��ʱ
}

#else  /*SYSTEM_SUPPORT_OS*/ //����Ҫ֧��OSʱ
/**
 * @brief ΢�뼶��ʱ����
 * 
 * @param nus ��ʱ΢����,nus<=1000
 */
void delay_us(u32 nus)
{		
    u32 ticks;
    u32 told   = 0;
    u32 tnow   = 0;
    u32 tcnt   = 0;
    u32 reload = SysTick->LOAD;  //LOAD��ֵ	    	 
        ticks  = nus*fac_us;     //��Ҫ�Ľ����� 
        told   = SysTick->VAL;   //�ս���ʱ�ļ�����ֵ
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)    //����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����
                tcnt += told-tnow;
            else
                tcnt += reload-tnow+told;
            told = tnow;
            if(tcnt >= ticks)  //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�
                break;
        }
    }
}

/**
 * @brief ���뼶��ʱ����
 * 
 * @param nms ��ʱ������
 */
void delay_ms(u16 nms)
{
    u32 i;
    for(i=0; i<nms; i++)
        delay_us(1000);
}

#endif
