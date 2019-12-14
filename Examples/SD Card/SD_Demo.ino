/*
 * Header Files
 */
#include <SPI.h>                 //Library for SPI interface
#include "SdFat.h"              //Library for SD card


/*
 * Defining objects of diffrent class
 */
File myFile;                    //Defining object of myFile class
SdFat SD;                       //Defining object of sdFat class


//Variables for SD card
char File_Name[20] = "SDTest.txt";
char SD_DataBuffer[40] = "Writing data to SD card 1234......";

 
void setup() 
{
  //Initializing UART0
  Serial.begin(115200);

  Serial.println("System Reset");
  Serial.println();


  //Initializing SD card
  Serial.print("Initializing SD card...");


  if(!SD.begin(10))
  {
    Serial.println("Initialization failed");
    while(1);
  }
  Serial.println("Initialization done.");
  Serial.println();

}

void loop() 
{
  // Open the file. note that only one file can be open at a time.
  //so you have to close this one before opening another
  myFile = SD.open(File_Name, FILE_WRITE);

  //If the file opened okay, write to it:
  if(myFile)
  {
    Serial.print("Writing to ");
    Serial.println(File_Name);

    myFile.println(SD_DataBuffer);

    //close the file
    myFile.close();
    Serial.println("done");
    Serial.println();
  }
  else
  {
    //If the file did't open, print an error
    Serial.println("error opening ");
    Serial.println(File_Name);
  }

  delay(10000);
}
