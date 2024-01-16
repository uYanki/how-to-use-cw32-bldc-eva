#ifndef __SLEEP_H__
#define __SLEEP_H__

#include "cw32_fwlib.h"

//---------------------------------------------------------------
//

typedef enum {
    UNIT_US = 1ul,
    UNIT_MS = 1000ul,
    UNIT_S  = 1000000ul,
} DelayUnit_e;

#if 1
typedef u32 tick_t;
#define TICK_MAX (UINT32_MAX)
#else
typedef u64 tick_t;
#define TICK_MAX (UINT64_MAX)
#endif

#define TICK_UNIT_US 1e2

inline void DelayInit(void);
inline void DelayBlock(tick_t nWaitTime);
inline bool DelayNonBlock(tick_t nStartTick, tick_t nWaitTime);

inline tick_t DelayGetTick(void);
inline tick_t DelayCalcDelta(tick_t nStartTick, tick_t nEndTick);

#define DelayBlockUS(t)       DelayBlock((t) * (UNIT_US) / (TICK_UNIT_US))
#define DelayNonBlockUS(s, t) DelayNonBlock(s, (t) * (UNIT_US) / (TICK_UNIT_US))

#define DelayBlockMS(t)       DelayBlock((t) * (UNIT_MS) / (TICK_UNIT_US))
#define DelayNonBlockMS(s, t) DelayNonBlock(s, (t) * (UNIT_MS) / (TICK_UNIT_US))

#define DelayBlockS(t)        DelayBlock((t) * (UNIT_S) / (TICK_UNIT_US))
#define DelayNonBlockS(s, t)  DelayNonBlock(s, (t) * (UNIT_S) / (TICK_UNIT_US))

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

bool   TimeRecStart(u8 id);
tick_t TimeRecEnd(u8 id);

#endif
