// SPDX-License-Identifier: BSD-3-Clause
// Copyright: Graham Whaley
/*
 py32fc642f15 Hayati Pro Ultra vape
 
 Test the FET/coil resistance measure inputs found on
  - PA6 (coil1)
  - PA4 (coil2)

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

#define FET1_MAIN_PORT	GPIOA
#define FET1_MAIN_PIN	GPIO_PIN_7

#define FET1_SUB_PORT	GPIOB
#define FET1_SUB_PIN	GPIO_PIN_7

// also as ADC_IN3
#define FET1_MEASURE_PORT	GPIOA
#define FET1_MEASURE_PIN	GPIO_PIN_6

#define FET2_MAIN_PORT	GPIOA
#define FET2_MAIN_PIN	GPIO_PIN_5

#define FET2_SUB_PORT	GPIOC
#define FET2_SUB_PIN	GPIO_PIN_1

// also as ADC_IN2
#define FET2_MEASURE_PORT	GPIOA
#define FET2_MEASURE_PIN	GPIO_PIN_4

#define HUNDREDS_UPPER_PART	17
#define HUNDREDS_LOWER_PART	16

static void APP_GpioConfig(void);

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

GPIO_InitTypeDef GPIO_InitStruct;

static void APP_ADCConfig(void);

int main(void)
{
  bool pull_fet = false;

  /* Reset of all peripherals, Initializes the Systick. */
  HAL_Init();
  
  charlie_init();

  /* Normally enable ports A and B for sensor reading - but we know they are already enabled
   * for the charlie led display.
   */

  /* Configure SENSOR1 as an ADC input */
  APP_ADCConfig();

#define FET1_SUB_PORT	GPIOB

  GPIO_InitStruct.Pin = FET1_SUB_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;         /* Need to be able to pull low, or float */
  GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* There is an external pullup on the fet */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     /* GPIO speed */
  HAL_GPIO_Init(FET1_SUB_PORT, &GPIO_InitStruct);

  while (1)
  {
    HAL_StatusTypeDef conv_ok;

    digit_teardrop(pull_fet);

    if (pull_fet) {
	  //Pull the test fet low to activate
	  HAL_GPIO_WritePin(FET1_SUB_PORT, FET1_SUB_PIN, GPIO_PIN_RESET);
	}

    //Do the read
    HAL_ADC_Start(&hadc1);
	conv_ok = HAL_ADC_PollForConversion(&hadc1, 100000);

    if (pull_fet) {
	  //Let the fet go again as soon as we can
	  HAL_GPIO_WritePin(FET1_SUB_PORT, FET1_SUB_PIN, GPIO_PIN_SET);
	}

	pull_fet = !pull_fet;

	if( conv_ok == HAL_OK ) {
  		int val;
		val = HAL_ADC_GetValue(&hadc1);

		//Display that value on the screen, best we can by scrolling digits across the
		// two whole segments. We should max at 4096, so only need 3 cycles to show all the
		// digits as pairs...
		for(int i=0; i<3; i++) {
			int t;

			t = val / 100;
			digit_show(t);
			val *= 10;	//shift the decimal value left one decimal digit

			if( i&0x1 ) {
				charlie_led_off(HUNDREDS_LOWER_PART);
				charlie_led_on(HUNDREDS_UPPER_PART);
			} else {
				charlie_led_on(HUNDREDS_LOWER_PART);
				charlie_led_off(HUNDREDS_UPPER_PART);
			}

    		HAL_Delay(1000);
		}
	} else {	//Some sort of error
		charlie_all_leds_off();
		digit_show(111);
	}

	//not quite a hard poll....
    HAL_Delay(3000);
	charlie_all_leds_off();
    HAL_Delay(1000);
  }
}

void SysTick_Handler()
{
	HAL_IncTick();
	charlie_tick();
}

static void APP_ADCConfig(void)
{
  ADC_ChannelConfTypeDef sConfig;

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

  //Channel 3 for coil1
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;                             
  sConfig.Channel      = ADC_CHANNEL_3;
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
