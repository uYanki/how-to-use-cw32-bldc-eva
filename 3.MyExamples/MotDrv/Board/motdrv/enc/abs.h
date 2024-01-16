#ifndef __ABS_ENC_H__
#define __ABS_ENC_H__

#include "main.h"
#include "defs.h"
#include "gconf.h"
#include "usart.h"

#if CONFIG_ENCODER_TYPE == ENC_ABS

void     tformat_send_command(uint8_t txbuf[], uint16_t txcnt, uint8_t rxbuf[], uint16_t rxcnt);
void     tformat_send_clear_position(void);
void     tformat_send_read_request(void);
uint16_t tformat_process_response(void);

#endif

#endif
