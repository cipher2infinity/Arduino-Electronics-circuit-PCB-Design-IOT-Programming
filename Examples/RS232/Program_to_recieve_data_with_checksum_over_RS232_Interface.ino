char Rcvdata = 0;
char RecvBuffer[50] = "";
byte RecvBufferCount = 0;
byte CheckSumByte = 0;


/*
 * Setup Code
 */
void setup() 
{
  //Initializing UART0 at 9600 baud rate
  Serial.begin(9600);

  //Initializing UART1 at 9600 baud rate
  Serial.begin(9600);

  Serial.println("Sytem Reset");
  Serial.println();
}



/*
 * Main Loop
 * HELL%#
 */
void loop() 
{
  //Checking if data is available on UART1 or not
  if(Serial.available()>0)
  {
    Rcvdata = Serial1.read();

    Serial.write(Rcvdata);

    if(Rcvdata != '#')
    {
      RecvBuffer[RecvBufferCount] = Rcvdata;
      RecvBufferCount++;
    }
    else
    {
      Serial.print("Recieved Data : ");
      Serial.println(RecvBuffer);

      //Initializing checksum byte to 0
      CheckSumByte = 0;

      //Calculating checksum byte for the string
      for(uint8_t i = 0; i < ( RecvBufferCount - 1 ); i++)
      {
        CheckSumByte = CheckSumByte + RecvBuffer[i];
      }

      //Printing Recieved checksum byte
      Serial.print("Recieved CheckSum Byte : ");
      Serial.write(RecvBuffer[RecvBufferCount - 1]);
      Serial.println();

      //Printing Calculated checksum byte
      Serial.print("Calculated CheckSum Byte : ");
      Serial.write(CheckSumByte);
      Serial.println();

      //Comparing calculated checksum byte with recieved checksum byte
      if(CheckSumByte == RecvBuffer[RecvBufferCount - 1])
      {
        Serial.println("CheckSum Byte Match");
      }
      else
      {
        Serial.println("Data Corrupted");
      }

      Serial.println();

      //clearing Buffer
      for(uint8_t i = 0; i < 50; i++)
      {
        RecvBuffer[i] = 0;
      }

      RecvBufferCount = 0;
    }
  }

}
