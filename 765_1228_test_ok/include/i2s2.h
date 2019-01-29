
#ifndef __I2S2_H__
#define __I2S2_H__

extern void MX_I2S2_Init(void);

extern void I2S2_RxCpltCallback(void);

extern void I2S2_M1RxCpltCallback(void);

extern void pollint_i2s_slave(void);

extern void pollint_Audio_TS(void);


extern uint8_t Get_i2s_Callback_Flag(void);

extern void Reset_i2s_Callback_Flag(void);

#endif //__I2S2_H__
