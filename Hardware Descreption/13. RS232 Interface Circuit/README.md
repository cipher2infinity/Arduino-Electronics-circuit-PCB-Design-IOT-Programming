# RS232 Interface Circuit Descreption

1. MAX232 from Maxim Corp. is a line driver IC for RS232 Interface, MAX232_TX and MAX232_RX is connected to Rx and TX pins of UART1 on ATMEGA256.
2. T1Out and R1In is connected to DB9 connector.
3. C2 and C8 are used to double the voltage from 5v to 10 volt which is in RS232 standerd range for logic '0'.
4. C10 and C9 are used to invert the voltage from +10 to -10 volt which is basically logic '1' for RS232 standerd.
