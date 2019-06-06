# SD Card Circuit

1. SN74LVC245 is a 8 channel transiever IC which is used to convert the voltage from 5v to 3.3v as WiFi module works on 3.3v.
2. SD Card works on 3.3 V so voltage on SPI pins are first drop down to 3.3v by using voltage level converter IC.
3. SD detect pin is pulled to 3.3v using a 10k pull up resistor to provide default logic high on SD detect pin of SD card holder.
4. This will allow us to detect whether SD card is present or not.
5. BSS138 MOSFET is used to convert the voltage on MISO pin.
