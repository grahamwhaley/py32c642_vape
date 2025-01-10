# Low power timer test

There are a few low power modes - low power timer, sleep mode and stop mode. We will test some low
power timer mode here.

The lptimer runs at 32.768KHz, but we use the predevider set to 128, so that is 256 ticks per second.

According to the datasheet we should be able to get the CPU down to about 85uA in sleep mode, and
way down to 1.7uA in stop mode...

 > Note: The test board is likely broken, and as seen later we should expect to get down to more like
   8uA in stop mode...

| mode | current |
| ---- | ------- |
| run, 0 led  | 3.8mA   |
| run, 1 led  | 4.1mA   |
| stop, no Tick  | 3.18mA   |
| stop, no Tick, no gpio  | 3.13mA   |

OK, we are a good couple of mA above where we should be. Possible explanations:

  - we have not turned everything off
  - the resistor divider voltage measuring on the board is drawing current
  - the lipo charging chip is not in sleep mode. Speaking of which, in shutdown mode it should
    be drawing something like 27uA - leading us to a total of about 100uA, 0.1mA
  - we are powering the board through the wrong connectors?

Aha, brainwave - let's reprogram the original vape firmware, and see what current it draws when idling!

And.. tada!, it is drawing 3.16mA. Pretty much the same as we are when we still have GPIOs active (and
the vape fw needs to keep some GPIOs active, as that is the wake signal from the pressure sensors).

Next thing then is to wire the power from a bench PSU directly across the battery leads and make
sure that is the minimum... right, with the supply nominally at 3.29v the meter says 2.69mA. That's
still pretty poor! The battery in the vape is a 550mAh, which notionally only gives us about 8.5 days
of standby??? To confirm, I flipped the multimeter into uA mode, and we get 2540uA.

Now, I wonder if there is a timeout 'super sleep' mode on the vape? Let's leave it sat for a while and
see if it manages a super deep sleep?

Or, maybe this vape board had an issue which meant the vape self-discharged - and it's not our fault?
We should try another board as a baseline comparison at least...

Aha, so, we recovered another board. This time we left it fully 'stock', with original firmware. We then
activated the sensor, and it is alive. The current then drops to about 1.4mA briefly before finally
dropping to 6.8uA - now, that is more like it!

Now to figure out what is different between the boards, or the firmwares - was the first board duff
or??
What is different is that this new board shows on the screen 'Lo' and lights the battery symbol when
I activate the sensor. OK, now the original board is also doing that. Hmm, I wonder if there are some
option bytes that could be different?

It's all a bit odd. I cranked the voltage to 4.1v. The original unit shows low battery still. The new
unit showed 80% battery once, and now shows low battery again.

Well, I have 4 of these units - looks like it is maybe time to try another 'virgin' board and see what
we find..

Oh, btw, if the board really can sit at say 8uA at 3.7v, then for a 550mAh battery, that could be
2864 days, or more than 7 years.... that's about the same as a Lipo self discharge rate (about
1% per month or so).
