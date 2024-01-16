#include "gconf.h"
#include "usart.h"
#include "bsp_uart.h"

void Uart_SetWorkDir(uart_dir_e dir)
{
    switch (dir)
    {
        case UART_DIR_TX: {
#if CONFIG_UART_MODE == UART_MODE_RS485
            DelayBlockUS(CONFIG_RS485_SWITCH_TIME_US * 10);
            HAL_GPIO_WritePin(RS485_RTS_GPIO_Port, RS485_RTS_Pin, GPIO_PIN_RESET);
            DelayBlockUS(CONFIG_RS485_SWITCH_TIME_US);
#endif
            break;
        }
        case UART_DIR_RX: {
#if CONFIG_UART_MODE == UART_MODE_RS485
            DelayBlockUS(CONFIG_RS485_SWITCH_TIME_US);
            HAL_GPIO_WritePin(RS485_RTS_GPIO_Port, RS485_RTS_Pin, GPIO_PIN_SET);
            DelayBlockUS(CONFIG_RS485_SWITCH_TIME_US);
#endif
            break;
        }
    }
}

