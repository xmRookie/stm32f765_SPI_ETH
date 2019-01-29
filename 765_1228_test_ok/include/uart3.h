
#ifndef __UART3_UART_H__
#define __UART3_UART_H__

extern void MX_USART3_UART_Init(void);

extern void USART3_Putchar(uint8_t ch);

extern void USART3_Putbuf(uint8_t  *buf, uint16_t len);

extern void USART3_RxReaddy(void);

extern void USART3_CheckSerial(void);

extern void USART3_IRQ_Deal(void);
#endif
