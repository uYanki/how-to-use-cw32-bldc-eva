#ifndef __BOARD_CONF_H__
#define __BOARD_CONF_H__

#define WEAK                    __attribute__((weak))

// #define CONFIG_SYSCLK_FREQ 8000000
#define CONFIG_SYSCLK_FREQ      64000000

///< debug uart

#define CONFIG_USING_DEBUG_UART 1

#if CONFIG_USING_DEBUG_UART

#define CONFIG_DEBUG_UART_BASE CW_UART1
#define CONFIG_DEBUG_UART_BAUD 115200

#define CONFIG_REDIRECT_PRINTF 1

#endif

///< n ms ticker

#define CONFIG_USING_DEBUG_TICKER 1

#if CONFIG_USING_DEBUG_TICKER

#define CONFIG_DEBUG_TICKER_PERIOD_N_MS 5  // 1~500

#endif

#endif
