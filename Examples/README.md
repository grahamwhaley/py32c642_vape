# Examples

This directory contains the example code that was both used to investigate and confirme the layout
of the board, and to be used as example code for future development.

The below sections give brief details on the Examples.

## Blink

Blink a single LED using a single GPIO pin wiggle up/down. Just shows we have the right connections
to the LED, and that we are indeed programming the flash and booting correctly.

## led_walk

Walk through all the LEDs by directly manipulating the GPIO pins from the loop code. This is not
making great use of the charlieplexing, as we cannot sensibly light more than one LED at a time, but
it gives us enough code to then work out which LED is connected to which pin, and to verify that we
have identified the correct 5 charliplexed GPIO pins.

## led_walk_charlie

The first 'full' charlieplex example. Here we use a 1KHz time to cycle through any selected 'enabled'
LED to give the impression that we have lit all the LEDs. Note, we cycle through 25 LEDs, so we are
blinking any lit LEDs at 1000/25 == 40Hz, which is acceptible, but if I stare hard at the LEDs I can
perceive they have a slight 'glimmer' to them. If using in production you probably want to set up a
timer to be more like 60Hz to make viewing more comfortable.

The [led_walk_charlie README file](./led_walk_charlie/README.md) file contains more details.

## digit_counter

In this example we code up the table of LED data needed to drive the 2 1/2 7-segment digits as a
numerical display. We then run them as a count-up counter over and over. This is the ultimately useful
code, as it allows an application to display a number, and optionally turn on/off the battery,
teardrop and percentage symbols.

## pressure_sensors

Here we read the pressure sensor results from PA3 and PB5. We read PA3 using the ADC (for no other
reason than we were initially trying to figure out if this was an analog or digital pressure switch),
and PB5 with a GPIO input (and note, I don't think PB5 supports an ADC input mode).

When we note a positive on PB5 we light the battery light. When we detect a positive on PA3 we light
the teardrop.

Given these do look like digital inputs, if using in production it probably makes sense to use them
as digital GPIO inputs, and not use the ADC.

## fet_measure

This test uses the coil sense FET/ADC pins to check if a coil is present, shorted or open. More
details can be found in the [fet_measure README file](./fet_measure/README.md).

## fet_pwm

Show how to PWM the FET vape coils. We only show how to do this for the first coil, but it should
be trivial to make it do the other coil as well. We chose 20KHz as the PWM rate, but there is no
particular reason for that - it could be higher or lower. Note, it took some research to find the
right TIM RCC clock setup and GPIO pin config, purely as they were a bit buried in the original
example. More details in [the README](fet_pwm/README.md).

## lp_clock

Put the chip/board into the lowest power mode we can whilst having it occasionally wake from the
low power clock timer interrupt/event. This allows us to measure our lowest(ish) idle power.

More details in the [README](./lp_clock/README.md).

## bat_measure

Detect if we have charging power connected (to the USB/VBUS), and measure the VCCA voltage that is
in effect the battery voltage. More details in the [README](bat_measure/README.md).
