#include "sleep.h"
#include "board_conf.h"

//---------------------------------------------------------------
//

void DelayInit(void)
{
    uwTickFreq = TICK_FREQ_1KHZ;

#if CONFIG_TICK_INC == TICK_INC_1MS
    InitTick(CONFIG_SYSCLK_FREQ / 1ul);
#elif CONFIG_TICK_INC == TICK_INC_100US
    InitTick(CONFIG_SYSCLK_FREQ / 10ul);
#else
#error "unsupported tick increment"
#endif
}

void DelayBlock(tick_t nWaitTime)
{
    assert(nWaitTime);
    SysTickDelay(nWaitTime);
}

bool DelayNonBlock(tick_t nStartTick, tick_t nWaitTime)
{
    assert(nWaitTime);
    return HAL_GetTick() >= (nStartTick + nWaitTime);
}

tick_t HAL_GetTick(void)
{
    return GetTick();
}

tick_t HAL_DeltaTick(tick_t nStartTick, tick_t nEndTick)
{
    if (nEndTick >= nStartTick)
    {
        return nEndTick - nStartTick;
    }
    else
    {
        return MAX_SYSTICK_DELAY - nStartTick + nEndTick;
    }
}

//---------------------------------------------------------------
// time measure

static tick_t saMeasureTime[10] = {0};

bool TimeRecStart(u8 id)
{
    if (id < ARRAY_SIZE(saMeasureTime))
    {
        saMeasureTime[id] = GetTick();
        return true;
    }
    return false;
}

u32 TimeRecEnd(u8 id)
{
    if (id < ARRAY_SIZE(saMeasureTime))
    {
        return CONFIG_TICK_INC * 1e6 * (HAL_GetTick() - saMeasureTime[id]) / SystemCoreClock;  // us
    }
    return U32_MAX;
}
