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

static void set_led(int high, int low);
static void APP_GpioConfig(void);

int main(void)
{
  int charlie=0;
  int mode = 0;		//0==single, 1==all, 2==off

  /* Reset of all peripherals, Initializes the Systick. */
  HAL_Init();
  
  charlie_init();

  while (1)
  {

	//Walk the charlie leds. Only turn them off in modes 0/2
	if (mode != 1) charlie_led_off(charlie);

	//Now, we are not walking the (n*(n-1)) maximum number of leds that this charlie pin setup
	// will support - we are walking the (n*n) number of 'pin combinations', which includes
	// pin combos that will not light anything - when the high pin == low pin...
	charlie = (charlie+1) % CHARLIE_COMBOS;
	charlie++;
	if (charlie >= CHARLIE_COMBOS) {
		charlie = 0;
		//walk through the three modes
		mode = (mode+1)%3;
	}

    // turn leds on if we are not in mode 2
	if (mode != 2) charlie_led_on(charlie);

    /* Delay 250ms */
    HAL_Delay(250);
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

