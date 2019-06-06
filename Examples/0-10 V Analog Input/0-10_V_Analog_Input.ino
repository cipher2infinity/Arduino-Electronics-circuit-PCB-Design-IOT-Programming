/*
 * Descreption :
 * 
 * This code reads the analog channel A12, A13, A14, A15 and converts the
 * Input readings to voltage and the displays the voltage on serial terminal.
 * 
 * The converted voltage is multiplied by 2 because on hardware level 
 * voltage is scaled down to half of input voltage using voltage divider.
 * 
 */


//Defining Analog Input pins
const byte Analog_Channel_1 = A12;    //Pin 85 on Atmega256
const byte Analog_Channel_2 = A13;    //Pin 84 on Atmega256
const byte Analog_Channel_3 = A14;    //Pin 83 on Atmega256
const byte Analog_Channel_4 = A15;    //Pin 82 on Atmega256

float Analog_Value_1, Analog_Value_2, Analog_Value_3, Analog_Value_4;  

/*
 * Setup Code
 */
void setup() 
{
  //Initializing serial communication
  Serial.begin(9600);
}

/*
 * Main Loop
 */
void loop() 
{
  //0-1023 (Atmega256 ADC = 10 bit)
  //2 to the power 10 = 1024
  //5/1024 = 0.0048828
  // Analog reading for 1 = 0.0048828
  // Analog reading for 2 = 2 * 0.0048828 = 0.00976
  // Analog reading for 512 = 512 * 0.0048828 = 2.499 ~ 2.5
  // Analog reading for 1024 = 1024 * 0.0048828 = 4.999 ~ 5
  
  //Reading defined analog channels
  Analog_Value_1 = 2 * (0.0048828 * analogRead(Analog_Channel_1));
  Analog_Value_2 = 2 * (0.0048828 * analogRead(Analog_Channel_2));
  Analog_Value_3 = 2 * (0.0048828 * analogRead(Analog_Channel_3));
  Analog_Value_4 = 2 * (0.0048828 * analogRead(Analog_Channel_4));

  //Displaing Analog channel values 
  Serial.print("Analog Channel 1 : ");
  Serial.print(Analog_Value_1);

  Serial.print("\t\t Analog Channel 2 : ");
  Serial.print(Analog_Value_2);

  Serial.print("\t Analog Channel 3 : ");
  Serial.print(Analog_Value_3);

  Serial.print("\t Analog Channel 4 : ");
  Serial.println(Analog_Value_4);

  delay(1000);
}
