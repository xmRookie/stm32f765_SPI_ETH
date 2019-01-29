#ifndef __INC_EVENTMANAGE_H__
#define __INC_EVENTMANAGE_H__
	
typedef struct
{
    uint16_t     par;                // 事件参数
    uint8_t     evt;                // 事件
} STR_EVENT;                  // 定义事件结构

//-----------------------------------------------------------------------------
extern void ClearAllEvent(void);
extern void ReadEvent(STR_EVENT *evt);
extern void WriteEvent(STR_EVENT *evt);
extern void PushEvt_EvtPar(uint8_t event, uint8_t para);
extern void PushEvt_OnlyEvt(uint8_t event);
//-----------------------------------------------------------------------------
#endif
