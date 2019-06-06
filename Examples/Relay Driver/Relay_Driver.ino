/*
 * Descreption :
 * 
 * This code drives (Switch ON/OFF) the relay connected to relay driver circuit 
 * with a delay of 5 Sec
 * 
 * Relay Driver circuit_1's input is connected to pin 36 of Arduino Mega
 * Relay Driver circuit_2's input is connected to pin 37 of Arduino Mega
 * 
 */

 
//Defining the pins for relay
const byte Relay_1 = 36;        //Pin 54 on Atmega256
const byte Relay_2 = 37;        //Pin 53 on Atmega256

void setup() 
{
  //Defining Relay_1 and Relay_2 pin's as output 
  pinMode(Relay_1, OUTPUT);
  pinMode(Relay_2, OUTPUT);
}

void loop() 
{
  //Setting Relay_1 and Relay_2 pin's to High state
  digitalWrite(Relay_1, HIGH);
  digitalWrite(Relay_2, HIGH);

  //Delay of one second
  delay(5000);

  //Setting Relay_1 and Relay_2 pin's to Low state
  digitalWrite(Relay_1, LOW);
  digitalWrite(Relay_2, LOW);

  //Delay of one second
  delay(5000);
}
