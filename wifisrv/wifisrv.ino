#include <SoftwareSerial.h>
#define DEBUG true
//use mega Serial 2 for serial monitor; Serial 1 on pins 19 (RX) and 18 (TX);// Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX).

SoftwareSerial esp8266(10, 11);

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  esp8266.begin(9600);
  /*
  esp8266.println("AT+RST");
  delay(5000);
  esp8266.println("AT+CWMODE=3");
  delay(2000);
  esp8266.println("AT+CWSAP=\"VVAP\",\"password\",3,0");
  delay(2000);
  */
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CWSAP=\"ssAP\",\"password\",3,0\r\n",2000,DEBUG);
//  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  Serial.println("I've done all the shit");
}

void loop() // run over and over
{
  //*
  if (esp8266.available()) {
    while(esp8266.available()){
      char c = esp8266.read(); // read the next character.
      Serial.write(c);
    }
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
    //Serial.println(command);
    esp8266.print(command);
  }
  //*/
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    esp8266.print(command); // send the read character to the esp8266
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    return response;
}
