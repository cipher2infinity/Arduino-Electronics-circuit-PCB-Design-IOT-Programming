/*
 * Descreption :
 * 
 * This code reads the status of pin 66, 67, 68, 69 of Atmega256
 * and prints the status on serial monitor
 * 
 * For this code programing is done using internal registers 
 * because mentioned pins are not connected to female buck 
 * strip on Atmega256
 * 
 * Pins on ATmega256
 * PJ3 = 66
 * PJ4 = 67
 * PJ5 = 68
 * PJ6 = 69
 */

void setup() 
{
  //Defining pin 3, 4, 5, 6 of port J as Input pin
  DDRJ = DDRJ & ~(1 << 3) & ~(1 << 4) & ~(1 << 5) & ~(1 << 6);

  /*
   * Explaination for above statement
   *                                                             0b11111111         0b00000000    
  (1 << 3) = 0b00001000         ~(1 << 3) = 0b11110111           0b11110111         0b11110111
  (1 << 4) = 0b00010000         ~(1 << 4) = 0b11101111         & 0b11101111       & 0b11101111
  (1 << 5) = 0b00100000         ~(1 << 5) = 0b11011111         & 0b11011111       & 0b11011111
  (1 << 6) = 0b01000000         ~(1 << 6) = 0b10111111         & 0b10111111       & 0b10111111
                                                              ---------------    ---------------
                                                                 0b10000111         0b00000000
  */
  
  //Initializing the serial communication at 9600 baud rate
  Serial.begin(9600);
}

void loop() 
{
  //Checking 3rd pin of port J  ( Pin 66 on Atmega256)
  //PINJ & 0b00001000
  if(PINJ & (1<<3))
  {
    Serial.print("Pin 66 : High \t");
  }
  else
  {
    Serial.print("Pin 66 : Low \t");
  }

  //Checking 4th pin of port J  ( Pin 67 on Atmega256)
  //PINJ & 0b00010000
  if(PINJ & (1<<4))
  {
    Serial.print("Pin 67 : High \t");
  }
  else
  {
    Serial.print("Pin 67 : Low \t");
  }

  //Checking 5th pin of port J  ( Pin 68 on Atmega256)
  //PINJ & 0b00100000
  if(PINJ & (1<<5))
  {
    Serial.print("Pin 68 : High \t");
  }
  else
  {
    Serial.print("Pin 68 : Low \t");
  }

  //Checking 6th pin of port J  ( Pin 69 on Atmega256)
  //PINJ & 0b01000000
  if(PINJ & (1<<6))
  {
    Serial.println("Pin 69 : High");
  }
  else
  {
    Serial.println("Pin 69 : Low");
  }

  delay(1000);
}
