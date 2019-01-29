#ifndef __MP2_H__
#define __MP2_H__

extern void MP2_init(void);

int MP2_Encode(short *buf,int len);

void reset_MP2_encode(void);

void pollint_Audio_TS(void);


#endif  /*__MP2_H__ */
