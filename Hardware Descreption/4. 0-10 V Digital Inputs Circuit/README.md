# 0-10 V Digital Inputs Circuit Descreption

1. To isolate GPIO pin's of microcontroller from the output circuitary 4 channel optocoupler is used.
2. TLP292-4 is a 4 channel optocoupler IC manufactured by Toshiba
3. Diodes connected on input side are used to protect the optocoupler pins from reverse polarity.
4. 1K resistors in series are used to limit the current flowing from the input LED of optocoupler.
5. On output side of optocoupler 1k resistors are pulled down to ground, this is done to pull the GPIO pins of microcontroller
