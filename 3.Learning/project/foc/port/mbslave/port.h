#ifndef _PORT_H
#define _PORT_H

#include "board.h"

#define PR_BEGIN_EXTERN_C extern "C" {
#define PR_END_EXTERN_C   }

#define ENTER_CRITICAL_SECTION()
#define EXIT_CRITICAL_SECTION()

typedef char BOOL;

typedef u8 UCHAR;
typedef s8 CHAR;

typedef u16 USHORT;
typedef s16 SHORT;

typedef u32 ULONG;
typedef s32 LONG;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// Rx DMA
#define UART_RX_DMA_BUFADDR sUartRxbuf
#define UART_RX_DMA_BUFSIZE 256u
extern u8 sUartRxbuf[UART_RX_DMA_BUFSIZE];

#endif
