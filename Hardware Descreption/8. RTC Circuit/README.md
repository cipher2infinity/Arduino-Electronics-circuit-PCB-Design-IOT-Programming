# RTC Circuit Descreption

1. DS1307 RTC IC is responcible for the realtime clock feature of datalogger.
2. 32.768 KHz crystal is connected on X1 and X2 pin of RTC IC, to provide clock to the IC.
3. A CMOS battery adapter is connected on VBATT pin this will keep the clock running even if the power to datalogger is cutoff.
4. Combination of 0.1uf and 10uf capacitor is used to eliminate the noise from power supply that will avoid the data of RTC being corrupted.
5. 2.2k pull up resistors are provided to SCL and SDA pins of I2C interface this will avoid the open drain condition of the I2C hardware and will provide by default logic high on I2C pins if they are not pulled down.
