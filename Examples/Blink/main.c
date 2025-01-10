// SPDX-License-Identifier: BSD-3-Clause
// Copyright: Graham Whaley

/*
 py32fc642f15 Hayati Pro Ultra vape LED blink example

 Based off the code from:
   - https://github.com/IOsetting/py32f0-template
   Which in turn is based off the example code from Puya Semiconductor,
   which in turn is based off the code from ST Microelectronics.

   Simply blink a single LED to show we have the correct GPIO pin worked out.
*/

#include "py32f002b_hal.h"

static void APP_GpioConfig(void);

int main(void)
{
  /* Reset of all peripherals, Initializes the Systick. */
  HAL_Init();
  
  /* Initialize GPIO */
  APP_GpioConfig();

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);

  while (1)
  {
    /* Delay 250ms */
    HAL_Delay(250);

    /* LED flipping */
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
  }
}

static void APP_GpioConfig(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();                          /* Enable GPIOA clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();                          /* Enable GPIOB clock */

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            /* Push-pull output */
  GPIO_InitStruct.Pull = GPIO_NOPULL;                    /* Disable pull-up */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO speed */  
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);                

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;            /* Open Drain */
  GPIO_InitStruct.Pull = GPIO_NOPULL;                    /* Disable pull-up */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO speed */  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);                
}

void APP_ErrorHandler(void)
{
  while (1)
  {
  }
}

