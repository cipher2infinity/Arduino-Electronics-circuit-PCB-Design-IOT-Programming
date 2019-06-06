1. Between TX pin of microcontroller and Rx pin of BLE module a SN74LVC1G125 buffer is connecetd this is done to step down the voltage from 5v to 3.3v as BLE works on 3.3v.
2. Tx pin of BLE is connected to Rx pin of Atmega256, basically BLE is conneted on UART1 of Atmega256.
3. A led with current limiting resistor is connected on pin 24 of BLE, this will let you know the status of BLE whether BLE is connected or not.
4. Key pin is pulled up to 3.3v using a 10k resistor this will provide default logic high on key pin. Whnever key pin will be pulled down to ground the BLE will disconnect the connection from connected device.
