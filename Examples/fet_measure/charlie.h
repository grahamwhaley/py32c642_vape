// SPDX-License-Identifier: BSD-3-Clause
// Copyright: Graham Whaley

#ifndef _CHARLIE_H
#define _CHARLIE_H

#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif 

#define CHARLIE_NUM_WIRES 5
#define CHARLIE_NUM_LEDS (CHARLIE_NUM_WIRES * (CHARLIE_NUM_WIRES-1))
#define CHARLIE_COMBOS (CHARLIE_NUM_WIRES * CHARLIE_NUM_WIRES)

void charlie_init(void);
void charlie_tick(void);

void charlie_all_leds_off(void);
void charlie_all_leds_on(void);

void charlie_led_off(int led);
void charlie_led_on(int led);

#ifdef __cplusplus
}
#endif

#endif /* _CHARLIE_H */
