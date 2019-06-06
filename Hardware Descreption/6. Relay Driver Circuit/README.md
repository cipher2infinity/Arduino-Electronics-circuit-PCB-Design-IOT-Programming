# Relay Driver Circuit Descreption
1. This circuit is used to control the relay which will be connected externally.
2. This circuit uses EL357 optocoupler IC which is used to isolate the microcontroller pin from the output circuitry.
4. BC817 is a NPN transistor which is used to drive the relay, it can provide maximum of 600 mA which is more than sufficient to drive a relay.
5. SS14 schottky diode is used to provide the protection form back EMF.
6. R64 and R65 are used to limit the current flowing from the base of BC817 transistor.
7. R58 and R59 are the current limiting resistor for the led of optocoupler.
8. Relay_1 and Relay_2 pins are connected to GPIO pin of the microcontroller.
