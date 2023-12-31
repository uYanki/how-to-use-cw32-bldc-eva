#include "sleep.h"

//---------------------------------------------------------------
//

static volatile tick_t m_ticks = 0;

void DelayInit(void)
{
    if (SysTick_Config(SystemCoreClock / (1e6 / TICK_UNIT_US)) == 0U)
    {
        NVIC_SetPriority(SysTick_IRQn, 0);
    }
}

void DelayBlock(tick_t nWaitTime)
{
    tick_t nStartTick = HAL_GetTick();

    while ((HAL_GetTick() - nStartTick) < nWaitTime)
        ;
}

bool DelayNonBlock(tick_t nStartTick, tick_t nWaitTime)
{
    return HAL_GetTick() >= (nStartTick + nWaitTime);
}

tick_t HAL_GetTick(void)
{
    return m_ticks;
}

tick_t HAL_DeltaTick(tick_t nStartTick, tick_t nEndTick)
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

/**
 * @note call HAL_IncTick() in SysTick_Handler()
 *
 *      extern void HAL_IncTick(void);
 *      HAL_IncTick();
 *
 */
__weak void HAL_IncTick(void)
{
    ++m_ticks;
}

//---------------------------------------------------------------
//

void FirewareDelay(u32 nWaitTime)
{
    u8 n;
    while (nWaitTime--)
    {
        n = UINT8_MAX;
        while (n--)
        {
        }
    }
}

//---------------------------------------------------------------
// time measure

static tick_t saMeasureTime[10] = {0};

bool TimeRecStart(u8 id)
{
    if (id < ARRAY_SIZE(saMeasureTime))
    {
        saMeasureTime[id] = HAL_GetTick();
        return true;
    }
    return false;
}

tick_t TimeRecEnd(u8 id)
{
    if (id < ARRAY_SIZE(saMeasureTime))
    {
        // unit = TICK_PSC * 0.01us
        return 1e2 * 1e6 * (HAL_GetTick() - saMeasureTime[id]) / SystemCoreClock;
    }
    return TICK_MAX;
}
