#ifndef __INC_POLLINT_H__
#define __INC_POLLINT_H__
//----------------------------------------------------------------------------
extern void MX_TIM3_Init(void);
extern void Timer3_irq_fun(void);
extern void TimerTickNproc(void);
extern void pollintTim3(void);
extern void delayms_Lock(uint16_t value);
extern void pollint(void);
extern void delay_NoLock(uint16_t value);
//-----------------------------------------------------------------------------
#endif
