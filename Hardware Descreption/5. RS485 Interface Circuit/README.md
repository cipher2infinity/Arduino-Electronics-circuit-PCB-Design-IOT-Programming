# RS485 Interface Descreption

1. This circuit uses MAX485 IC as a driver IC which is manufactured by Maxim Corp.
2. RO and DI pins of IC is connected to UART2 of ATmega256 and over these pins data will be recieved and transmit.
3. !RE and DE are connected to GPIO pin of microcontroller.
4. While transmiting the data RS485_CTRL pin should be connected to logic '0' and while recieving the data it should be connected to Logic '1'
5. C7 is used to remove high frequency noise from the power supply provided to RS485 power pins
6. SM712 is a assymetric TVS diode that provides the protection from transients on RS485 lines.
7. 120 ohm is a termination resistor it is used for impedance matching.
8. A is pulled to 5v and B is pulled to gnd using 1k resistor this done to provide default state to the RS485 line.
9. F1 and F2 are the PPTC fuse of 125mA they are used to provide protection from shortcircuit or overcurrent.
10. MSTBA2 is a dual channel pluggable connector.
