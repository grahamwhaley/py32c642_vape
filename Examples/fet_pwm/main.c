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

// PA7 secondary function TIM1_CH4
#define FET1_MAIN_PORT	GPIOA
#define FET1_MAIN_PIN	GPIO_PIN_7

#define FET1_SUB_PORT	GPIOB
#define FET1_SUB_PIN	GPIO_PIN_7

// also as ADC_IN3
#define FET1_MEASURE_PORT	GPIOA
#define FET1_MEASURE_PIN	GPIO_PIN_6

// PA5 secondary function TIM1_CH1 / TIM14_CH1
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
GPIO_InitTypeDef GPIO_InitStruct;

static void APP_TimPwmConfig(void);
static void APP_TimConfig(void);
static void APP_ErrorHandler(void);

static TIM_HandleTypeDef TimHandle;
static TIM_OC_InitTypeDef sConfig;

int main(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  bool pull_fet = false;

  /* Reset of all peripherals, Initializes the Systick. */
  HAL_Init();
  
  __HAL_RCC_TIM1_CLK_ENABLE();

  charlie_init();

  APP_TimConfig();
  APP_TimPwmConfig();

  /* Channel4 Start PWM */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  // GPIO A clock init is already done by the charlie code
  // And now set that pin to be the alternative function
  GPIO_InitStruct.Pin = FET1_MAIN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;         /* Need to be able to pull low, or float */
  //GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;         /* Need to be able to pull low, or float */
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;		/* We want the timer func in AF2 */
  GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* There is an external pullup on the fet */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;     /* GPIO speed */
  HAL_GPIO_Init(FET1_MAIN_PORT, &GPIO_InitStruct);

  while (1)
  {
    digit_teardrop(pull_fet);

	pull_fet = !pull_fet;

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

static void APP_TimConfig(void)
{
  TimHandle.Instance = TIM1;                                                  

  // The +1's are because the hardware registers use 'val+1' in their calculations..

  // Presume we are running with a 24MHz clock
  // Decide what 'tick' we want - say, 20KHz...
  // And then decide what granularity of ratio control we'd like over that
  // say, single-percentage-points, so, 100...
  // So, a (20K*100) == 2MHz clock, with a reset cycle of 100

  // 24M/12 == 2MHz
  TimHandle.Init.Prescaler         = 12 - 1;                                 

  // Granularity of 100
  TimHandle.Init.Period            = 100-1;                                     

  // FIXME - explain why this? - no clock division
  TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;                  

  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;                      

  /* Repetition = 0  - start recount at 0? */
  TimHandle.Init.RepetitionCounter = 1 - 1;                                   

  /* Auto-reload register not buffered - why not?? */
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;          

  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)                                
  {
    APP_ErrorHandler();
  }

}

static void APP_TimPwmConfig(void)
{
  /* Set output compare mode: PWM1 */
  sConfig.OCMode       = TIM_OCMODE_PWM1;                                     

  /* OC channel output high level effective */
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;                                 

  /* Disable OC FastMode */
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;                                  

  /* OCN channel output high level effective */
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;                                

  /* Idle state OC1N output low level */
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;                              

  /* Idle state OC1 output low level*/
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;                               

  // We are focussing on TIM1_CH4 for the main FET coil0 output
  // Start with a 50/50 duty cycle
  sConfig.Pulse = 50;                                               

  /* Channel 4 configuration */
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
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
