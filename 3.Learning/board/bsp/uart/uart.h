#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "cw32_fwlib.h"

void UartInit(void);
void UartConfig(USART_InitTypeDef* USART_InitStruct);

void UartDmaTx(u8* addr, u16 size);

#endif
