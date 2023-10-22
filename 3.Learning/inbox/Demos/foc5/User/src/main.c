
#include "main.h"
#include "motor.h"

int main()
{
    board_init();

    InitTick(CONFIG_SYSCLK_FREQ);


}

void BTIM1_IRQCallBack()
{
    static f32 i = 0;
    setPhaseVoltage(5, 0, i += 0.01);
    ATIM_CtrlPWMOutputs(ENABLE);
    GPIO_TogglePin(PIN_LED1);
}

void GTIM2_IRQHandler(void)
{
    static RO u8 steps[6] = {1, 3, 2, 5, 0, 4};

    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC1);
    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC2);
    GTIM_ClearITPendingBit(CW_GTIM2, GTIM_IT_CC3);

    // ++nHallTotalCnt;

    u8 x = 0;
    if (PA15_GETVALUE()) x |= 0x1;
    if (PB03_GETVALUE()) x |= 0x2;
    if (PA02_GETVALUE()) x |= 0x4;
}

static void LED_Configuration(void)
{
    {
        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();
        __RCC_GPIOC_CLK_ENABLE();
    }
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStruct.Pins = GPIO_PIN_13;  // LED1
        GPIO_Init(CW_GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_11;  // LED2
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pins = GPIO_PIN_11;  // LED3
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
    }
}

static void PWM_Configuration(void)
{
    {
        __RCC_ATIM_CLK_ENABLE();
        __RCC_GPIOA_CLK_ENABLE();
        __RCC_GPIOB_CLK_ENABLE();

        PA08_AFx_ATIMCH1A();  // UH
        PA09_AFx_ATIMCH2A();  // VH
        PA10_AFx_ATIMCH3A();  // WH

        PB13_AFx_ATIMCH1B();  // UL
        PB14_AFx_ATIMCH2B();  // VL
        PB15_AFx_ATIMCH3B();  // WL
    }
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.IT    = GPIO_IT_NONE;
        GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

        GPIO_InitStruct.Pins = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;    // 上管
        GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
        GPIO_WritePin(CW_GPIOA, GPIO_InitStruct.Pins, GPIO_Pin_RESET);   // OFF

        GPIO_InitStruct.Pins = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;  // 下管
        GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
        GPIO_WritePin(CW_GPIOB, GPIO_InitStruct.Pins, GPIO_Pin_SET);     // OFF
    }
    {
        ATIM_InitTypeDef ATIM_InitStruct;

        /**
         * @note FREQ = ( CONFIG_SYSCLK_FREQ / ATIM_Prescaler ) / ( ReloadValue + 1 )
         *
         *    -->  freq = ( 64e6 / 64 ) / 1e2 = 1e4 = 5KHz
         *
         *      T = 2 / freq
         */

        ATIM_InitStruct.CounterDirection   = ATIM_COUNTING_UP;
        ATIM_InitStruct.CounterOPMode      = ATIM_OP_MODE_REPETITIVE;
        ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_CENTER_ALIGN;
        ATIM_InitStruct.BufferState        = DISABLE;
        ATIM_InitStruct.ClockSelect        = ATIM_CLOCK_PCLK;
        ATIM_InitStruct.RepetitionCounter  = ATIM_OP_MODE_REPETITIVE;
        ATIM_InitStruct.UnderFlowMask      = DISABLE;
        ATIM_InitStruct.OverFlowMask       = DISABLE;
        ATIM_InitStruct.Prescaler          = ATIM_Prescaler_DIV64;
        ATIM_InitStruct.ReloadValue        = PWM_RELOAD - 1;

        ATIM_Init(&ATIM_InitStruct);
    }
    {
        ATIM_OCInitTypeDef ATIM_OCInitStruct;

        ATIM_OCInitStruct.OCPolarity        = ATIM_OCPOLARITY_NONINVERT;
        ATIM_OCInitStruct.OCMode            = ATIM_OCMODE_PWM1;
        ATIM_OCInitStruct.OCInterruptState  = DISABLE;
        ATIM_OCInitStruct.OCInterruptSelect = ATIM_OC_IT_NONE;
        ATIM_OCInitStruct.OCDMAState        = DISABLE;
        ATIM_OCInitStruct.BufferState       = DISABLE;

        ATIM_OC1AInit(&ATIM_OCInitStruct);
        ATIM_OC2AInit(&ATIM_OCInitStruct);
        ATIM_OC3AInit(&ATIM_OCInitStruct);
        ATIM_OC1BInit(&ATIM_OCInitStruct);
        ATIM_OC2BInit(&ATIM_OCInitStruct);
        ATIM_OC3BInit(&ATIM_OCInitStruct);

        ATIM_PWMOutputConfig(OCREFA_TYPE_SINGLE, OUTPUT_TYPE_COMP, 1);

        ATIM_SetCompare1A(0);  // U
        ATIM_SetCompare2A(0);  // V
        ATIM_SetCompare3A(0);  // W

        ATIM_CtrlPWMOutputs(ENABLE);
    }

    ATIM_Cmd(ENABLE);
}

void ATIM_IRQHandler(void)
{
}

void DMACH1_IRQHandler(void)
{
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
