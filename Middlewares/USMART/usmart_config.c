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
	(void*)es8388_write_reg,"u8 es8388_write_reg(u8 reg, u8 val)",	
	(void*)es8388_read_reg,"u8 es8388_read_reg(u8 reg)",	 
	(void*)es8388_i2s_config,"void es8388_i2s_config(u8 fmt, u8 len)",	
	(void*)es8388_headphone_vol_config,"void es8388_headphone_vol_config(u8 volume)",	
	(void*)es8388_speaker_vol_config,"void es8388_speaker_vol_config(u8 volume)",	
	(void*)es8388_3d_config,"void es8388_3d_config(u8 depth)",	
	(void*)es8388_adda_config,"void es8388_adda_config(u8 dacen,u8 adcen)",	 
	(void*)es8388_output_config,"void es8388_output_config(u8 o1en,u8 o2en)",	
	(void*)es8388_mic_gain_config,"void es8388_mic_gain_config(u8 gain)",	
	(void*)es8388_alc_control,"void es8388_alc_control(u8 sel,u8 maxgain,u8 mingain)",	
	(void*)es8388_input_config,"void es8388_input_config(u8 in)",
		
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

