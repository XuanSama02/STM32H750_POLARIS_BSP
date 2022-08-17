#include "usmart.h"
#include "usmart_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
#include "delay.h"	 	
#include "sys.h"
#include "lcd.h"
#include "es8388.h" 

//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
	(void*)delay_ms,"void delay_ms(u16 nms)",
 	(void*)delay_us,"void delay_us(u32 nus)",
	(void*)ES8388_Write_Reg,"u8 ES8388_Write_Reg(u8 reg, u8 val)",	
	(void*)ES8388_Read_Reg,"u8 ES8388_Read_Reg(u8 reg)",	 
	(void*)ES8388_I2S_Cfg,"void ES8388_I2S_Cfg(u8 fmt, u8 len)",	
	(void*)ES8388_HPvol_Set,"void ES8388_HPvol_Set(u8 volume)",	
	(void*)ES8388_SPKvol_Set,"void ES8388_SPKvol_Set(u8 volume)",	
	(void*)ES8388_3D_Set,"void ES8388_3D_Set(u8 depth)",	
	(void*)ES8388_ADDA_Cfg,"void ES8388_ADDA_Cfg(u8 dacen,u8 adcen)",	 
	(void*)ES8388_Output_Cfg,"void ES8388_Output_Cfg(u8 o1en,u8 o2en)",	
	(void*)ES8388_MIC_Gain,"void ES8388_MIC_Gain(u8 gain)",	
	(void*)ES8388_ALC_Ctrl,"void ES8388_ALC_Ctrl(u8 sel,u8 maxgain,u8 mingain)",	
	(void*)ES8388_Input_Cfg,"void ES8388_Input_Cfg(u8 in)",
		
};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   

