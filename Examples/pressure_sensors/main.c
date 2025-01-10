// SPDX-License-Identifier: BSD-3-Clause
// Copyright: Graham Whaley
/*
 py32fc642f15 Hayati Pro Ultra vape charlieplexed LED walk example

 Based off the code from:
   - https://github.com/IOsetting/py32f0-template
   - github.com/decaday/PY32F0_Drivers/Examples/PY32F002B/Example/ADC/ADC_SingleConversion_TriggerSW_Polling
   Which in turn is based off the example code from Puya Semiconductor,
   which in turn is based off the code from ST Microelectronics.
*/

#include "py32f002b_hal.h"
#include "py32f002b_hal_adc.h"
#include "charlie.h"
#include "digit.h"

#define SENSOR1_PORT	GPIOA
#define SENSOR1_PIN		GPIO_PIN_3

#define SENSOR2_PORT	GPIOB
#define SENSOR2_PIN		GPIO_PIN_5

static void set_led(int high, int low);
static void APP_GpioConfig(void);

ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef sConfig;

GPIO_InitTypeDef GPIO_InitStruct;

static void APP_ADCConfig(void);

int main(void)
{
  /* Reset of all peripherals, Initializes the Systick. */
  HAL_Init();
  
  charlie_init();

  /* Normally enable ports A and B for sensor reading - but we know they are already enabled
   * for the charlie led display.
   */

  /* Configure SENSOR1 as an ADC input */
  APP_ADCConfig();

  /* Configure Sensor2 as a GPIO input */
  GPIO_InitStruct.Pin = SENSOR2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;            /* Input */
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;                    /* Enable pull-down */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     /* GPIO speed */
  HAL_GPIO_Init(SENSOR2_PORT, &GPIO_InitStruct);

  while (1)
  {
  	int val;
	GPIO_PinState p;

    HAL_ADC_Start(&hadc1);

	if( HAL_ADC_PollForConversion(&hadc1, 100000) == HAL_OK ) {
		val = HAL_ADC_GetValue(&hadc1);

		//Half the nominal max of 4095 we can get with a 12bit read.
		//Hopefully this stops us triggering on 'bit noise' and any
		// mildly floating pin.
		if ( val > 2048 ) digit_battery(true);
		else digit_battery(false);
	}

	p = HAL_GPIO_ReadPin(SENSOR2_PORT, SENSOR2_PIN);
	if (p == GPIO_PIN_SET )
		digit_teardrop(true);
	else
		digit_teardrop(false);
		
	//not quite a hard poll....
    HAL_Delay(1);
  }
}

void SysTick_Handler()
{
	HAL_IncTick();
	charlie_tick();
}

static void APP_ADCConfig(void)
{
  __HAL_RCC_ADC_CLK_ENABLE();                                                /* Enable ADC clock */

  hadc1.Instance = ADC1;
  
  hadc1.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV32;               /* Set ADC clock*/
  hadc1.Init.Resolution            = ADC_RESOLUTION_12B;                      /* 12-bit resolution for converted data */
  hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;                     /* Right-alignment for converted data */
  hadc1.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;              /* Scan sequence direction: forward */
  hadc1.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;                     /* Single Conversion */
  hadc1.Init.LowPowerAutoWait      = DISABLE;                                 /* Auto-delayed conversion feature disabled */
  hadc1.Init.ContinuousConvMode    = DISABLE;                                 /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  hadc1.Init.DiscontinuousConvMode = DISABLE;                                 /* Disable discontinuous mode */
  hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;                      /* Software start to trig the 1st conversion manually, without external event */
  hadc1.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;           /* Parameter discarded because software trigger chosen */
  hadc1.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;                /* DR register is overwritten with the last conversion result in case of overrun */
  hadc1.Init.SamplingTimeCommon    = ADC_SAMPLETIME_41CYCLES_5;               /* The channel sampling time is 41.5 ADC clock cycles */
  if (HAL_ADC_Init(&hadc1) != HAL_OK)                                         /* ADC initialization */
  {
    APP_ErrorHandler();
  }

  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;                             
  sConfig.Channel      = ADC_CHANNEL_1;                                      
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)                       /* Configure ADC Channel */
  {
    APP_ErrorHandler();
  }
    
  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)                           /* ADC Calibration */
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void APP_ErrorHandler(void)
{
  while (1)
  {
  }
}
