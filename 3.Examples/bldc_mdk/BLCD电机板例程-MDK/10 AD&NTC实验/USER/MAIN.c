/*
系统时钟配置为64M
所用IO口：PA4
LED1 0.5S翻转一次
OLED屏幕显示NTC温度值
注意：供电5V或者12V
*/

#include "main.h"
#include "OLED_I2C.h"
#include "global.h"

void GPIO_Configuration(void);
void RCC_Configuration(void);
void ADC_Configuration(void);
void BTIM_init(void);
unsigned int TempSampleComu(void);

unsigned int timecount=0;
uint16_t adcvalue;
float canshu=0;
uint16_t   daAA=0;

int main()
{	
	char temp_buff[10];
	RCC_Configuration();   //64M时钟配置
	GPIO_Configuration();  //LED_Init
	ADC_Configuration();   //ADC_Init
	BTIM_init();           //BTIM_init 
	I2C_init();            //I2C_init
	I2C_OLED_Init();
	
	while(1)
	{ 
			if(timecount>500)//每500ms单次转换一次，并通过OLED显示
			{
				  timecount=0;		
			    ADC_SoftwareStartConvCmd(ENABLE);
					while(ADC_GetITStatus(ADC_IT_EOC))
					{
						ADC_ClearITPendingBit(ADC_IT_EOC); 
            canshu=TempSampleComu();
	          sprintf(temp_buff,"NTC:%0.1f C ",(canshu/10));
		        I2C_OLED_ShowString(0,0,temp_buff);
	          I2C_OLED_UPdata();
					} 					 	
			}			
	}
}

//NTC温度查表计算	
unsigned int TempSampleComu(void)
{	
	float t;
	unsigned int dat,max,min,mid,da,j;

	      t=ADC_GetConversionValue();
				t=t/4096;
				t=t*3300;
				t=t/(5-t/1000); 
				dat=t*10;
	      da=dat;
		    max=97;	min=0;	
		
				while(1)	
				{
				mid=(max+min)/2;
				if(Table[mid]<da
					)		max=mid;
				else 					min=mid;
				if((max-min)<=1)		break;
				}	
      
				if(max==min)da=min*10;
				else 
				{
					j=(Table[min]-Table[max])/10;
					j=(Table[min]-da)/j;
					da=j;
					da=10*min+da; //采集的温度放大了10倍
				 }
				
return da;		 
}


void RCC_Configuration(void)
{
  /* 0. HSI使能并校准 */
  RCC_HSI_Enable(RCC_HSIOSC_DIV6);

  /* 1. 设置HCLK和PCLK的分频系数　*/
  RCC_HCLKPRS_Config(RCC_HCLK_DIV1);
  RCC_PCLKPRS_Config(RCC_PCLK_DIV1);
  
  /* 2. 使能PLL，通过PLL倍频到64MHz */
  RCC_PLL_Enable(RCC_PLLSOURCE_HSI, 8000000, 8);     // HSI 默认输出频率8MHz
 // RCC_PLL_OUT();  //PC13脚输出PLL时钟
  
  ///< 当使用的时钟源HCLK大于24M,小于等于48MHz：设置FLASH 读等待周期为2 cycle
  ///< 当使用的时钟源HCLK大于48MHz：设置FLASH 读等待周期为3 cycle
  __RCC_FLASH_CLK_ENABLE();
  FLASH_SetLatency(FLASH_Latency_3);   
    
  /* 3. 时钟切换到PLL */
  RCC_SysClk_Switch(RCC_SYSCLKSRC_PLL);
  RCC_SystemCoreClockUpdate(64000000);	
}



void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	
	__RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.IT = GPIO_IT_NONE; //LED1 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pins = GPIO_PIN_13;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(CW_GPIOC, &GPIO_InitStruct);
	GPIO_WritePin(CW_GPIOC,GPIO_PIN_13,GPIO_Pin_SET);
}


void ADC_Configuration(void)
{
   ADC_SingleChTypeDef ADC_SingleInitStruct;

  __RCC_ADC_CLK_ENABLE();    // ADC时钟使能
	__RCC_GPIOB_CLK_ENABLE();

  PA04_ANALOG_ENABLE();
  ADC_SingleInitStruct.ADC_Chmux = ADC_ExInputCH4;         // * 通道4输入PA04
  ADC_SingleInitStruct.ADC_DiscardEn = ADC_DiscardNull;
  ADC_SingleInitStruct.ADC_InitStruct.ADC_AccEn = ADC_AccDisable;
  ADC_SingleInitStruct.ADC_InitStruct.ADC_Align = ADC_AlignRight;	   //ADC转换结果右对齐
  ADC_SingleInitStruct.ADC_InitStruct.ADC_ClkDiv = ADC_Clk_Div16;    //PCLK
  ADC_SingleInitStruct.ADC_InitStruct.ADC_DMAEn = ADC_DmaDisable;    //关闭DMA传输
  ADC_SingleInitStruct.ADC_InitStruct.ADC_InBufEn = ADC_BufEnable;   //开启跟随器
  ADC_SingleInitStruct.ADC_InitStruct.ADC_OpMode = ADC_SingleChOneMode;   
  ADC_SingleInitStruct.ADC_InitStruct.ADC_SampleTime = ADC_SampTime5Clk;//5个ADC时钟周期
  ADC_SingleInitStruct.ADC_InitStruct.ADC_TsEn = ADC_TsDisable;  //内置温度传感器禁用  
  ADC_SingleInitStruct.ADC_InitStruct.ADC_VrefSel = ADC_Vref_VDDA;//VDDA参考电压
  ADC_SingleInitStruct.ADC_WdtStruct.ADC_WdtAll = ADC_WdtDisable;    

  ADC_SingleChOneModeCfg(&ADC_SingleInitStruct);
  ADC_Enable();    // 使能ADC
	
	ADC_SoftwareStartConvCmd(ENABLE);
}


void BTIM_init(void)
{
	BTIM_TimeBaseInitTypeDef BTIM_InitStruct;
	
	__RCC_BTIM_CLK_ENABLE();
	
	__disable_irq(); 
  NVIC_EnableIRQ(BTIM1_IRQn); 
  __enable_irq();
	
	BTIM_InitStruct.BTIM_Mode = BTIM_Mode_TIMER;
  BTIM_InitStruct.BTIM_OPMode = BTIM_OPMode_Repetitive;
  BTIM_InitStruct.BTIM_Period = 8000;
  BTIM_InitStruct.BTIM_Prescaler = BTIM_PRS_DIV8;
  BTIM_TimeBaseInit(CW_BTIM1, &BTIM_InitStruct);
	
  BTIM_ITConfig(CW_BTIM1, BTIM_IT_OV, ENABLE);
  BTIM_Cmd(CW_BTIM1, ENABLE);
}

void BTIM1_IRQHandler(void)
{
  /* USER CODE BEGIN */
 static unsigned int count2=0;
 if(BTIM_GetITStatus(CW_BTIM1, BTIM_IT_OV))
  {
    BTIM_ClearITPendingBit(CW_BTIM1, BTIM_IT_OV);
		count2++;
		timecount++;
		if(count2>500)
		{
			count2=0;
		  PC13_TOG();
		}
	}
  /* USER CODE END */
}









