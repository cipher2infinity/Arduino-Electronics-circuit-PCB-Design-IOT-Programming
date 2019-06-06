/*
 * Descreption :
 * 
 * This code reads all the parameters of Datalogger
 * 
 * RTC_Parameters, 0-10v Analog Input Readings, 0-10 V Digital Input readings,
 * 4-20 mA sensor readings, Satus of BLE, Status of WiFi, Status of SD card,
 * Status of Relay driver circuit
 * 
 * Later construct a string which is seperated by comma and the send it 
 * over ble module (UART2).
 */


/*
 * 
 * Header files 
 */
#include <Wire.h>                 //Library for I2C Interface
#include "DS1307.h"               //Library for DS1307 IC (RTC)
#include <Adafruit_ADS1015.h>     //Library for ADS1115 IC (16 Bit ADC)


/*
 * 
 * Defining objects of diffrent class
 */
DS1307 clock;                     //Defining object of DS1307 class
Adafruit_ADS1115 ads;             //Defining object of Adafruit_ADS1115 class


/*
 * 
 * Variable declaration
 */
//General variables
uint16_t Analog_Value = 0; 
bool Status = 0;


//Variables for 0-10v Analog Input sensor
static const uint8_t Analog_Input_Pins[] = {A12,A13,A14,A15};
float Analog_Voltage = 0; 


//Variables for Relay Driver
const byte Relay_1_Pin = 36;
const byte Relay_2_Pin = 37;


//Variables for BLE Module
const byte BLE_Status_Pin = 38;


//Variables for ADC
int16_t adc0, adc1;
float Voltage;


//Variables for pin change interrupt program
volatile uint32_t Pul_PJ3=0, Pul_PJ4=0, Pul_PJ5=0, Pul_PJ6=0;
uint32_t Pulse_count[4];   


//Buffers for diffrent 
char BLE_DataBuffer[100];
char Temp_Buffer[10];


/*
 * Setup Code
 */
void setup() 
{
  //Initializing UART0 at 9600 baud rate
  Serial.begin(9600);

  //Initializing UART2 at 9600 baud rate
  Serial2.begin(9600);

  Serial.println("System Reset");


  //Initializing Digital Inputs for Relay
  pinMode(Relay_1_Pin, OUTPUT);           // set pin to input
  pinMode(Relay_2_Pin, OUTPUT);           // set pin to input


  //Initializing RTC and setting RTC Parameters
  clock.begin();
  clock.fillByYMD(2019,5,4);              //Setting Year, Month and Day
  clock.fillByHMS(6,18,0);                //Setting Hour, Min and Sec
  clock.fillDayOfWeek(SAT);               //Setting day
  clock.setTime();                        //Sending parameters to RTC
  

  //Initializing ADS1115
  ads.begin();


  //Initializing PCINT functionality on PCINT12, PCINT13, PCINT14, PCINT15
  sei();                                  // turn on global interrupts
  PCICR =  (1 << PCIE1);                  // set PCIE1 PCMSK1 scan
  PCMSK1 = (1 << PCINT12) | (1 << PCINT13) | (1 << PCINT14) | (1 << PCINT15) ;   // set PCINT12, PCINT13, PCINT14, PCINT15 to trigger an interrupt on state change 
  
}

/*
 * Main Loop
 */
void loop() 
{
  //Clearing RS232 DataBuffer
  strcpy(BLE_DataBuffer, "");

  //Reading RTC parameters
  Read_RTC_Parameters();
  
  //Reading 0-10v Analog Input Channel
  Read_Analog_Input_Channel();

  //Reading 0-10v Digital Input Channel
  Read_Pulse_Count_Digital_Input();

  //Readings 4-20mA sensor
  Read_4_20_mA_Sensor();
  
  //Reading Status of BLE module
  Read_BLE_Status();

  //Reading 0-10v Digital Input Channel
  Read_Relay_Status();

  //Sending the data to serial terminal
  Serial.println(BLE_DataBuffer);

  //Sending the data on BLE app
  Serial2.println(BLE_DataBuffer);

  delay(1000);
}


/*
 * Function to read RTC parameters
 * MM/DD/YYYY,HH:MM:SS,DAY,
 */
void Read_RTC_Parameters()
{
  
  clock.getTime();                                //Getting time from DS1307 RTC IC
  itoa(clock.month, Temp_Buffer, 10);             //Converting integer value to ascii charecter and storing it in Temp_Buffer 
  strcat(BLE_DataBuffer, Temp_Buffer);          //Storing temperory buffer in BLE_DataBuffer
  strcpy(Temp_Buffer, "");                        //Clearing temperory buffer to store new analog value
  strcat(BLE_DataBuffer, "/");                  //Including / in BLE_DataBuffer

  itoa(clock.dayOfMonth, Temp_Buffer, 10);
  strcat(BLE_DataBuffer, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(BLE_DataBuffer, "/");

  itoa(clock.year+2000, Temp_Buffer, 10);
  strcat(BLE_DataBuffer, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(BLE_DataBuffer, ",");

  itoa(clock.hour, Temp_Buffer, 10);
  strcat(BLE_DataBuffer, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(BLE_DataBuffer, ":");

  itoa(clock.minute, Temp_Buffer, 10);
  strcat(BLE_DataBuffer, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(BLE_DataBuffer, ":");

  itoa(clock.second, Temp_Buffer, 10);
  strcat(BLE_DataBuffer, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(BLE_DataBuffer, ",");

  switch (clock.dayOfWeek)
  {
    case MON:
      strcat(BLE_DataBuffer, "MON,");
      break;
    case TUE:
      strcat(BLE_DataBuffer, "TUE,");
      break;
    case WED:
      strcat(BLE_DataBuffer, "WED,");
      break;
    case THU:
      strcat(BLE_DataBuffer, "THU,");
      break;
    case FRI:
      strcat(BLE_DataBuffer, "FRI,");
      break;
    case SAT:
      strcat(BLE_DataBuffer, "SAT,");
      break;
    case SUN:
      strcat(BLE_DataBuffer, "SUN,");
      break;
  }
}


/*
 * Function to read 0-10v Analog Value
 * HH:MM:SS,MM/DD/YYYY,DAY,A1,A2,A3,A4,
 */
void Read_Analog_Input_Channel()
{
  for(uint8_t i = 0; i < 4; i++)
  {
    //Reading Analog input channel
    Analog_Value = analogRead(Analog_Input_Pins[i]); 
               
    //Converting Analog value to voltage
    Analog_Voltage = Analog_Value * 0.0048828 * 2;

    //Converting float value to charecter and storing it in temoprary buffer
    dtostrf(Analog_Voltage, 6, 4, Temp_Buffer);                  

    //Storing temperory buffer in BLE_DataBuffer
    strcat(BLE_DataBuffer, Temp_Buffer);   

    //Clearing temperory buffer to store new analog value
    strcpy(Temp_Buffer, "");

    //Including comma in BLE_DataBuffer
    strcat(BLE_DataBuffer, ",");                                
  }
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


/*
 * Function to construct string for external pulses 
 * HH:MM:SS,MM/DD/YYYY,DAY,A1,A2,A3,A4,D1,D2,D3,D4,
 */
//Function to construct string for external pulses 
void Read_Pulse_Count_Digital_Input()
{
  for(uint8_t i = 0; i < 4; i++)
  {
    //Converting float value to charecter and storing it in temoprary buffer
    itoa(Pulse_count[i], Temp_Buffer, 10);
  
    //Storing temperory buffer in BLE_DataBuffer
    strcat(BLE_DataBuffer, Temp_Buffer);
  
    //Clearing temperory buffer to store new analog value
    strcpy(Temp_Buffer, "");
  
    //Including comma in BLE_DataBuffer
    strcat(BLE_DataBuffer, ",");
  } 
}


/*
 * Function to read Read_4_20_mA_Sensor
 * HH:MM:SS,MM/DD/YYYY,DAY,A1,A2,A3,A4,D1,D2,D3,D4,S1,S2,
 */
//Function to read Read_4_20_mA_Sensor
void Read_4_20_mA_Sensor()
{
  //Reading 4-20 mA Sensor (Channel 1)
  adc0 = ads.readADC_SingleEnded(0);

  Voltage = ((5 * adc0) * 0.00003761);

  //Converting float value to charecter and storing it in temoprary buffer
  dtostrf(Voltage, 6, 4, Temp_Buffer);

  //Storing temperory buffer in BLE_DataBuffer
  strcat(BLE_DataBuffer, Temp_Buffer);

  //Clearing temperory buffer to store new analog value
  strcpy(Temp_Buffer, "");

  //Including comma in BLE_DataBuffer
  strcat(BLE_DataBuffer, ",");


  //Reading 4-20 mA Sensor (Channel 2)
  adc1 = ads.readADC_SingleEnded(1);
  
  Voltage = ((5 * adc1) * 0.00003761);

  //Converting float value to charecter and storing it in temoprary buffer
  dtostrf(Voltage, 6, 4, Temp_Buffer);

  //Storing temperory buffer in BLE_DataBuffer
  strcat(BLE_DataBuffer, Temp_Buffer);

  //Clearing temperory buffer to store new analog value
  strcpy(Temp_Buffer, "");

  //Including comma in BLE_DataBuffer
  strcat(BLE_DataBuffer, ",");
}


/*
 * Function to read status of BLE module
 * HH:MM:SS,MM/DD/YYYY,DAY,A1,A2,A3,A4,D1,D2,D3,D4,S1,S2,STA_BLE,
 */
void Read_BLE_Status()
{
  //Reading status of Relay 1
  Status = digitalRead(BLE_Status_Pin);

  if(Status == 1)
  {
    strcat(BLE_DataBuffer, "1");
  }
  else
  {
    strcat(BLE_DataBuffer, "0");
  }
  
  //Including comma in BLE_DataBuffer
  strcat(BLE_DataBuffer, ",");
}


/*
 * Function to read Relay State
 * HH:MM:SS,MM/DD/YYYY,DAY,A1,A2,A3,A4,D1,D2,D3,D4,S1,S2,STA_BLE,STA_WiFi,STA_SD,R1,R2
 */
void Read_Relay_Status()
{
  //Reading status of Relay 1
  Status = digitalRead(Relay_1_Pin);

  if(Status == 1)
  {
    strcat(BLE_DataBuffer, "1");
  }
  else
  {
    strcat(BLE_DataBuffer, "0");
  }
  
  //Including comma in BLE_DataBuffer
  strcat(BLE_DataBuffer, ",");

  //Reading status of Relay 2
  Status = digitalRead(Relay_2_Pin);

  if(Status == 1)
  {
    strcat(BLE_DataBuffer, "1");
  }
  else
  {
    strcat(BLE_DataBuffer, "0");
  }
  
  //Including comma in BLE_DataBuffer
  strcat(BLE_DataBuffer, ",");
}

  
