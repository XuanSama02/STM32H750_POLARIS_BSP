#ifndef __ES8388_H
#define __ES8388_H

#include "sys.h"

/**
 * https://github.com/XuanSama02
 * @auther:  XuanSama02
 * @date:    2022/08/20
 * @brif:    北极星开发板ES8388驱动
 * @version:
 * 2022/08/20: 实现了北极星开发板ES8388驱动
 */

#define ES8388_ADDR  0x10  //ES8388的器件地址

u8 es8388_init(void);
u8 es8388_write_reg(u8 addr, u8 txd);
u8 es8388_read_reg(u8 addr);
void es8388_i2s_config(u8 fmt, u8 len);
void es8388_headphone_vol_config(u8 volume);
void es8388_speaker_vol_config(u8 volume);
void es8388_3d_config(u8 depth);
void es8388_adda_config(u8 dac_en, u8 adc_en);
void es8388_output_config(u8 o1_en, u8 o2_en);
void es8388_mic_gain_config(u8 gain);
void es8388_alc_control(u8 sel, u8 gain_max, u8 gain_min);
void es8388_input_config(u8 in);

#endif
