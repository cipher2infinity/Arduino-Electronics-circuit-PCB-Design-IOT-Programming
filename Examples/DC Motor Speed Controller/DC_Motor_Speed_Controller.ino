/*
 * Descreption :
 * 
 * This code controlles the speed of DC Motor with the PWM functionality
 * It controles the speed of DC Motor from ( 0 - Max and Max - 0 )
 * 
 * DC Motor speed controller circuit's input is connected to pin 6 of Arduino Mega
 * 
 */


//Defining pin for Motor speed controle
const byte MTR_Speed_Controle_Pin = 6;        //Pin 15 on Atmega256

/*
 * Setup Code
 */
void setup() 
{
  //Do nothing in setup
}

/*
 * Main Loop
 */
void loop() 
{
  //Decrease the value from min to max with the step of 5
  for (int Speed = 0 ; Speed <= 255; Speed += 5) 
  {
    // sets the value (range from 0 to 255):
    analogWrite(MTR_Speed_Controle_Pin, Speed);
    
    // wait for 300 milliseconds
    delay(300);
  }

  //Increase the value from max to min with the step of 5
  for (int Speed = 255 ; Speed >= 0; Speed -= 5) 
  {
    // sets the value (range from 0 to 255):
    analogWrite(MTR_Speed_Controle_Pin, Speed);
    
    // wait for 30 milliseconds
    delay(300);
  }
}
