# Battery check and measure

In this example we check two things:

 - If a charge voltage is present, by checking the input status of PB4, which reads a resistor
   divider across the LiPo chip voltage input
 - Measure the current VCCA, which in effect is the same as the battery voltage, so we can get
   some idea of the status of the battery charge.

There are a couple of 'anomolies' here though with this board...

 - I don't really understand why they did not wire the LiPo charge chip 'CHRG' and 'FULL' output
   pins to tell how the LiPo charge is doing... If we used PB4, and PB3 looks free, we might have
   enough pins
 - How in the current setup can you tell how far through a charge cycle you are? You can probably
   measure the first 80% of the charge cycle, as that is the CC phase, when the cell voltage will
   slowly rise, but the last 20% or so is the CV phase, at which point the voltage you are reading
   on VCCA will be the same, and afaict we have no way to turn off the LiPo charger to measure the
   actual cell voltage, as the LiPo charger chip EN pin is wired permanently on?

## How we calculate VCCA

The 'trick' to calculating VCCA is to use the ADC to measure the internal 1.2v reference voltage
with VCCA as the ADC reference voltage. You can then work out what 'proportion' of the reading
correlated with 1.2v, and thus what the full scale reading represents.

    adc_val = 1.2  * 4096

           -----------

               VCCA

or, to swap that around, and get the result in millivolts rather than volts (so we don't have to do
any float math in our tiny processor...)

    VCCA =  1200 * 4096

         ----------

           adc_val

and that is basically the code you will find in the example.
