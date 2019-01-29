
#ifndef __SPI1_H__
#define __SPI1_H__

extern void MX_SPI1_Init(void);

extern void SPI1_TxCpltCallback(void);

extern void SPI1_RxCpltCallback(void);

extern void SPI1_Flow_deal(void);

extern void send_spi_dat(void);

extern void En_flow_ctrl(void);

extern void setenPollint(uint8_t en);

extern uint8_t getenPollint(void);


extern void Write_GPIOA_Pin4(uint8_t Status);

extern void SPI1_Communicate(void );
#endif //__SPI1_H__
