/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "gconf.h"

#include "system/sleep.h"
#include "pinctrl/key.h"
#include "pinctrl/led.h"

#include "modbus/mb.h"
#include "modbus/mbport.h"

#include "motdrv/enc/hall.h"
#include "motdrv/enc/abs.h"
#include "motdrv/foc/foc.h"
#include "motdrv/foc/pwmctrl.h"
#include "motdrv/filter/rc.h"

#include "oled/oled_i2c.h"

#include "sensor/ntc.h"
#include "sensor/vbus.h"
#include "sensor/pot.h"
#include "sensor/isen.h"

#include "button/button.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern void Modbus_StartReceive(void);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// ST HAL 霍尔模式配置 https://zhuanlan.zhihu.com/p/126478231
// 霍尔位置的对应角�? https://zhuanlan.zhihu.com/p/462932301
// 霍尔传感器信号超前于转子π/6，因此，电角�? = 霍尔角度 - π/6。正向旋转时

hall_encoder_t HallEnc = {
    .HallState = 0,
    .Direction = DIR_FWD,
    .Placement = 120,
    .ElecAngle = 0,
    .EdgeCount = 0,
    .HA_Port   = ENC_HALL_A_GPIO_Port,
    .HA_Pin    = ENC_HALL_A_Pin,
    .HB_Port   = ENC_HALL_B_GPIO_Port,
    .HB_Pin    = ENC_HALL_B_Pin,
    .HC_Port   = ENC_HALL_C_GPIO_Port,
    .HC_Pin    = ENC_HALL_C_Pin,
};

#define AD_CUR_C ADConv[0]
#define AD_CUR_A ADConv[1]
#define AD_VBUS  ADConv[3]
#define AD_NTC   ADConv[2]
#define AD_POT1  ADConv[4]
#define AD_POT2  ADConv[5]

void EncCycle()
{
    if (ParaTbl.u16EncTurnClr != 0)
    {
        ParaTbl.u16EncTurnClr = 0;
        ParaTbl.u32EncTurns   = 0;
    }
}

__IO uint16_t ADConv[6] = {0};

int ADC_Offset[6];

void Calibrate_ADC_Offset(void)
{
    DelayBlockMS(200);  // wait for sample
    ADC_Offset[0] = AD_CUR_A;
    ADC_Offset[1] = AD_CUR_C;

    if ((ADC_Offset[0] < 1800) || (ADC_Offset[0] > 2200))
    {
        // Error_State.bits.ADC_error = 1;
    }
    if ((ADC_Offset[1] < 1800) || (ADC_Offset[1] > 2200))
    {
        // Error_State.bits.ADC_error = 1;
    }
}

int lpf(int old, int new, float a)
{
    return old * a + new * (1.f - a);
}

rc_filter_t hpf_cura = {.coeff = 2 * 3.1415926 * 80 / 2000};
rc_filter_t hpf_curb = {.coeff = 2 * 3.1415926 * 80 / 2000};
rc_filter_t hpf_curc = {.coeff = 2 * 3.1415926 * 80 / 2000};
rc_filter_t lpf_cura = {.coeff = 2 * 3.1415926 * 200 / 2000};
rc_filter_t lpf_curb = {.coeff = 2 * 3.1415926 * 200 / 2000};
rc_filter_t lpf_curc = {.coeff = 2 * 3.1415926 * 200 / 2000};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
    DelayInit();

    const uint8_t ucSlaveID[] = {0xAA, 0xBB, 0xCC};
    eMBInit(MB_RTU, 1, 1, 115200, MB_PAR_EVEN);
    eMBSetSlaveID(0x34, true, ucSlaveID, ARRAY_SIZE(ucSlaveID));
    eMBEnable();
    Modbus_StartReceive();

    HallEnc_Init(&HallEnc);
    // HAL_TIM_Base_Start(TIM_HALL);
    // HAL_TIM_Base_Start_IT(TIM_HALL);

    HAL_ADCEx_Calibration_Start(&hadc1);

		// oled_init();
		// oled_draw_str(0, 0, "hello world", crBLACK, crWHITE);
		// oled_update();

    ParaTbl_Init();

    //-----------------------------------------------------------------

    ParaTbl.u16ElecAngSrc = ELEC_ANG_SRC_HALL;  // closeloop
    // ParaTbl.u16ElecAngSrc = ELEC_ANG_SRC_NONE;  // openloop

    ParaTbl.u16PwmDutyMax = PWM_PERIOD / 2;
    ParaTbl.u16DutyPha = ParaTbl.u16DutyPhb = ParaTbl.u16DutyPhc = ParaTbl.u16PwmDutyMax / 2;

    ParaTbl.u16MotPolePairs = 2;
    ParaTbl.u16Umdc         = (int)11.2;
    ParaTbl.u16CarryFreq    = 64e6 / PWM_PERIOD;  // center align mode

    if (ParaTbl.u16ElecAngSrc == ELEC_ANG_SRC_NONE)  // zero inject
    {
        ParaTbl.s16VqRef       = 16000;
        ParaTbl.s16VdRef       = 0;
        ParaTbl.u16ElecAngInc  = 24;
        ParaTbl.u16WaitTimeInc = 8;
    }
    else  // svpwm7
    {
		
			ParaTbl.u16ElecOffset =  HALL_ANGLE_30*1.5; 	// 为 0 正好对上， 这里超前45
        ParaTbl.s16VqRef      = 0;
        ParaTbl.s16VdRef      = 0;
        ParaTbl.s16SpdKp      = 3;
        ParaTbl.s16SpdKi      = 8;
        ParaTbl.s16SpdDigRef  = 500;
    }

    ParaTbl.u16RunState = 0;

    // Hall Enc
    ParaTbl.u32EncRes = 6 * ParaTbl.u16MotPolePairs;

    // Abs Enc
    // ParaTbl.u32EncRes = 1 << 14;  // 14-bit
    // ParaTbl.s64EncPosInit = 0;

    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, ParaTbl.u16PwmDutyMax >> 1);
    HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_4);

    Calibrate_ADC_Offset();

    //-----------------------------------------------------------------

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
        eMBPoll();

			if(ParaTbl.u16RunState == 2){
					//HAL_NVIC_SystemReset();
			  	NVIC_SystemReset();
				ParaTbl.u16RunState =0 ;
			}
			
        PeriodCall(1 * UNIT_S, { LED1_TGL(); });
        PeriodCall(10 * UNIT_S / (abs(ParaTbl.s16SpdFb) + 1), { LED2_TGL(); });

#if 0
        PeriodCall(20 * UNIT_MS, { button_scan(); });
#endif

        //---------------

        static u16 u16RunStatePre = 0;

        if (u16RunStatePre != ParaTbl.u16RunState)
        {
            u16RunStatePre = ParaTbl.u16RunState;
            ParaTbl.u16RunState ? PWM_Start() : PWM_Stop();
        }

        //-------------------------------------------------

#if CONFIG_ENCODER_TYPE == ENC_HALL
        ParaTbl.u16HallAngFb = HallEnc.ElecAngle;
#endif

        PeriodCall(100 * UNIT_MS, {  // speed calc
#if CONFIG_ENCODER_TYPE == ENC_HALL
            ParaTbl.s16SpdFb  = 10 * HallEnc.Direction * 60.f * HallEnc.EdgeCount / ParaTbl.u16MotPolePairs / 6;
            HallEnc.EdgeCount = 0;

            // ParaTbl.s32EncPos += HallEnc.EdgeCount;
            // ParaTbl.u32EncTurns += ParaTbl.u32EncRes / ParaTbl.u32EncRes;
            // ParaTbl.s32EncPos %= ParaTbl.u32EncRes;
#endif
        });

        PeriodCall(1 * UNIT_MS, {
            ParaTbl.u16Temp = NTC_GetTemp(AD_NTC);
            ParaTbl.u16Umdc = VBUS_GetVoltage(AD_VBUS);
            ParaTbl.s16Ai1U = POT1_GetVoltage(AD_POT1) + ParaTbl.s16Ai1Bias;
            ParaTbl.s16Ai2U = POT2_GetVoltage(AD_POT2) + ParaTbl.s16Ai2Bias;
        });

        //-------------------------------------------------
				
#if CONFIG_ENCODER_TYPE == ENC_ABS
        PeriodCall(50 * UNIT_MS, { tformat_send_read_request(); });
#endif
				
        PeriodCall(500 * UNIT_US, {
            // ParaTbl.s16CurPhAFb = rc_hpf(&hpf_cura, ISEN_GetPhaseCurrent((int16_t)AD_CUR_A));
            // ParaTbl.s16CurPhCFb = rc_hpf(&hpf_curc, ISEN_GetPhaseCurrent((int16_t)AD_CUR_C));
            // ParaTbl.s16CurPhBFb = rc_hpf(&hpf_curb, ISEN_GetPhaseCurrent((int16_t)0 - (int16_t)AD_CUR_A - (int16_t)AD_CUR_C));

            ParaTbl.s16CurPhAFb = (int32_t)ISEN_GetPhaseCurrent((int16_t)AD_CUR_A - ADC_Offset[0]) * 65535 / ParaTbl.u16Umdc;
            ParaTbl.s16CurPhCFb = (int32_t)ISEN_GetPhaseCurrent((int16_t)AD_CUR_C - ADC_Offset[1]) * 65535 / ParaTbl.u16Umdc;
            ParaTbl.s16CurPhBFb = 0 - ParaTbl.s16CurPhAFb - ParaTbl.s16CurPhCFb;

            ParaTbl.s16CurPhAFb = rc_lpf(&lpf_cura, ParaTbl.s16CurPhAFb);
            ParaTbl.s16CurPhBFb = rc_lpf(&lpf_curb, ParaTbl.s16CurPhBFb);
            ParaTbl.s16CurPhCFb = rc_lpf(&lpf_curc, ParaTbl.s16CurPhCFb);
        });

        ifoc();

        extern hall_encoder_t HallEnc;
        HallEnc_Update(&HallEnc);

        if (ParaTbl.u16RunState)
        {
            if (ParaTbl.u16ElecAngSrc == ELEC_ANG_SRC_NONE)
            {
                ParaTbl.u16ElecAngRef += ParaTbl.u16ElecAngInc;
            }
            else if (ParaTbl.u16ElecAngSrc == ELEC_ANG_SRC_HALL)
            {
                ParaTbl.u16ElecAngRef = ((u32)ParaTbl.u16HallAngFb + (u32)ParaTbl.u16ElecOffset) % UINT16_MAX;
            }

            spdloop();
            // curloop();
            ofoc();

            PWM_SetDuty(ParaTbl.u16DutyPha, ParaTbl.u16DutyPhb, ParaTbl.u16DutyPhc);

            if (ParaTbl.u16ElecAngSrc == ELEC_ANG_SRC_NONE)
            {
                DelayBlockUS(ParaTbl.u16WaitTimeInc);
            }
        }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
#if CONFIG_ENCODER_TYPE == ENC_ABS
    if (huart == &UART_TFORMAT)
    {
        ParaTbl.s32EncPos = tformat_process_response();
        // tformat_send_clear_position();
    }
#endif
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
