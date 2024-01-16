
#include "gconf.h"

#include "usart.h"

#include "bsp_uart.h"
#include "system/sleep.h"

// clang-format off

#if CONFIG_REDIRECT_PRINTF_SW

#if defined(__CC_ARM) || 1

// auto switch uart direction when call printf

#include <stdarg.h>
#include <stdio.h>

int $Super$$__2printf(const char* fmt, ...);

// precall
int $Sub$$__2printf(const char* fmt, ...)
{
    int len;
	
    // set tx dir
    Uart_SetWorkDir(UART_DIR_TX);

    // $Super$$__2printf("<*>");

    va_list args;
    va_start(args, fmt);
    len = vprintf(fmt, args);  // print
    va_end(args);

    // set rx dir
	  Uart_SetWorkDir(UART_DIR_RX);

    return len;
}

#endif

int fputc(int ch, FILE* f)
{
    if (ch == '\n')
    {
        HAL_UART_Transmit(&CONFIG_REDIRECT_UART_PORT, (uint8_t*) "\r", 1, HAL_MAX_DELAY);
    }

    HAL_UART_Transmit(&CONFIG_REDIRECT_UART_PORT, (uint8_t*) &ch, 1, HAL_MAX_DELAY);

    return (ch);
}

__asm(".global __use_no_semihosting\n\t");

void _sys_exit(int x)
{
    x = x;
}

//__use_no_semihosting was requested, but _ttywrch was

void _ttywrch(int ch)
{
    ch = ch;
}

FILE __stdout;

#endif

#ifndef NDEBUG

#include <stdlib.h>
#include <stdio.h>

int fputs(const char* str, FILE* stream)
{
    printf("%s", str);
    return 0;

    // if error
    // return EOF;
}

__attribute__((weak, noreturn)) 
void __aeabi_assert(const char* expr, const char* file, int line)
{
    char str[12], *p;

    fputs("*** assertion failed: ", stderr);
    fputs(expr, stderr);
    fputs(", file ", stderr);
    fputs(file, stderr);
    fputs(", line ", stderr);

    p    = str + sizeof(str);
    *--p = '\0';
    *--p = '\n';
    while (line > 0)
    {
        *--p = '0' + (line % 10);
        line /= 10;
    }
    fputs(p, stderr);

    abort();
}

__attribute__((weak))
void abort(void)
{
    for (;;)
        ;
}

#endif

// clang-format on
