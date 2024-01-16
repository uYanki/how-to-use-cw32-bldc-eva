#include "abs.h"

#if CONFIG_ENCODER_TYPE == ENC_ABS

static uint8_t TformatRxBuf[6] = {0};

void tformat_send_command(uint8_t txbuf[], uint16_t txcnt, uint8_t rxbuf[], uint16_t rxcnt)
{
    HAL_GPIO_WritePin(RS485_RTS2_GPIO_Port, RS485_RTS2_Pin, GPIO_PIN_SET);  // tx
    HAL_UART_Transmit(&UART_TFORMAT, txbuf, txcnt, 0xFF);
    HAL_GPIO_WritePin(RS485_RTS2_GPIO_Port, RS485_RTS2_Pin, GPIO_PIN_RESET);  // rx
    HAL_UART_Receive_DMA(&UART_TFORMAT, rxbuf, rxcnt);
}

void tformat_send_clear_position(void)
{
    uint8_t TformatTxBuf[1] = {0xAA};
    tformat_send_command(TformatTxBuf, ARRAY_SIZE(TformatTxBuf), nullptr, 0);
}

void tformat_send_read_request(void)
{
    uint8_t TformatTxBuf[1] = {0x02};
    tformat_send_command(TformatTxBuf, ARRAY_SIZE(TformatTxBuf), TformatRxBuf, ARRAY_SIZE(TformatRxBuf));
}

uint16_t tformat_process_response(void)
{
    uint16_t angle = 0;

    ///< @note: The result of bitwise XOR of the same number is 0
    uint16_t crc = TformatRxBuf[0] ^ TformatRxBuf[1] ^ TformatRxBuf[2] ^ TformatRxBuf[3] ^ TformatRxBuf[4] ^ TformatRxBuf[5];

    if (crc == 0)
    {
        angle = (TformatRxBuf[4] << 16) + (TformatRxBuf[3] << 8) + TformatRxBuf[2];
        angle >>= 2;
    }

    return angle;
}

#if 0

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart == &UART_TFORMAT)
    {
        tformat_process_response();
    }
}

#endif

#endif