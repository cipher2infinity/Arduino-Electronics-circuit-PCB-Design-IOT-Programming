/*
 * Descreption :
 * 
 * This code enables the WiFi module to connect to the predefined
 * WiFi network and later it connects to the dweet.io website.
 * 
 * After that it sends the data to dweet.io website which later sends 
 * it to freeboard.io website
 * 
 * This is the format of data (Link) and this data is sent as a HTTP request.
 * https://dweet.io/dweet/for/IndustrialDataLogger?Date=5-8-2019&Time=11:22:21&Day=Sat&A1=3.3&A2=2.5&A3=2.5&A4=5&D1=300&D2=2.5&D3=2.5&D4=2.5&S1=1.5&S2=1.5&BLE=1&WiFi=0&SD=1&R1=0&R2=1
 * 
 */


/*
 * 
 * Header files 
 */
#include <Wire.h>                 //Library for I2C Interface
#include "DS1307.h"               //Library for DS1307 IC (RTC)
#include <Adafruit_ADS1015.h>     //Library for ADS1115 IC (16 Bit ADC)
#include <compat/deprecated.h>    //Library to read single pin of perticular port
#include <WiFi101.h>              //Library for ATWINC1510 WiFi module
#include <SPI.h>                  //Library for SPI Interface


/*
 * 
 * Defining objects of diffrent class
 */
DS1307 clock;                     //Defining object of DS1307 class
Adafruit_ADS1115 ads;             //Defining object of Adafruit_ADS1115 class
WiFiClient client;                //Defining object of WiFiClient class


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


//Variables for SD card Module
const byte SD_Status_Pin = 4;


//Variables for ADC
int16_t adc0, adc1;
float Voltage;


//Variables for pin change interrupt program
volatile uint32_t Pul_PJ3=0, Pul_PJ4=0, Pul_PJ5=0, Pul_PJ6=0;
uint32_t Pulse_count[4];   


//Variables for WiFi module
byte status, pingResult;
bool previoslyConnected;
int  Result;
char hostName[20] = "www.google.com";
char hostName1[20] = "dweet.io";
const byte httpPort    = 80;

//Digital pins for WIFI  
const byte WIFI_Reset_Pin    =   8;  
const byte WIFI_ChipSel_Pin  =   53; 
const byte WIFI_IRQ_Pin      =   2; 

//Wifi credentials
char ssid[] = "Piyush";      // your network SSID (name)
char pass[] = "Piyush1234";    // your network password (use for WPA, or use as key for WEP)


//Buffers for diffrent purpose  
char FreeBoard_API_String[300];
char Temp_Buffer[10];
const char FreeBoar_API_Link[50] = "https://dweet.io/dweet/for/IndustrialDataLogger?"; 

/*
 * Setup Code
 */
void setup() 
{
  //Initializing UART0
  Serial.begin(115200);

  Serial.println("System Reset");


  //Initializing Digital Inputs for Relay
  pinMode(Relay_1_Pin, OUTPUT);           // set pin to input
  pinMode(Relay_2_Pin, OUTPUT);           // set pin to input


  //Initializing RTC and setting RTC Parameters
  clock.begin();
  clock.fillByYMD(2019,6,5);              //Jan 19,2013
  clock.fillByHMS(4,45,0);                //6:08:0"
  clock.fillDayOfWeek(WED);               //Saturday
  clock.setTime();                        //write RTC parameters to chip
  

  //Initializing ADS1115
  ads.begin();


  //Initializing PCINT functionality on PCINT12, PCINT13, PCINT14, PCINT15
  sei();                                  // turn on global interrupts
  PCICR =  (1 << PCIE1);                  // set PCIE1 PCMSK1 scan
  PCMSK1 = (1 << PCINT12) | (1 << PCINT13) | (1 << PCINT14) | (1 << PCINT15) ;   // set PCINT12, PCINT13, PCINT14, PCINT15 to trigger an interrupt on state change 


  //Initializing WiFi module and connecting to WiFi module
  Connect_To_WiFi_Network();
}

//Original String
//https://dweet.io/dweet/for/IndustrialDataLogger?Date=5-8-2019&Time=11:22:21&Day=Sat&A1=3.3&A2=2.5&A3=2.5&A4=5&D1=300&D2=2.5&D3=2.5&D4=2.5&S1=1.5&S2=1.5&BLE=1&WiFi=0&SD=1&R1=0&R2=1

//HTTP Request
//POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&D1=0&D2=0&D3=0&D4=0&S1=0.0380&S2=0.0184&BLE=0&WiFi=1&SD=1&R1=0&R2=0


/*
 * Main Loop
 */
void loop() 
{
  strcpy(FreeBoard_API_String, "POST /dweet/for/IndustrialDataLogger?");

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

  //Reading Status of WIFI module
  Read_WIFI_Status();

  //Reading Status of SD card module
  Read_SD_Status();

  //Reading 0-10v Digital Input Channel
  Read_Relay_Status();

  //Sending Data over RS232 interface with checksum byte
  SendDataToFreeBoard(FreeBoard_API_String);

  //Serial.println(FreeBoard_API_String);

  delay(3000);
}


/*
 * Function to read RTC parameters
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&
 */
void Read_RTC_Parameters()
{
  clock.getTime();                                //Getting time from DS1307 RTC IC

  strcat(FreeBoard_API_String, "Date=");
  
  itoa(clock.month, Temp_Buffer, 10);             //Converting integer value to ascii charecter and storing it in Temp_Buffer 
  strcat(FreeBoard_API_String, Temp_Buffer);      //Storing temperory buffer in FreeBoard_API_String
  strcpy(Temp_Buffer, "");                        //Clearing temperory buffer to store new analog value
  strcat(FreeBoard_API_String, "-");              //Including - in FreeBoard_API_String

  itoa(clock.dayOfMonth, Temp_Buffer, 10);
  strcat(FreeBoard_API_String, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(FreeBoard_API_String, "-");

  itoa(clock.year+2000, Temp_Buffer, 10);
  strcat(FreeBoard_API_String, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(FreeBoard_API_String, "&");

  strcat(FreeBoard_API_String, "Time=");

  itoa(clock.hour, Temp_Buffer, 10);
  strcat(FreeBoard_API_String, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(FreeBoard_API_String, ":");

  itoa(clock.minute, Temp_Buffer, 10);
  strcat(FreeBoard_API_String, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(FreeBoard_API_String, ":");

  itoa(clock.second, Temp_Buffer, 10);
  strcat(FreeBoard_API_String, Temp_Buffer);
  strcpy(Temp_Buffer, "");
  strcat(FreeBoard_API_String, "&");

  strcat(FreeBoard_API_String, "Day=");

  switch (clock.dayOfWeek)
  {
    case MON:
      strcat(FreeBoard_API_String, "MON&");
      break;
    case TUE:
      strcat(FreeBoard_API_String, "TUE&");
      break;
    case WED:
      strcat(FreeBoard_API_String, "WED&");
      break;
    case THU:
      strcat(FreeBoard_API_String, "THU&");
      break;
    case FRI:
      strcat(FreeBoard_API_String, "FRI&");
      break;
    case SAT:
      strcat(FreeBoard_API_String, "SAT&");
      break;
    case SUN:
      strcat(FreeBoard_API_String, "SUN&");
      break;
  }
}


/*
 * Function to read 0-10v Analog Value
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&
 */
void Read_Analog_Input_Channel()
{
  for(uint8_t i = 0; i < 4; i++)
  {
    //Storing temperory buffer in FreeBoard_API_String
    strcat(FreeBoard_API_String, "A");

    //Converting float value to charecter and storing it in temoprary buffer
    itoa(i + 1, Temp_Buffer, 10);
  
    //Storing temperory buffer in FreeBoard_API_String
    strcat(FreeBoard_API_String, Temp_Buffer);
  
    //Clearing temperory buffer to store new analog value
    strcpy(Temp_Buffer, ""); 
    strcat(FreeBoard_API_String, "=");
    
    //Reading Analog input channel
    Analog_Value = analogRead(Analog_Input_Pins[i]); 
               
    //Converting Analog value to voltage
    Analog_Voltage = Analog_Value * 0.0048828 * 2;

    //Converting float value to charecter and storing it in temoprary buffer
    dtostrf(Analog_Voltage, 6, 4, Temp_Buffer);                  

    //Storing temperory buffer in FreeBoard_API_String
    strcat(FreeBoard_API_String, Temp_Buffer);   

    //Clearing temperory buffer to store new analog value
    strcpy(Temp_Buffer, "");

    //Including comma in FreeBoard_API_String
    strcat(FreeBoard_API_String, "&");                              
  }
}


/*
 * Function to count pulses on PCINT12, PCINT13, PCINT14, PCINT15 
 * (Pin Number 66, 67, 68, 69 on Atmega256)
 * Store count values in Count[0], Count[1], Count[2], Count[3]
 */
ISR (PCINT1_vect)                   
{
  //Checking if their is logic high on pin no 3 of Port J (pcint12)
  //If logic high is present then increment the count
  if( bit_is_set(PINJ,3) )
  {
      Pul_PJ3++;                  
      Pulse_count[0] = Pul_PJ3;   
      Serial.println(Pul_PJ3);
  }

  //Checking if their is logic high on pin no 4 of Port J (PCINT13)
  //If logic high is present then increment the count 
  if( bit_is_set(PINJ,4) )
  {
      Pul_PJ4++;                 
      Pulse_count[1] = Pul_PJ4; 
      Serial.println(Pul_PJ4);
  }

  //Checking if their is logic high on pin no 5 of Port J (PCINT14)
  //If logic high is present then increment the count 
  if( bit_is_set(PINJ,5) )
  {
      Pul_PJ5++;                  
      Pulse_count[2] = Pul_PJ5;  
      Serial.println(Pul_PJ5); 
  }

  //Checking if their is logic high on pin no 6 of Port J (PCINT15)
  //If logic high is present then increment the count 
  if( bit_is_set(PINJ,6) )
  {
      Pul_PJ6++;                  
      Pulse_count[3] = Pul_PJ6;  
      Serial.println(Pul_PJ6);
  }
}


/*
 * Function to construct string for external pulses 
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&D1=0&D2=0&D3=0&D4=0&
 */
//Function to construct string for external pulses 
void Read_Pulse_Count_Digital_Input()
{
  for(uint8_t i = 0; i < 4; i++)
  {
    //Storing temperory buffer in FreeBoard_API_String
    strcat(FreeBoard_API_String, "D");

    //Converting float value to charecter and storing it in temoprary buffer
    itoa(i + 1, Temp_Buffer, 10);
  
    //Storing temperory buffer in FreeBoard_API_String
    strcat(FreeBoard_API_String, Temp_Buffer);
  
    //Clearing temperory buffer to store new analog value
    strcpy(Temp_Buffer, ""); 
    strcat(FreeBoard_API_String, "=");
    
    //Converting float value to charecter and storing it in temoprary buffer
    itoa(Pulse_count[i], Temp_Buffer, 10);
  
    //Storing temperory buffer in FreeBoard_API_String
    strcat(FreeBoard_API_String, Temp_Buffer);
  
    //Clearing temperory buffer to store new analog value
    strcpy(Temp_Buffer, "");
  
    //Including comma in FreeBoard_API_String
    strcat(FreeBoard_API_String, "&");
  } 
}


/*
 * Function to read Read_4_20_mA_Sensor
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&D1=0&D2=0&D3=0&D4=0&S1=0.0380&S2=0.0184&
 */
//Function to read Read_4_20_mA_Sensor
void Read_4_20_mA_Sensor()
{
  //Storing temperory buffer in FreeBoard_API_String
  strcat(FreeBoard_API_String, "S1=");
  
  //Reading 4-20 mA Sensor (Channel 1)
  adc0 = ads.readADC_SingleEnded(0);

  Voltage = ((5 * adc0) * 0.00003761);

  //Converting float value to charecter and storing it in temoprary buffer
  dtostrf(Voltage, 6, 4, Temp_Buffer);

  //Storing temperory buffer in FreeBoard_API_String
  strcat(FreeBoard_API_String, Temp_Buffer);

  //Clearing temperory buffer to store new analog value
  strcpy(Temp_Buffer, "");

  //Including comma in FreeBoard_API_String
  strcat(FreeBoard_API_String, "&");


  //Storing temperory buffer in FreeBoard_API_String
  strcat(FreeBoard_API_String, "S2=");
  
  //Reading 4-20 mA Sensor (Channel 2)
  adc1 = ads.readADC_SingleEnded(1);
  
  Voltage = ((5 * adc1) * 0.00003761);

  //Converting float value to charecter and storing it in temoprary buffer
  dtostrf(Voltage, 6, 4, Temp_Buffer);

  //Storing temperory buffer in FreeBoard_API_String
  strcat(FreeBoard_API_String, Temp_Buffer);

  //Clearing temperory buffer to store new analog value
  strcpy(Temp_Buffer, "");

  //Including comma in FreeBoard_API_String
  strcat(FreeBoard_API_String, "&");
}


/*
 * Function to read status of BLE module
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&D1=0&D2=0&D3=0&D4=0&S1=0.0380&S2=0.0184&BLE=0&
 */
void Read_BLE_Status()
{
  //Reading status of Relay 1
  Status = digitalRead(BLE_Status_Pin);

  if(Status == 1)
  {
    strcat(FreeBoard_API_String, "BLE=1");
  }
  else
  {
    strcat(FreeBoard_API_String, "BLE=0");
  }
  
  //Including comma in FreeBoard_API_String
  strcat(FreeBoard_API_String, "&");
}


/*
 * Function to read status of WiFi module
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&D1=0&D2=0&D3=0&D4=0&S1=0.0380&S2=0.0184&BLE=0&WiFi=1&
 */
//Function to check status of WIFI Network
void Read_WIFI_Status()
{    
  if(WiFi.status() == WL_CONNECTED )
  {
    strcat(FreeBoard_API_String, "WiFi=1"); 
  }
  else
  {
    strcat(FreeBoard_API_String, "WiFi=0");
  }

  //Including comma in FreeBoard_API_String
  strcat(FreeBoard_API_String, "&");
}


/*
 * Function to read status of wiFi module
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&D1=0&D2=0&D3=0&D4=0&S1=0.0380&S2=0.0184&BLE=0&WiFi=1&SD=1&
 */
//Function to read BLE status
void Read_SD_Status()
{
  
  //Reading status of Relay 1
  Status = digitalRead(SD_Status_Pin);

  if(Status == 1)
  {
    strcat(FreeBoard_API_String, "SD=0");
  }
  else
  {
    strcat(FreeBoard_API_String, "SD=1");
  }
  
  //Including comma in FreeBoard_API_String
  strcat(FreeBoard_API_String, "&");
}


/*
 * Function to read Relay State
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&D1=0&D2=0&D3=0&D4=0&S1=0.0380&S2=0.0184&BLE=0&WiFi=1&SD=1&R1=0&R2=0
 */
void Read_Relay_Status()
{
  //Reading status of Relay 1
  Status = digitalRead(Relay_1_Pin);

  if(Status == 1)
  {
    strcat(FreeBoard_API_String, "R1=1");
  }
  else
  {
    strcat(FreeBoard_API_String, "R1=0");
  }
  
  //Including comma in FreeBoard_API_String
  strcat(FreeBoard_API_String, "&");

  //Reading status of Relay 2
  Status = digitalRead(Relay_2_Pin);

  if(Status == 1)
  {
    strcat(FreeBoard_API_String, "R2=1");
  }
  else
  {
    strcat(FreeBoard_API_String, "R2=0");
  }
}


/*
 * Function to send data over RS232 Interface with CheckSum Byte
 * POST /dweet/for/IndustrialDataLogger?Date=5-4-2019&Time=6:19:11&Day=SAT&A1=0.0000&A2=0.0098&A3=0.0098&A4=0.0000&D1=0&D2=0&D3=0&D4=0&S1=0.0380&S2=0.0184&BLE=0&WiFi=1&SD=1&R1=0&R2=0
 */
//Function to send data over RS232 Interface with CheckSum Byte 
void SendDataToFreeBoard(char *DataString)
{  
  if (client.connect(hostName1, httpPort))
  {
    Serial.println();
    Serial.println("Host Connected");

    strcat(FreeBoard_API_String, " HTTP/1.1\r\n");
    strcat(FreeBoard_API_String, "Host: ");
    strcat(FreeBoard_API_String, hostName1);
    strcat(FreeBoard_API_String, "\r\n");
    strcat(FreeBoard_API_String, "Connection: close\r\n\r\n");
    
    Serial.println();
    Serial.print("String Length: ");
    Serial.println(strlen(FreeBoard_API_String));
    Serial.println(FreeBoard_API_String);
    
    client.println(FreeBoard_API_String);
    client.stop();
    Serial.println("Sent!!!!!!!");
  }
  else
  {
    Serial.println(F("Connection Failed"));
    Serial.print(F("Pinging Google: ")); 
    pingResult = WiFi.ping(hostName); //hostName
  
    if (pingResult >= 0) 
    {
      Serial.print(F("SUCCESS! RTT = "));
      Serial.print(pingResult);
      Serial.println(" ms");
    } 
    else 
    {
      Serial.print(F("FAILED! Error code: "));
      Serial.println(pingResult);
      
      status = WiFi.status();
      if(status != WL_CONNECTED)
      {
        if(previoslyConnected == true) 
        WiFi.end();
        Connect_To_WiFi_Network();
      }          
    }
  }
}



/*
 * Function to connect Wifi Module
 */
void Connect_To_WiFi_Network()
{
   //Defining Local variable
   byte WiFi_Connection_Attempt_Count = 0;

   //Defining pin number for chipselect, Interrupt and reset pin of WiFi module
   WiFi.setPins(WIFI_ChipSel_Pin, WIFI_IRQ_Pin, WIFI_Reset_Pin);

  
   Serial.println("** Scanning Available Networks **");

   //Scanning all the available network
   byte Available_Network_Count = WiFi.scanNetworks();
  
   //Displaying all the available WiFi network
   Serial.print("Total Available Networks: ");
   Serial.println(Available_Network_Count);
   Serial.println();
  
   //Displaying Parameters of available WiFi Network 
   for(uint8_t i = 0; i < Available_Network_Count; i++) 
   { 
     //Displaying number of network       
     Serial.print(i);
     Serial.print(") ");

     //Displaying SSID of WiFi Network
     Serial.print("SSID: ");
     Serial.println(WiFi.SSID(i));
     
     //Displaying Recieved Signal Strength of WiFi Network
     Serial.print("   Signal Strength: ");
     Serial.print(WiFi.RSSI(i));
     Serial.println(" dBm");

     //Displaying type of encryption for WiFi Network
     Serial.print("   Encryption Type: ");
     Serial.println(WiFi.encryptionType(i));

     Serial.println();
   }
   
   
  
   //Connecting to predefined SSID (Maximum 5 Attempts)
   if(WiFi.status() != WL_CONNECTED)
   {
     //Initializing the count
     WiFi_Connection_Attempt_Count = 0;

     //Stay in loop if not connected to WiFi Network and if connection attempts is less than 5
     while (WiFi.status() != WL_CONNECTED && WiFi_Connection_Attempt_Count < 5 ) 
     {       
       Serial.print("Attempting to connect to SSID: ");
       Serial.println(ssid);

       //Connecting to WiFi network
       status = WiFi.begin(ssid, pass);
       delay(5000);

       //Incrementing count
       WiFi_Connection_Attempt_Count++;
     }
   }


   //Enter only if succesfully connected to predefined Network     
   if(WiFi.status() == WL_CONNECTED)
   {
     Serial.println();
     Serial.println("*****Connected to WiFi Network*****");

     //Displaying the status of WiFi network
     printWiFiStatus();
       
     Serial.print("Ping Google: "); 
     
     //Trying to connect to googles server to check internet is available or not
     Result = WiFi.ping(hostName); 
     
     if(Result >= 0) 
     {
       Serial.print("SUCCESS! RTT = ");

       //Displaing the time taken to get the responce from the google server
       Serial.print(Result);
       Serial.println(" ms");
     }
     else 
     {
       Serial.print("FAILED! Error code: ");
       Serial.println(Result);
     }
    }
    else
    {
      Serial.print("Failed to connect!");

      //Try to connect one more time
      Connect_To_WiFi_Network();
    }
}


/*
 * Function to print parameters of connected SSID
 */
void printWiFiStatus() 
{  
  //Displaying SSID of connected Network
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  //Displaying IP address allocated to WiFi Module
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  //Displying signal strength of recieved signal
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
