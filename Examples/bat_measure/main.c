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

//Notionally the C642 has the VCC ADC input option...
#define ADC_CHSELR_CHSEL10

#include "py32f002b_hal.h"
#include "py32f002b_hal_adc.h"
#include "charlie.h"
#include "digit.h"

#define HUNDREDS_UPPER_PART 17
#define HUNDREDS_LOWER_PART 17

// resistor divider on lipo charger input is wired across. Frustratingly this is not an
// ADC pin, so the only thing we can monitor is if external (usb-c 5v) power is applied
// or not. Personally, I would have liked to have seen this wired to the CHRG pin on the
// charging chip?
#define BAT_PORT	GPIOB
#define BAT_PIN	GPIO_PIN_4

static void APP_GpioConfig(void);

ADC_HandleTypeDef hadc;

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

  GPIO_InitStruct.Pin = BAT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;         /* Need to be able to pull low, or float */
  GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* There is an external pullup on the fet */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     /* GPIO speed */
  HAL_GPIO_Init(BAT_PORT, &GPIO_InitStruct);

  while (1)
  {
    GPIO_PinState p;
    HAL_StatusTypeDef conv_ok;

	// Read the charge chip input pin, to see if we have power connected...
    p = HAL_GPIO_ReadPin(BAT_PORT, BAT_PIN);

    if( p == GPIO_PIN_SET ) {
	  digit_teardrop(true);
	} else {
	  digit_teardrop(false);
	}

    //Do the read of the vref (1.2v)
    HAL_ADC_Start(&hadc);
	conv_ok = HAL_ADC_PollForConversion(&hadc, 100000);

	if( conv_ok == HAL_OK ) {
  		int val;
		int millivolts;
		val = HAL_ADC_GetValue(&hadc);

		// Now we have a measure of the 1.5v, relative to VCCA, in the 4096 range... so, work out
		// where that 1.5v 'sits', and thus work out the max (VCCA).
		//millivolts = ( (1.5*1000) * 4095 ) / val;
		millivolts = (4095 * 1200 ) / val;

		//Display that value on the screen, best we can by scrolling digits across the
		// two whole segments. We should max at 4096, so only need 3 cycles to show all the
		// digits as pairs...
		for(int i=0; i<3; i++) {
			int t;

			t = millivolts / 100;
			digit_show(t);
			millivolts *= 10;	//shift the decimal value left one decimal digit

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

  hadc.Instance = ADC1;
  
  hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV32;               /* Set ADC clock*/
  hadc.Init.Resolution            = ADC_RESOLUTION_12B;                      /* 12-bit resolution for converted data */
  hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;                     /* Right-alignment for converted data */
  hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;              /* Scan sequence direction: forward */
  hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;                     /* Single Conversion*/
  hadc.Init.LowPowerAutoWait      = DISABLE;                                 /* Auto-delayed conversion feature disabled */
  hadc.Init.ContinuousConvMode    = DISABLE;                                 /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  hadc.Init.DiscontinuousConvMode = DISABLE;                                 /* Disable discontinuous mode */
  hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;                      /* Software start to trig the 1st conversion manually, without external event */
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;           /* Parameter discarded because software trigger chosen */
  hadc.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;                /* DR register is overwritten with the last conversion result in case of overrun */
  hadc.Init.SamplingTimeCommon    = ADC_SAMPLETIME_41CYCLES_5;               /* Channel sampling time is 41.5 ADC clock cycles */
  if (HAL_ADC_Init(&hadc) != HAL_OK)                                         /* ADC initialization */
  {
    APP_ErrorHandler();
  }

  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;                            /* Set the rank for the ADC channel order */
  sConfig.Channel      = ADC_CHANNEL_VREFINT;                                /* ADC channel selection */
  //sConfig.Channel      = ADC_CHANNEL_VCCA;                                /* ADC channel selection */
  //sConfig.Channel      = ADC_CHANNEL_TEMPSENSOR;                                /* ADC channel selection */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)                      /* Configure ADC channels */
  {
    APP_ErrorHandler();
  }
  
  /* Configure VrefBuf 1.5V */  
  //HAL_ADC_ConfigVrefBuf(&hadc,ADC_VREFBUF_1P5V);
  /* Configure VrefBuf VCCA */  
  HAL_ADC_ConfigVrefBuf(&hadc,ADC_VREFBUF_VCCA);
  
  if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)                          /* ADC Calibration */
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
