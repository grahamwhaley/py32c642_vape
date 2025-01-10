// SPDX-License-Identifier: BSD-3-Clause
// Copyright: Graham Whaley

/* Code for controlling individual LEDs in the charlieplex matrix */

#include "py32f002b_hal.h"
#include "charlie.h"

#define BATTERY_NUM 20
#define TEARDROP_NUM 21
#define PERCENTAGE_NUM 22

/* Lower digit entries are:

        5
      3   1
		4
     15   10
		2
*/

static int lower_digit_valid = 0b00000000000000001000010000111110;
static int lower_digit[10] = {
	/* 3         2         1         0*/
	/* 0         0         0         0*/
	/*                X    X    XXXXX */
	0b00000000000000001000010000101110,	//0
	0b00000000000000000000010000000010,	//1
	0b00000000000000001000000000110110,	//2
	0b00000000000000000000010000110110,	//3
	0b00000000000000000000010000011010,	//4
	0b00000000000000000000010000111100,	//5
	0b00000000000000001000010000111100,	//6
	0b00000000000000000000010000100010,	//7
	0b00000000000000001000010000111110,	//8
	0b00000000000000000000010000111110	//9
};

/* tens digit entries are:

        11
     14    12
        19
      9    13
         8
*/

static int tens_digit_valid = 0b00000000000010000110101110000000;
static int tens_digit[10] = {
	/* 3         2         1         0*/
	/* 0         0         0         0*/
	/*            X    XX X XXX       */
	0b00000000000000000110101110000000,	//0
	0b00000000000000000010000010000000,	//1
	0b00000000000010000000101110000000,	//2
	0b00000000000010000010100110000000,	//3
	0b00000000000010000110000010000000,	//4
	0b00000000000010000110100100000000,	//5
	0b00000000000010000110101100000000,	//6
	0b00000000000000000010100010000000,	//7
	0b00000000000010000110101110000000,	//8
	0b00000000000010000110100110000000	//9
};

/* hundreds digit entries are:


     17

     16

*/

static int hundreds_digit_valid = 0b00000000000000110000000000000000;
static int hundreds_digit[10] = {
	/* 3         2         1         0*/
	/* 0         0         0         0*/
	/*              XX                */
	0b00000000000000000000000000000000,	//0
	0b00000000000000110000000000000000,	//1
	0b00000000000000000000000000000000,	//2
	0b00000000000000000000000000000000,	//3
	0b00000000000000000000000000000000,	//4
	0b00000000000000000000000000000000,	//5
	0b00000000000000000000000000000000,	//6
	0b00000000000000000000000000000000,	//7
	0b00000000000000000000000000000000,	//8
	0b00000000000000000000000000000000	//9
};

void digit_show(int n) {
	int d, tens, hundreds;

	d = n%10;

	{
		int v = lower_digit[d];
		int bit = 0;

		//We could make this more efficient by only cycling up to the highest
		// digit for this segment.
		// We could go even further by only doing math/decisions for set bits, and
		// completely skipping 0 bits in the valid array.
		for(int i=0; i<32; i++) {
			if (v&0x1) {
				charlie_led_on(bit);
			} else {
				if (lower_digit_valid & (1<<bit)) charlie_led_off(bit);
			}

			v = v>>1;
			bit++;
		}
	}

	tens = (n%100)/10;

	{
		int v = tens_digit[tens];
		int bit = 0;

		for(int i=0; i<32; i++) {
		//FIXME - we need to be smarter about clearing down '0' when counting...
			if (v&0x1) {
				if( (n<100) && (tens==0) )		//Don't show a leading '0'
					charlie_led_off(bit);
				else
					charlie_led_on(bit);
			} else {
				if (tens_digit_valid & (1<<bit)) charlie_led_off(bit);
			}

			v = v>>1;
			bit++;
		}
	}

	hundreds = (n%1000)/100;

	{
		int v = hundreds_digit[hundreds];
		int bit = 0;

		for(int i=0; i<32; i++) {
			if (v&0x1) {
				charlie_led_on(bit);
			} else {
				if (hundreds_digit_valid & (1<<bit)) charlie_led_off(bit);
			}

			v = v>>1;
			bit++;
		}
	}
}

void digit_battery(bool show) {
	if (show)
		charlie_led_on(BATTERY_NUM);
	else
		charlie_led_off(BATTERY_NUM);
}

void digit_teardrop(bool show) {
	if (show)
		charlie_led_on(TEARDROP_NUM);
	else
		charlie_led_off(TEARDROP_NUM);
}

void digit_percentage(bool show) {
	if (show)
		charlie_led_on(PERCENTAGE_NUM);
	else
		charlie_led_off(PERCENTAGE_NUM);
}

