#include "sleep.h"

// M0+ don't have DWT

//---------------------------------------------------------------

void DelayInit(void)
{}

void DelayBlock(tick_t nWaitTick)  // 若要在中断里使用，需把优先级调高
{
    tick_t nStartTick = DelayGetTick();

    while (DeltaTick(nStartTick, DelayGetTick()) < nWaitTick)
        ;
}

bool DelayNonBlock(tick_t nStartTick, tick_t nWaitTick)  // 可在中断里使用
{
    return DeltaTick(nStartTick, DelayGetTick()) >= nWaitTick;
}

tick_t DelayGetTick(void)
{
#ifdef USE_HAL_DRIVER
    // SysTick is Count Down Mode
    uint32_t us = 1000 - 1000 * SysTick->VAL / SysTick->LOAD;
    uint32_t ms = HAL_GetTick() * 1000;
    return ms + us;
#else
    #error "Required a TIM to provide a clock source !!!"
#endif
}

tick_t DeltaTick(tick_t nStartTick, tick_t nEndTick)
{
    if (nEndTick >= nStartTick)
    {
        return nEndTick - nStartTick;
    }
    else
    {
        return TICK_MAX - nStartTick + nEndTick;
    }
}

//---------------------------------------------------------------
// time measure

static tick_t saMeasureTime[UINT8_MAX] = {0};

/**
 * @param id 0~255
 */
void TimeRecord(uint8_t id)
{
    saMeasureTime[id] = DelayGetTick();
}

/**
 * @return unit: us 
 */
tick_t TimeInterval(uint8_t id)
{
    return DeltaTick(saMeasureTime[id], DelayGetTick());
}
