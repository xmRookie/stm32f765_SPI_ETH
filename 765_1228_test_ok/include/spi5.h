
#ifndef __SPI5_H__
#define __SPI5_H__

extern void MX_SPI5_Init(void);
extern void SPI5_RxCpltCallback(void);
extern void SPI5_RxHalfCpltCallback(void);

extern void SPI5_M1RxCpltCallback(void);

extern void pollint_spi_slave(void);

extern void Client_flow_state(uint8_t state);

extern void En_RX_Flow_Ctrl(void);

extern uint8_t Get_Spi_Lock(void);

#endif //__SPI5_H__
