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
#include "digit.h"

static void set_led(int high, int low);
static void APP_GpioConfig(void);

int main(void)
{
  int charlie=0;
  int counter = 0;
  bool battery = false, teardrop = false, percentage = false;

  /* Reset of all peripherals, Initializes the Systick. */
  HAL_Init();
  
  charlie_init();

  while (1)
  {
  	digit_show(counter);

	if (counter%10 == 0) {
		battery = !battery;
		digit_battery(battery);
	}

	if (counter%25 == 0) {
		teardrop = !teardrop;
		digit_teardrop(teardrop);
	}

	if (counter%50 == 0) {
		percentage = !percentage;
		digit_percentage(percentage);
	}

    /* Delay 250ms */
    //HAL_Delay(250);
    HAL_Delay(100);

	counter = (counter+1)%199;
  }
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

