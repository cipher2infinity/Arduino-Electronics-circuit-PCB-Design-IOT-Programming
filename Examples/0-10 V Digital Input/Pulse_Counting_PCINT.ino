/*
 * Descreption :
 * 
 * This code counts the pulses on Pin Number 66, 67, 68, 69 of Atmega256
 * and prints the pulse count on the serial monitor
 * 
 * This code uses Pin change interrupt functionality of ATmega256 to 
 * count the pulses on Pin Number 66, 67, 68, 69
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


//Variables for pin change interrupt program
//Stores the count of pulses on PCINT12, PCINT13, PCINT14, PCINT15 pin respectively.
byte Pul_PJ3=0, Pul_PJ4=0, Pul_PJ5=0, Pul_PJ6=0;


//Defining buffer to count number of pulses 
//Stores the pulse count of PCINT12, PCINT13, PCINT14, PCINT15 pin in the buffer
byte Pulse_count[4];   

/*
 * Setup Code
 */

void setup() 
{
  //Initialilizing serial communication on Uart0 at 9600 baud rate
  Serial.begin(9600);
  
  //Initializing PCINT functionality on PCINT12, PCINT13, PCINT14, PCINT15
  sei();                                  // turn on global interrupt bit in Status register (SREG)
  PCICR =  (1 << PCIE1);                  // set PCIE1 PCMSK1 scan    (1 << 1) = 0b00000010
  PCMSK1 = (1 << PCINT12) | (1 << PCINT13) | (1 << PCINT14) | (1 << PCINT15) ;   //0b11110000  // set PCINT12, PCINT13, PCINT14, PCINT15 to trigger an interrupt on state change 
}


/*
 * Main Loop
 */
void loop() 
{
  //Printing pulse count of all the 4 channel of 0-10V Input on datalogger
  for(uint8_t i = 0; i < 4; i++)
  {
    Serial.print("Pulse Count CH");
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(Pulse_count[i]);
    Serial.print("\t\t");
  }

  Serial.println();
  
  delay(1000);
}


/*
 * Function to count pulses on PCINT12, PCINT13, PCINT14, PCINT15 
 * (Pin Number 66, 67, 68, 69 on Atmega256)
 * Store count values in Count[0], Count[1], Count[2], Count[3]
 */
ISR (PCINT1_vect)                   //Interrupt service routin for pin change interrupt 1
{
  //Checking if their is logic high on pin no 3 of Port J (pcint12)
  //If logic high is present then increment the count
  if( bit_is_set(PINJ,3) )
  {
      Pul_PJ3++;                  
      Pulse_count[0] = Pul_PJ3;   
      //Serial.println(Pul_PJ3);
  }

  //Checking if their is logic high on pin no 4 of Port J (PCINT13)
  //If logic high is present then increment the count 
  if( bit_is_set(PINJ,4) )
  {
      Pul_PJ4++;                 
      Pulse_count[1] = Pul_PJ4; 
      //Serial.println(Pul_PJ4);
  }

  //Checking if their is logic high on pin no 5 of Port J (PCINT14)
  //If logic high is present then increment the count 
  if( bit_is_set(PINJ,5) )
  {
      Pul_PJ5++;                  
      Pulse_count[2] = Pul_PJ5;  
      //Serial.println(Pul_PJ5); 
  }

  //Checking if their is logic high on pin no 6 of Port J (PCINT15)
  //If logic high is present then increment the count 
  if( bit_is_set(PINJ,6) )
  {
      Pul_PJ6++;                  
      Pulse_count[3] = Pul_PJ6;  
      //Serial.println(Pul_PJ6);
  }
}
