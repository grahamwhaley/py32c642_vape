// SPDX-License-Identifier: BSD-3-Clause
// Copyright: Graham Whaley

/* Code for controlling individual LEDs in the charlieplex matrix */

#include "py32f002b_hal.h"
#include "charlie.h"

// We are storing *all* possible pin combinations here. We could make this
// more efficient if we used row:column numbering, and thus did not store
// the entries where row==column, as they are not 'valid' charlieplex values
// for lighting leds...
static bool charlie_leds[CHARLIE_COMBOS];
static int charlie_counter = 0;
static bool last_led_on = false;

// Turn all the leds off by making all the charlie pins high-z
void leds_all_off() {
  GPIO_InitTypeDef GPIO_InitStruct;

  //In theory it looks like we could use either open drain mode with a pin
  // set high, or analog input mode to set the pins to high-z, but in practice
  // it looks like the analog pin mode drains enough current to make a faint
  // glow (at least a hard-reset default state pin seems to), so we went with
  // OD mode, and that looks to work fine.

  //GPIOA - two pins - set to Open Drain mode
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;            /* Open Drain */
  GPIO_InitStruct.Pull = GPIO_NOPULL;                    /* Disable pull-up */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO speed */  
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);                

  //GPIOA - three pins - set to Open Drain mode
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);                

  //and now ensure all those pins are 'high'
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
}

// Turn on a single led in the charlieplex array by raising one pin high, and
// setting one pin low - and leave the rest as high-z
static void led_on(int high, int low) {
  GPIO_InitTypeDef  GPIO_InitStruct;
  int high_gpio=0, low_gpio=0;
  GPIO_TypeDef *high_port, *low_port;

  //Nothing to do if row == column
  if (high == low ) return;

  //Work out which port/pin we need for the high pin
  // We could probably do this as a const lookup table, which might be a fraction quicker?
  switch(high) {
  	case 0:
		high_gpio = GPIO_PIN_1;
		high_port = GPIOA;
		break;

  	case 1:
		high_gpio = GPIO_PIN_0;
		high_port = GPIOA;
		break;

  	case 2:
		high_gpio = GPIO_PIN_0;
		high_port = GPIOB;
		break;

  	case 3:
		high_gpio = GPIO_PIN_1;
		high_port = GPIOB;
		break;

  	case 4:
		high_gpio = GPIO_PIN_2;
		high_port = GPIOB;
		break;

	default:	//Should never happen... default to something 'safe'?
		high_gpio = GPIO_PIN_1;
		high_port = GPIOA;
		break;
  }
  
  // Work out which port/pin we need to set to low
  switch(low) {
  	case 0:
		low_gpio = GPIO_PIN_1;
		low_port = GPIOA;
		break;

  	case 1:
		low_gpio = GPIO_PIN_0;
		low_port = GPIOA;
		break;

  	case 2:
		low_gpio = GPIO_PIN_0;
		low_port = GPIOB;
		break;

  	case 3:
		low_gpio = GPIO_PIN_1;
		low_port = GPIOB;
		break;

  	case 4:
		low_gpio = GPIO_PIN_2;
		low_port = GPIOB;
		break;

	default:	//Should never happen... default to something 'safe'?
		low_gpio = GPIO_PIN_1;
		low_port = GPIOA;
		break;
  }

  //Set the high pin to be push-pull, so we can make it a proper 'high'
  // Note, the low pin is in OD main, which can already make a real 'low'
  GPIO_InitStruct.Pin = high_gpio;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            /* Push-pull output */
  GPIO_InitStruct.Pull = GPIO_NOPULL;                    /* Disable pull-up */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO speed */  
  HAL_GPIO_Init(high_port, &GPIO_InitStruct);                

  //and finally set the high pin high, and the low pin low
  HAL_GPIO_WritePin(high_port, high_gpio, GPIO_PIN_SET);
  HAL_GPIO_WritePin(low_port, low_gpio, GPIO_PIN_RESET);
}

void charlie_init(void)
{

  // Enable the two GPIO ports with the charlie leds on them...
  __HAL_RCC_GPIOA_CLK_ENABLE();                          /* Enable GPIOA clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();                          /* Enable GPIOB clock */

  // default to all off
  charlie_all_leds_off();
}

// Turn off everything we are using
// This is over-zealous, as we might not be the only ones using the GPIO ports!
void charlie_deinit(void) {
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);

  //And turn off the clocks
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
}

//Our high-frequency multiplex driver function, to be called from a timer somewhere in order
// to cycle through the lit LEDs.
void charlie_tick(void)
{
	//If a led was previously on, turn it off by forcing all lines high-z
	if (last_led_on) leds_all_off();

	//cylcle to the next LED to check
	charlie_counter = (charlie_counter+1)%CHARLIE_COMBOS;

	//and if that led is on, then go turn it on...
	if (charlie_leds[charlie_counter]) {

		//Calculate the 'charlie number' for that led to work out which lines
		// we need to pull high and low
		int high = charlie_counter / CHARLIE_NUM_WIRES;
		int low = charlie_counter % CHARLIE_NUM_WIRES;
		led_on(high, low);

		//Remember we will need to turn that off again at some point
		last_led_on = true;
	}
	// else - leds are already off, nothing to do
}

//Note, this marks the LEDs to be off in the status array - it does not actually do the
// physical pin wiggling - that is done in the charlie timer function
void charlie_all_leds_off(void)
{
	for(int i=0; i<CHARLIE_COMBOS; i++ ) charlie_leds[i] = false;
}

//Note, this marks the LEDs to be on in the status array - it does not actually do the
// physical pin wiggling - that is done in the charlie timer function
void charlie_all_leds_on(void)
{
	for(int i=0; i<CHARLIE_COMBOS; i++ ) charlie_leds[i] = true;
}

void charlie_led_off(int led)
{
	// wires^2 - as we are using 'possible' charlie numbering, not row/column numbers here
	if (led >= (CHARLIE_COMBOS) ) return;
	charlie_leds[led] = false;
}

void charlie_led_on(int led)
{
	if (led >= (CHARLIE_COMBOS) ) return;
	charlie_leds[led] = true;
}

