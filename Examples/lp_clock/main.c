// SPDX-License-Identifier: BSD-3-Clause
// Copyright: Graham Whaley
/*
 py32fc642f15 Hayati Pro Ultra vape charlieplexed LED walk example

 Based off the code from:
   - https://github.com/IOsetting/py32f0-template
   Which in turn is based off the example code from Puya Semiconductor,
   which in turn is based off the code from ST Microelectronics.
*/

#include "py32f002b_hal.h"
#include "charlie.h"

#define CHARLIE_BATT 20
#define CHARLIE_TEAR 21
#define CHARLIE_PERCENT 22

void APP_ErrorHandler(void);
static void APP_RCCOscConfig(void);
static void nap(void);

LPTIM_HandleTypeDef LPTIMConf = {0};
EXTI_HandleTypeDef ExtiHandle;
EXTI_ConfigTypeDef ExtiCfg;

int main(void)
{
  /* Reset of all peripherals, Initializes the Systick. */
  HAL_Init();
  APP_RCCOscConfig();

  charlie_init();

  /* Initialize LPTIM */
  LPTIMConf.Instance = LPTIM1;                        /* LPTIM1 */
  LPTIMConf.Init.Prescaler = LPTIM_PRESCALER_DIV128;  /* DIV 128 */
  LPTIMConf.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE; /* UPDATE IMMEDIATE */
  /* Initialize LPTIM */
  if (HAL_LPTIM_Init(&LPTIMConf) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  ExtiCfg.Line = EXTI_LINE_29;
  ExtiCfg.Mode = EXTI_MODE_EVENT;
  HAL_EXTI_SetConfigLine(&ExtiHandle, &ExtiCfg);

  // We use the 128 divider on the lptimer 32768Hz clock to give us 256
  // ticks per second.
  //  3 second wake time
  HAL_LPTIM_SetContinue_Start_IT(&LPTIMConf, 768);

  while (1)
  {
	charlie_led_on(CHARLIE_BATT);
	HAL_Delay(3000);
	nap();

	charlie_led_on(CHARLIE_TEAR);
	HAL_Delay(3000);
	nap();

	charlie_led_on(CHARLIE_PERCENT);
	HAL_Delay(3000);
	nap();

	charlie_all_leds_off();
	HAL_Delay(3000);
	nap();
  }
}

static void nap(void) {
  HAL_SuspendTick();
  charlie_deinit();
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);
  HAL_ResumeTick();
  charlie_init();
}

static void APP_RCCOscConfig(void)
{
  RCC_OscInitTypeDef OSCINIT;
  RCC_PeriphCLKInitTypeDef LPTIM_RCC;

  /* LSI Clock Configure */
  OSCINIT.OscillatorType = RCC_OSCILLATORTYPE_LSI;  /* LSI */
  OSCINIT.LSIState = RCC_LSI_ON;                    /* LSI ON */
  /* RCC Configure */
  if (HAL_RCC_OscConfig(&OSCINIT) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  LPTIM_RCC.PeriphClockSelection = RCC_PERIPHCLK_LPTIM;     /* Clock Configure Selection：LPTIM */
  LPTIM_RCC.LptimClockSelection = RCC_LPTIMCLKSOURCE_LSI;   /* Select LPTIM Clock Source：LSI */
  /* Peripherals Configure */
  if (HAL_RCCEx_PeriphCLKConfig(&LPTIM_RCC) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* Enable LPTIM Clock */
  __HAL_RCC_LPTIM_CLK_ENABLE();
}

void SysTick_Handler()
{
	HAL_IncTick();
	charlie_tick();
}

void APP_ErrorHandler(void)
{
  while (1)
  {
  }
}

