/*
 * Descreption :
 * 
 * This code initializes the RTC and later displays the 
 * RTC parameters on serial monitor
 */

#include <Wire.h>       //Library for I2C Interface
#include "DS1307.h"     //Library for RTC - DS1307

DS1307 clock;           //define a object of DS1307 class

/*
 * Setup Code
 */
void setup()
{
  //Initializing UART0 at 9600 baud rate
  Serial.begin(9600);

  //Initialization for DS1307
  clock.begin();
  clock.fillByYMD(2019,6,5);  //Setting year month and date in RTC
  clock.fillByHMS(12,41,0);   //Setting Hour, Min and Sec in RTC
  clock.fillDayOfWeek(WED);   //Setting Day in RTC
  clock.setTime();            //Sending all the parameters to RTC
}

/*
 * Main Loop
 */
void loop()
{ 
  //Displaying RTC parameters on serial monitor
  printTime();
 
  delay(1000);
}



/*Function: Display time on the serial monitor*/
void printTime()
{
  clock.getTime();
  Serial.print(clock.hour, DEC);
  Serial.print(":");
  Serial.print(clock.minute, DEC);
  Serial.print(":");
  Serial.print(clock.second, DEC);
  Serial.print("  ");
  Serial.print(clock.month, DEC);
  Serial.print("/");
  Serial.print(clock.dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(clock.year+2000, DEC);
  Serial.print(" ");
  
  switch (clock.dayOfWeek)// Friendly printout the weekday
  {
    case MON:
      Serial.print("MON");
      break;
    case TUE:
      Serial.print("TUE");
      break;
    case WED:
      Serial.print("WED");
      break;
    case THU:
      Serial.print("THU");
      break;
    case FRI:
      Serial.print("FRI");
      break;
    case SAT:
      Serial.print("SAT");
      break;
    case SUN:
      Serial.print("SUN");
      break;
  }
  Serial.println(" ");
}
