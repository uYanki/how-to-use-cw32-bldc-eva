/* ----------------------- System includes --------------------------------- */
#include "port.h"

/* ----------------------- Modbus includes --------------------------------- */
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ----------------------------------------- */

/* ----------------------- Static variables -------------------------------- */

/* ----------------------- Start implementation ---------------------------- */

static tick_t tMbTimeout = CONFIG_TICK_INC;

BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
    // usTim1Timerout50us: 时基 50us

#if CONFIG_TICK_INC == TICK_INC_1MS
#error "unsupported tick !!!"
    // tMbTimeout = (tick_t)usTim1Timerout50us * 50;  // 时基 1000us
#elif CONFIG_TICK_INC == TICK_INC_100US
    tMbTimeout = (tick_t)usTim1Timerout50us * 50;  // 时基 100us
#else
#error "unsupported tick !!!"
#endif


    return TRUE;
}

inline void vMBPortTimersEnable()
{
}

inline void vMBPortTimersDisable()
{
}

// call in SysTick_Handler in cw32f030_systick.c
// or call in while(1) in main.c
void ecbMbTick(void)
{
    static u16 u16LastCount = 0;

    // frame size
    u16 u16CurCount = UART_RX_DMA_BUFSIZE - (UART_RX_DMA_CH->CNT & 0x7FFF);

    // start bit + databits + stop bit = 10 bit, 3.5 byte * 10 = 35 bit

    // 100 us -> 1 tick
    // 1750 us -> 17.5 tick ≈ 18 tick
    // 35 bit * 1s / 9600 bps -> 3645.833 us -> 36.458333 tick ≈ 37 tick
    // 35 bit * 1s / 115200 bps -> 303.8194 us -> 3.038194 tick ≈ 3 tick

    static tick_t tFrameRecv = 0;

    if (u16CurCount > 0)
    {
        if (u16LastCount == u16CurCount && tFrameRecv != 0)
        {
            if (DelayNonBlockUS(tFrameRecv, tMbTimeout))  // tMbTimeout
            {
                // disable RX DMA
                // UartDmaRx(nullptr, 0);

                // recvice frame
                extern void ecbMbUartRx(RO u8 * buffer, u16 length);
                ecbMbUartRx(UART_RX_DMA_BUFADDR, u16CurCount);

                // enable RX DMA
                UartDmaRx(UART_RX_DMA_BUFADDR, UART_RX_DMA_BUFSIZE);
            }
        }
        else
        {
            tFrameRecv   = HAL_GetTick();
            u16LastCount = u16CurCount;
        }
    }
}
