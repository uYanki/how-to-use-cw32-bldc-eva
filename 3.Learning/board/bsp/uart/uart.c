#include "uart.h"

#include "board_conf.h"
#include "pinmap.h"

#include "port.h"

void UartInit(void)
{
    // GPIO
    {
        GPIO_InitTypeDef GPIO_InitStruct = {
            .IT    = GPIO_IT_NONE,
            .Speed = GPIO_SPEED_HIGH,
        };

        UART_CLKEN(UART_CLK, ENABLE);
        UART_TX_GPIO_CLKEN(UART_TX_GPIO_CLK, ENABLE);
        UART_RX_GPIO_CLKEN(UART_RX_GPIO_CLK, ENABLE);

        UART_TX_GPIO_AF();
        UART_RX_GPIO_AF();

        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pins = UART_TX_GPIO_PIN;  // tx
        GPIO_Init(UART_TX_GPIO_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
        GPIO_InitStruct.Pins = UART_RX_GPIO_PIN;  // tx
        GPIO_Init(UART_RX_GPIO_PORT, &GPIO_InitStruct);
    }
    // DMA
    {
        RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_DMA, ENABLE);

        // TX
        {
            DMA_InitTypeDef DMA_InitStructure = {
                .DMA_Mode          = DMA_MODE_BLOCK,
                .TrigMode          = DMA_HardTrig,
                .HardTrigSource    = UART_TX_DMA_TRIG,
                .DMA_TransferCnt   = 0,
                .DMA_TransferWidth = DMA_TRANSFER_WIDTH_8BIT,
                .DMA_DstInc        = DMA_DstAddress_Fix,
                .DMA_DstAddress    = (u32)&UART_PORT->TDR,
                .DMA_SrcInc        = DMA_SrcAddress_Increase,
                .DMA_SrcAddress    = 0,
            };

            DMA_Init(UART_TX_DMA_CH, &DMA_InitStructure);
        }

        // RX
        {
            DMA_InitTypeDef DMA_InitStructure = {
                .DMA_Mode          = DMA_MODE_BLOCK,
                .DMA_TransferWidth = DMA_TRANSFER_WIDTH_8BIT,
                .TrigMode          = DMA_HardTrig,
                .HardTrigSource    = UART_RX_DMA_TRIG,
                .DMA_TransferCnt   = 0,
                .DMA_SrcInc        = DMA_SrcAddress_Fix,
                .DMA_SrcAddress    = (u32)&UART_PORT->RDR,
                .DMA_DstInc        = DMA_DstAddress_Increase,
                .DMA_DstAddress    = 0,
            };

            DMA_Init(UART_RX_DMA_CH, &DMA_InitStructure);
        }
    }
    // nvic
    {
        __disable_irq();
        NVIC_EnableIRQ(UART_TX_DMA_IRQn);
        __enable_irq();

        // 对应的中断函数在 interrupts_cw32f030.c 中
        DMA_ITConfig(UART_TX_DMA_CH, DMA_IT_TC, ENABLE);
    }
}

void UartConfig(USART_InitTypeDef* USART_InitStruct)
{
#if 0
    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate            = CONFIG_DEBUG_UART_BAUD;
    USART_InitStruct.USART_Over                = USART_Over_16;
    USART_InitStruct.USART_Source              = USART_Source_PCLK;
    USART_InitStruct.USART_UclkFreq            = CONFIG_SYSCLK_FREQ;
    USART_InitStruct.USART_StartBit            = USART_StartBit_FE;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
#endif
    USART_Init(UART_PORT, USART_InitStruct);
}

void UartDmaTx(__IN u8* bufaddr, u16 bufsize)
{
    if (bufaddr == nullptr || bufsize == 0)
    {
        return;
    }

    UART_TX_DMA_CH->CNT     = bv16 | bufsize;
    UART_TX_DMA_CH->SRCADDR = (u32)bufaddr;

    USART_DMACmd(UART_PORT, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(UART_TX_DMA_CH, ENABLE);
}

void UartDmaRx(__OUT u8* bufaddr, u16 bufsize)
{
    if (bufaddr == nullptr || bufsize == 0)
    {
        DMA_Cmd(UART_RX_DMA_CH, DISABLE);
        USART_DMACmd(UART_PORT, USART_DMAReq_Rx, DISABLE);
        return;
    }

    UART_RX_DMA_CH->CNT     = bv16 | bufsize;  // UART_RX_DMA_BUFSIZE;
    UART_RX_DMA_CH->DSTADDR = (u32)bufaddr;    // UART_RX_DMA_BUFADDR;

    DMA_Cmd(UART_RX_DMA_CH, ENABLE);
    USART_DMACmd(UART_PORT, USART_DMAReq_Rx, ENABLE);
}

#if CONFIG_REDIRECT_PRINTF

int fputc(int ch, FILE* f)
{
    if (ch == '\n')
    {
        USART_SendData_8bit(UART_PORT, '\r');
        while (USART_GetFlagStatus(UART_PORT, USART_FLAG_TXE) == RESET) {}
    }

    USART_SendData_8bit(UART_PORT, (uint8_t)ch);
    while (USART_GetFlagStatus(UART_PORT, USART_FLAG_TXE) == RESET) {}

    return ch;
}

#endif
