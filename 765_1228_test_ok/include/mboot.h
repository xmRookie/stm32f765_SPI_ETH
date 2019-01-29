#ifndef __M_BOOT_H__
#define __M_BOOT_H__


extern void SetSwitchFlag(void);

extern void TimeOutToChkSW(void);

extern uint8_t DownloadProgram(STR_PECRPORTOCOL *lan);

extern void SetUpdataFlag(void);

extern void ReceiveAGotoUpd(void);

#endif//__M_BOOT_H__
