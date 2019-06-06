# 0-10 V Analog Input Circuit Descreption

1. Pair of resistor R1-R13, R2-R14, R3-R15, R4-R16 are used to scale down the voltage (i.e. 10v will be converted to 5v) and later it is provided to the analog input pins of Atmega256.
2. AIN2, AIN3, AIN4, AIN5 are connected to A12, A13, A14 and A15 which are pins of Arduino Mega.
3. 5.1 V zener diodes are used to supress the applied input voltage if it is greater than 10v, that will protect the analog pins of Atmega256.
