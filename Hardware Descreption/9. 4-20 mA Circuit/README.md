# 4-20 mA Circuit Descreption

1. 100 ohm resistor after the diode are used to convert the current range to voltage range (4-20 mA to 0.4 to 2V).
2. The converted voltage is provided to the diffrential amplifier (Gain = 1) basically it is acting as a buffer. Diffrential amplifier is used to provide the impedance matching between the 4-20 mA sensor output and ADC IC's input.
3. Schottky diodes (SS14) on inverting and non inverting terminals are used to protect the OPAMP pins from overvoltage.
4. Diodes are connected before 100 ohm resistor to avoid readings when sensor is connected in wrong direction.
5. 0.1 uf capacitors are connceted on input pin and output pin of OPAMP this is done to avoid noise on input and output of OPAMP.
6. Output of OPAMP is connected to analog channel 1 and analog channel 2 of ADS1115 IC to read analog output of opamps.
7. ADS1115's I2C pins are connected to I2C pins of microcontroller to read the data for anlog channel 0 and 1.
8. ADDR pin is connected to ground this is done to assign I2C address to the IC.
