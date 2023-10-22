#ifndef __SLEEP_H__
#define __SLEEP_H__

#include "cw32_fwlib.h"

//---------------------------------------------------------------
//

#define TICK_INC_1MS    1000ul  // 1ms
#define TICK_INC_100US  100ul   // 0.1ms

#define CONFIG_TICK_INC TICK_INC_1MS

//---------------------------------------------------------------
//

typedef enum {
    UNIT_US = 1ul,
    UNIT_MS = 1000ul,
    UNIT_S  = 1000000ul,
} DelayUnit_e;

typedef u32 tick_t;

inline void DelayInit(void);
inline void DelayBlock(tick_t nWaitTime);
inline bool DelayNonBlock(tick_t nStartTick, tick_t nWaitTime);

inline tick_t HAL_GetTick(void);
inline tick_t HAL_DeltaTick(tick_t nStartTick, tick_t nEndTick);

#define DelayBlockUS(t)       DelayBlock((t) * (UNIT_US) / CONFIG_TICK_INC)
#define DelayNonBlockUS(s, t) DelayNonBlock(s, (t) * (UNIT_US) / CONFIG_TICK_INC)

#define DelayBlockMS(t)       DelayBlock((t) * (UNIT_MS) / CONFIG_TICK_INC)
#define DelayNonBlockMS(s, t) DelayNonBlock(s, (t) * (UNIT_MS) / CONFIG_TICK_INC)

#define DelayBlockS(t)        DelayBlock((t) * (UNIT_S) / CONFIG_TICK_INC)
#define DelayNonBlockS(s, t)  DelayNonBlock(s, (t) * (UNIT_S) / CONFIG_TICK_INC)

//---------------------------------------------------------------
// time measure

#define TID_0                 0
#define TID_1                 1
#define TID_2                 2
#define TID_3                 3
#define TID_4                 4
#define TID_5                 5
#define TID_6                 6
#define TID_7                 7
#define TID_8                 8
#define TID_9                 9

bool TimeRecStart(u8 id);
u32  TimeRecEnd(u8 id);

#endif
