#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "cw32_fwlib.h"

void UartInit(void);
void UartConfig(USART_InitTypeDef* USART_InitStruct);

void UartDmaTx(__IN u8* bufaddr, u16 bufsize);
void UartDmaRx(__OUT u8* bufaddr, u16 bufsize);

#endif
