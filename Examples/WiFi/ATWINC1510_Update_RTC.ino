/*
 * Descreption :
 * 
 * This code enables the WiFi module to conncet to the predefined
 * WiFi network and later it program the wrong date inside the RTC.
 * 
 * After that it fetches the Time and Date from the cloud by using 
 * two diffrent API links by sending HTTP request to Thing Speak.com
 * 
 * 1. GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=SYHSST4VZ9KNQR1L
 * 2. GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=9QT023WUBZ6XCMSL
 * 
 * After fetching the correct date and time It program it in RTC module
 * 
 * Later it displays the RTC parameters on serial terminal with a delay of one sec.
 */


/*
 * 
 * Header files 
 */
#include <WiFi101.h>              //Library for ATWINC1510 WiFi module
#include <SPI.h>                  //Library for SPI Interface
#include <Wire.h>                 //Library for I2C interface
#include "DS1307.h"               //Library for DS1307 I2C IC


/*
 * 
 * Defining objects of diffrent class
 */
WiFiClient client;                //Defining object of WiFiClient class
DS1307 clock;                     //Defining object of DS1307 class


/*
 * 
 * Variables for WiFi module
 */
byte status;
int  Result;
char hostName[20] = "www.google.com";

//Digital pins for WIFI  
const byte WIFI_Reset_Pin    =   8;                             //Defining Reset pin for WiFi module
const byte WIFI_ChipSel_Pin  =   53;                            //Defining ChipSelect pin for WiFi module
const byte WIFI_IRQ_Pin      =   2;                             //Defining Interrupt pin for WiFi module

//Wifi credentials
const char ssid[] = "Piyush";                                   //Defining SSID of WiFi Network
const char pass[] = "Piyush1234";                               //Defining Password of WiFi Network

//Defining Buffers  
char API_Responce[50]               = {0};                      //Defining buffer to store API responce
char Temp_Buffer[10]                = {0};                      //Defining buffer for general purpose
char ThingSpeak_API_String[90]      = {0};                      //Defining buffer to store constructed api string
const char Date_APIKey[]            = "SYHSST4VZ9KNQR1L";       //Defining API key to fetch Date from cloud
const char Time_APIKey[]            = "9QT023WUBZ6XCMSL";       //Defining API key to fetch Time from cloud
const char Date_Time_API_String[70] = "GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=";        //Defining API link to fetch Date and Time from cloud




/*
 * 
 * Setup
 */
void setup() 
{
  //Initializing UART0 at a baud rate of 115200
  Serial.begin(115200);

  //Displaying messege to indicate system is reset
  Serial.println("System Reset");
  Serial.println();


  //Initializing WiFi module and connecting to WiFi module
  Connect_To_WiFi_Network();


  //Initializing RTC IC and setting the RTC parameters
  clock.begin();
  clock.fillByYMD(2013,1,19);       //Setting Year, Month, and Date for RTC
  clock.fillByHMS(7,28,30);         //Setting Hour, Min and Sec for RTC
  clock.fillDayOfWeek(SAT);         //Setting Day for RTC
  clock.setTime();                  //Sending RTC parameters to RTC


  //Displaying the RTC parameters before updating them
  Serial.println();
  Serial.println();
  Serial.println("*****RTC parameters before updating*****");
  printTime();
  Serial.println("               **********               ");
  Serial.println();
}

/*
 * 
 * Main Loop
 */
void loop() 
{
  Serial.println();

  Serial.print("********** Recieving data from internet *********");

  //Fetching Time from Cloud using API
  Fetch_API_Data(Time_APIKey);


  //Updating RTC time with the time fetched from cloud
  Updating_RTC_Time();


  //Clearing API responce Buffer
  for(uint8_t i=0; i<50;i++)
  {
    API_Responce[i]  =  0;
  }
  
  delay(1000);


  //Fetching Date from Cloud using API
  Fetch_API_Data(Date_APIKey); 


  //Updating RTC Date and Day with the Date and Day fetched from cloud
  Updating_RTC_Date();


  //Clearing API responce Buffer
  for(uint8_t i=0; i<50;i++)
  {
    API_Responce[i]  =  0;
  }
  
  Serial.println("           ********** Done *********           ");
  Serial.println();


  //Displaying the RTC parameters After updating them
  Serial.println();
  Serial.println();
  Serial.println("*****RTC parameters After updating*****");
  Serial.println();
  
  while(1)
  {
    //Displaying RTC parameters on Serial monitor
    printTime();

    delay(1000);
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


/*
 * Function to fetch Date, Time and Day parameters from API link
 */
void Fetch_API_Data(char *API_Key)
{
  uint8_t API_Responce_Count = 0;
  char Recieved_Byte = 0;
  int WaitingTime = 0;
  
  if(client.connect("184.106.153.149",80))
  {
    Serial.println();
    Serial.println("Connected to Client");

    //Constructing API string
    //GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=API_Key (76RGQ1AGEGT31XBY or TIOXS6GSUABJ5AWF)
    strcpy(ThingSpeak_API_String, Date_Time_API_String);
    strcat(ThingSpeak_API_String, API_Key);

    Serial.println(ThingSpeak_API_String);

    //Sending HTTP request to the connected server
    client.println(ThingSpeak_API_String);    
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();
  
    //Wait till the client is not available
    while(!client.available()) 
    {
      delay(1);
      WaitingTime++;
  
      //Stop the client
      if(WaitingTime > 10000) 
      {
        client.stop();
        Serial.println("\r\nTimeout");
        return 0;
      }
    }
  
  
    // If there are incoming bytes available
    // from the client, read them and store them in API responce buffer:
    while(client.available())
    { 
      //Read the data sent by the client
      Recieved_Byte = client.read();

      //Storing the recieved data in API Responce buffer and incrementing the count
      API_Responce[API_Responce_Count]  =  Recieved_Byte;
      API_Responce_Count++; 
    }

    //Displaying the clients responce
    Serial.println(API_Responce);


    //Disconnecting from the client
    client.stop();  
  }
  else
  {
    Serial.println("Connection Failed");

    Connect_To_WiFi_Network();
  }
}

/*
 * Function to update RTC time with the time fetched from cloud
 */
void Updating_RTC_Time()
{
  //Local Variable
  char *SplitedData;
  byte SplitedData_Count = 0;
  byte Hour = 0, Min = 0, Sec = 0;
  
  //Here we are extracting Month, Date, Year from the recieved string
  SplitedData = strtok(API_Responce, ":");

  while(SplitedData != NULL)
  { 
    if(SplitedData_Count == 0)
    {
      Hour = atoi(SplitedData);
    }
    else if(SplitedData_Count == 1)
    {
      Min = atoi(SplitedData);
    }
    else if(SplitedData_Count == 2)
    {
      Sec = atoi(SplitedData);
    }

    SplitedData_Count++;

    SplitedData = strtok(NULL, ": ");
  }

  clock.fillByHMS(Hour,Min,Sec);
}

/*
 * Function to update Date with the date fetched from cloud
 */
void Updating_RTC_Date()
{
  //Local Variable
  char *SplitedData;
  byte SplitedData_Count = 0;
  char Day[4] = {0};
  char Buffer[4] = {0};
  byte Month = 0, Date = 0;
  uint16_t Year = 0;
  
  //Here we are extracting Month, Date, Year from the recieved string
  SplitedData = strtok(API_Responce, ",");

  while(SplitedData != NULL)
  {     
    if(SplitedData_Count == 0)
    {
      strncpy(Day, strupr(SplitedData), 3);   //MON

      clock.fillDayOfWeek(GetDayValue(Day));
    }
    else if(SplitedData_Count == 1)
    {
      strncpy(Buffer, SplitedData, 3);

      Month = GetMonthValue(Buffer);
    }
    else if(SplitedData_Count == 2)
    {
      Date = atoi(SplitedData);
    }
    else if(SplitedData_Count == 3)
    {
      Year = atoi(SplitedData);
    }

    SplitedData_Count++;

    SplitedData = strtok(NULL, ", ");
  }

  clock.fillByYMD(Year, Month, Date);

  clock.setTime();
}

/*
 * Function to get integer value for corresponding day
 */
byte GetDayValue(char *Day)
{
  if(strcmp("MON", Day) == 0) return 1;
  else if(strcmp("TUE", Day) == 0)  return 2;
  else if(strcmp("WED", Day) == 0)  return 3;
  else if(strcmp("THU", Day) == 0)  return 4;
  else if(strcmp("FRI", Day) == 0)  return 5;
  else if(strcmp("SAT", Day) == 0)  return 6;
  else if(strcmp("SUN", Day) == 0)  return 7;
}

/*
 * Function to get integer value for corresponding Month
 */
byte GetMonthValue(char *Month)
{  
  if(strcmp("Jan", Month) == 0) return 1;
  else if(strcmp("Feb", Month) == 0)  return 2;
  else if(strcmp("Mar", Month) == 0)  return 3;
  else if(strcmp("Apr", Month) == 0)  return 4;
  else if(strcmp("May", Month) == 0)  return 5;
  else if(strcmp("Jun", Month) == 0)  return 6;
  else if(strcmp("Jul", Month) == 0)  return 7;
  else if(strcmp("Aug", Month) == 0)  return 8;
  else if(strcmp("Sep", Month) == 0)  return 9;
  else if(strcmp("Oct", Month) == 0)  return 10;
  else if(strcmp("Nov", Month) == 0)  return 11;
  else if(strcmp("Dec", Month) == 0)  return 12;
}

/*
 * Function to print RTC parameters (day, date, Time)
 */
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
