
/* ----------------------- System includes --------------------------------- */
#include "port.h"

/* ----------------------- Modbus includes --------------------------------- */
#include "mb.h"
#include "mbport.h"
#include "mbrtu.h"

/* ----------------------- Defines ----------------------------------------- */

/* ----------------------- Static variables -------------------------------- */

/* ----------------------- Start implementation ---------------------------- */

// 256 (frame) = 1 (id) + 1 (func) + 254(data) + 2 (crc)

u8 sUartRxbuf[UART_RX_DMA_BUFSIZE] = {0};

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    if (xRxEnable)
    {
        // RS485_SetRxDir();

        UartDmaRx(UART_RX_DMA_BUFADDR, UART_RX_DMA_BUFSIZE);
    }

    if (xTxEnable)
    {
        // RS485_SetTxDir();
    }
}

BOOL xMBPortSerialInit(UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
    // ucPort is set by eMBInit

    if (ucDataBits != 8)
    {
        return FALSE;
    }

    USART_InitTypeDef USART_InitStruct = {
        .USART_Over                = USART_Over_16,
        .USART_Source              = USART_Source_PCLK,
        .USART_UclkFreq            = CONFIG_SYSCLK_FREQ,
        .USART_StartBit            = USART_StartBit_FE,
        .USART_StopBits            = USART_StopBits_1,
        .USART_Mode                = USART_Mode_Tx | USART_Mode_Rx,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
    };

    if (ucPort == 1)
    {
        // 当设置为无校验时, 数据字长为8位
        // 当设置为有校验时, 数据字长自动设置为9位 (该数据字长包括了校验位)

        USART_InitStruct.USART_BaudRate = ulBaudRate;

        switch (eParity)
        {
            case MB_PAR_NONE: USART_InitStruct.USART_Parity = USART_Parity_No; break;
            case MB_PAR_EVEN: USART_InitStruct.USART_Parity = USART_Parity_Even; break;
            case MB_PAR_ODD: USART_InitStruct.USART_Parity = USART_Parity_Odd; break;
            default: return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    UartConfig(&USART_InitStruct);

    return TRUE;
}
