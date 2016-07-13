#include <SoftwareSerial.h>

SoftwareSerial GPRS(7, 8);
unsigned char buffer[64];
int count=0;

void setup()
{
  GPRS.begin(19200);               // the GPRS baud rate   
  Serial.begin(19200);             // the Serial port of Arduino baud rate.
   
   GPRS.print("AT+CMGF=1\r\n");
   delay(500);
   GPRS.print("AT+CMGS=\"+8618017723066\"\r\n");
   delay(500);
   GPRS.print("hello world\r\n");
   delay(500);
   GPRS.write(0x1A);
   delay(500);
}

void loop() // run over and over
{
  //*
  if (GPRS.available()) {
    while(GPRS.available()){
      buffer[count++] = GPRS.read();
      if(count == 64)break;
    }
    Serial.write(buffer,count);
    clearBufferArray();
    count = 0;
  }
  //*/
  //*
  if (Serial.available()) {
    delay(1000);
    String command = "";
    while(Serial.available()){
      // read the most recent byte
      command += (char)Serial.read();
      // ECHO the value that was read, back to the serial port.
    }
    Serial.println(command);
    GPRS.print(command);
  }
  //*/
}

void clearBufferArray()              // function to clear buffer array
{
  for (int i=0; i<count;i++)
    { buffer[i]=NULL;}                  // clear all index of array with command NULL
}
