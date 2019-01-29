#include "common.h"

#define EVENTBUFLEN     32

#define READ_EVENT          0
#define WRITE_EVENT         1
#define CLEAR_EVENT         0xff

static void EventRdWri(uint8_t cmd, STR_EVENT *evt)
{
    static uint8_t eventcount;
    static uint8_t eventrdpos;
    static uint8_t eventbuffer[EVENTBUFLEN];
    static uint8_t parabuffer[EVENTBUFLEN];

    switch(cmd) {
    case READ_EVENT:                  // 读事件
        evt->evt = evNON;
        evt->par = 0xff;

        if(eventcount) {
            // 有事件
            evt->evt = eventbuffer[eventrdpos];
            evt->par = parabuffer[eventrdpos];
            eventrdpos = (eventrdpos + 1) % EVENTBUFLEN;
            eventcount--;
        }

        break;

    case WRITE_EVENT:
        if(eventcount < EVENTBUFLEN) {
            uint8_t wripos;

            wripos = (eventrdpos + eventcount) % EVENTBUFLEN;
            eventbuffer[wripos] = evt->evt;
            parabuffer[wripos] = evt->par;
            eventcount++;
        }

        break;

    default:
        eventcount = 0;
        eventrdpos = 0;
        break;
    }
}

// 清空所有事件
extern void ClearAllEvent(void)
{
    EventRdWri(CLEAR_EVENT, NULL);
}

// 读取事件
extern void ReadEvent(STR_EVENT *evt)
{
    EventRdWri(READ_EVENT, evt);
}

// 保存事件，事件和参数在结构体中
extern void WriteEvent(STR_EVENT *evt)
{
    EventRdWri(WRITE_EVENT, evt);
}

// 保存事件，直接传入事件和参数
extern void PushEvt_EvtPar(uint8_t event, uint8_t para)
{
    STR_EVENT evt;

    evt.par = para;
    evt.evt = event;
    WriteEvent(&evt);
}

// 保存事件，没有参数
extern void PushEvt_OnlyEvt(uint8_t event)
{
    PushEvt_EvtPar(event, 0xff);
}
