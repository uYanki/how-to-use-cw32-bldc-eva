#include "main.h"
#include "pinmap.h"

void bsp_gpio_init(void);

int main()
{
    bsp_gpio_init();

    for (;;) {
        // key - led
        GPIO_WritePin(PIN_LED1, GPIO_ReadPin(PIN_KEY1));
        GPIO_WritePin(PIN_LED2, GPIO_ReadPin(PIN_KEY2));
        GPIO_WritePin(PIN_LED3, GPIO_ReadPin(PIN_KEY3));
        // key - buzzer
        GPIO_WritePin(PIN_BUZ, (GPIO_PinState)(GPIO_ReadPin(PIN_KEY2) == GPIO_Pin_RESET));
    }
}

void bsp_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __RCC_GPIOA_CLK_ENABLE();
    __RCC_GPIOB_CLK_ENABLE();
    __RCC_GPIOC_CLK_ENABLE();

    // gpio

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.IT   = GPIO_IT_NONE;
    GPIO_InitStruct.Pins = GPIO_SPEED_HIGH;

    GPIO_InitStruct.Pins = GPIO_PIN_11;
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pins = GPIO_PIN_11;
    GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pins = GPIO_PIN_13;
    GPIO_Init(CW_GPIOC, &GPIO_InitStruct);

    // key

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;

    GPIO_InitStruct.Pins = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10;
    GPIO_Init(CW_GPIOB, &GPIO_InitStruct);

    // buzzer

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pins = GPIO_PIN_12;
    GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @return None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
