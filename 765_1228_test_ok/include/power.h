
#ifndef __POWER_H
#define __POWER_H
  /* Includes ------------------------------------------------------------------*/

extern void SetWorkMode(uint8_t mode);

extern uint8_t GetWorkMode(void);

extern void powerReset(STR_EVENT const *event);

#endif /* __POWER_H */

