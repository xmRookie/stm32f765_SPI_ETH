
#ifndef __USART2_UART_H__
#define __USART2_UART_H__

extern void MX_USART2_UART_Init(void);

extern void USART2_Rx_Deal(void );

extern void USART2_Tx_Deal(void );

extern void USART2_Putbuf(uint8_t const *buf, uint16_t len);

extern void USART2_Putchar(uint8_t ch);

extern void USART2_RxReaddy(void);

static uint16_t USART2_GetRxBufDat(uint8_t *buf);

extern void  pollint_spi_transmit(void );

#endif
