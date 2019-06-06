/*
 * Descreption :
 * 
 * This code reads the analog channel 0 and analog channel 1 of ADS1115 16 bit ADC IC over 
 * I2C interface and converts the analog readings to voltage. Later the converted voltage is
 * mapped to pressure range by calculating the slope. Later pressure readings are displayed 
 * on serial monitor
 * 
 * Converted voltage range (0.4V - 2V)
 * Pressure range (0 - 16 Bar)
 */

/*
 * Including header files
 */
#include <Wire.h>                       //Library for I2C Interface
#include <Adafruit_ADS1015.h>           //Library for ADS1115 16 bit ADC


//Defining object for Adafruit_ADS1115 class 
Adafruit_ADS1115 ads;  

//Defining the variables to store the respective channel value 
int16_t adc0, adc1;

//Defining Min and Max value for converted voltage
//Defining Min and Max value for pressure
const float L_Voltage  = 0.4;      
const byte  H_Voltage  = 2;
const byte  L_Pressure = 0;
const byte  H_Pressure = 16;

float Slope, Analog_Voltage_0, Analog_Voltage_1;
float Pressure_1, Pressure_2;


/*
 * Setup Code
 */
void setup(void) 
{
  //Intializing serial0 or UART0 communication at 9600 baud rate
  Serial.begin(9600);

  //Initializing the ADC to read analog vaues from all the four channels  
  ads.begin();
}

/*
 * Main Loop
 */
void loop(void) 
{
  //Reading the channel 0 and channel 1 and storing the values in  adc0 and adc1 variable.
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);

  //Converting ADC readings to voltage 
  Analog_Voltage_0 = ((3.3 * adc0) / 17634);
  Analog_Voltage_1 = ((3.3 * adc1) / 17634);

  //Finding slope for mapping voltage to pressure
  Slope = ((H_Pressure - L_Pressure) / (H_Voltage - L_Voltage));

  //Converting voltage range to pressure range
  Pressure_1 = ((Analog_Voltage_0 - L_Voltage) * Slope) + L_Pressure;
  Pressure_2 = ((Analog_Voltage_0 - L_Voltage) * Slope) + L_Pressure;

  //Displaying pressure range on serial terminal
  Serial.print("Presure Channel 1 :");
  Serial.println(Pressure_1);

  Serial.print("Presure Channel 2 :");
  Serial.println(Pressure_2);
  
  //Delay of one second
  delay(1000);
}
