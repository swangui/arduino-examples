#include <SoftwareSerial.h>
#define DEBUG true


const int ledPin =  13;
int ledState = HIGH;
#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE];
char buffer1[BUFFER_SIZE];
char buffer2[BUFFER_SIZE];
char buffer3[BUFFER_SIZE];
char buffer4[BUFFER_SIZE];
char buffer5[BUFFER_SIZE];
char buffer6[BUFFER_SIZE];
char buffer7[BUFFER_SIZE];
char buffer8[BUFFER_SIZE];

//use mega Serial 2 for serial monitor; Serial 1 on pins 19 (RX) and 18 (TX);// Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX).

SoftwareSerial esp8266(10, 11);

void setup()
{
  pinMode(ledPin, OUTPUT);
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
  clearSerialBuffer();
  
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=3\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CWSAP=\"ddAP\",\"password\",3,0\r\n",2000,DEBUG);
//  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
  Serial.println("I've done all the shit");
  
  digitalWrite(ledPin,ledState);
}

void loop() // run over and over
{
  /*
  if (esp8266.available()) {
    while(esp8266.available()){
      char c = esp8266.read(); // read the next character.
      Serial.write(c);
    }
  }
  //*/
  /*
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
  int ch_id, packet_len;
  char *pb;  
  //esp8266.readBytesUntil('\n', buffer, BUFFER_SIZE);
  esp8266.readBytesUntil('\r\n', buffer, BUFFER_SIZE);
  Serial.print("Full of shit : ");
  Serial.print("0");Serial.println(buffer);
  
  if(strncmp(buffer, "+IPD,", 5)==0) {
    // request: +IPD,ch,len:data
    sscanf(buffer+5, "%d,%d", &ch_id, &packet_len);
    if (packet_len > 0) {
      // read serial until packet_len character received
      // start from :
      pb = buffer+5;
      while(*pb!=':') pb++;
      pb++;
      if (strncmp(pb, "GET /led", 8) == 0) {
        Serial.print(millis());
        Serial.print(" : ");
        Serial.println(buffer);
        Serial.print( "get led from ch :" );
        Serial.println(ch_id);
 
        delay(500);
        clearSerialBuffer();
        
       if (ledState == LOW)
          ledState = HIGH;
       else
          ledState = LOW;
        digitalWrite(ledPin, ledState);
        
        homepage(ch_id);
 
      } else if (strncmp(pb, "GET / ", 6) == 0) {
        Serial.print(millis());
        Serial.print(" : ");
        Serial.println(buffer);
        Serial.print( "get Status from ch:" );
        Serial.println(ch_id);
        
        delay(100);
        clearSerialBuffer();
 
        confpage(ch_id);
      } else if (strncmp(pb, "POST / ", 7) == 0) {
        Serial.print(millis());
        Serial.print(" : ");
        Serial.println(buffer);
        Serial.print( "get Status from ch:" );
        Serial.println(ch_id);
        
        char b[BUFFER_SIZE];
        while(strncmp(b, "ssid", 4)!=0){
           for (int i =0;i<BUFFER_SIZE;i++ ) {
             b[i]=0;
           }
          esp8266.readBytesUntil('\n', b, BUFFER_SIZE);
        }
        Serial.print("FUCK FUCK!!!");
        Serial.println(b);
  /*
  esp8266.readBytesUntil('\r\n', buffer1, BUFFER_SIZE);
  Serial.print("1");Serial.println(buffer1);
  esp8266.readBytesUntil('\r\n', buffer2, BUFFER_SIZE);
  Serial.print("2");Serial.println(buffer2);
  esp8266.readBytesUntil('\r\n', buffer3, BUFFER_SIZE);
  Serial.print("3");Serial.println(buffer3);
  esp8266.readBytesUntil('\r\n', buffer4, BUFFER_SIZE);
  Serial.print("4");Serial.println(buffer4);
  esp8266.readBytesUntil('\r\n', buffer5, BUFFER_SIZE);
  Serial.print("5");Serial.println(buffer5);
  esp8266.readBytesUntil('\r\n', buffer6, BUFFER_SIZE);
  Serial.print("6");Serial.println(buffer6);
  esp8266.readBytesUntil('\r\n', buffer6, BUFFER_SIZE);
  Serial.print("7");Serial.println(buffer7);
  esp8266.readBytesUntil('\r\n', buffer7, BUFFER_SIZE);
  Serial.print("8");Serial.println(buffer8);
  //*/
        delay(500);
        clearSerialBuffer();
 
        saveconf(ch_id);
      }
    }
  }
  clearBuffer();
}

void confpage(int ch_id) {
  String Header;
 
  Header =  "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
  //Header += "Refresh: 5\r\n";
  
  String Content;
  Content = "<h1>Welcome to use FireAlarm</h1>";
  Content += "<p>Please set your home WiFi</p>";
  Content += "<form method=\"POST\" action=\"/\">";
  Content += "WiFi Name: <input type=\"text\" name=\"ssid\" /><br />";
  Content += "Password: <input type=\"password\" name=\"pwd\" /><br />";
  Content += "<input type=\"submit\" value=\"Save\"><br />";
  Content += "</form>";
  
  Header += "Content-Length: ";
  Header += (int)(Content.length());
  Header += "\r\n\r\n";
  
  
  esp8266.print("AT+CIPSEND=");
  esp8266.print(ch_id);
  esp8266.print(",");
  esp8266.println(Header.length()+Content.length());
  delay(10);
  
  // for debug buffer serial error
  //while (esp8266.available() >0 )  {
  //  char c = esp8266.read();
  //  Serial.write(c);
  //  if (c == '>') {
  //      esp8266.print(Header);
  //      esp8266.print(Content);
  //  }
  //}
  
  if (esp8266.find(">")) {
      esp8266.print(Header);
      esp8266.print(Content);
      delay(10);
   }
 
//  Serial1.print("AT+CIPCLOSE=");
//  Serial1.println(ch_id);
}

void saveconf(int ch_id) {
  String Header;
 
  Header =  "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
  //Header += "Refresh: 5\r\n";
  
  String Content;
  Content = "<h1>WiFi credential has been saved</h1>";
  
  Header += "Content-Length: ";
  Header += (int)(Content.length());
  Header += "\r\n\r\n";
  
  
  esp8266.print("AT+CIPSEND=");
  esp8266.print(ch_id);
  esp8266.print(",");
  esp8266.println(Header.length()+Content.length());
  delay(10);
  
  // for debug buffer serial error
  //while (esp8266.available() >0 )  {
  //  char c = esp8266.read();
  //  Serial.write(c);
  //  if (c == '>') {
  //      esp8266.print(Header);
  //      esp8266.print(Content);
  //  }
  //}
  
  if (esp8266.find(">")) {
      esp8266.print(Header);
      esp8266.print(Content);
      delay(10);
   }
 
//  Serial1.print("AT+CIPCLOSE=");
//  Serial1.println(ch_id);
}

void homepage(int ch_id) {
  String Header;
 
  Header =  "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
  //Header += "Refresh: 5\r\n";
  
  String Content;
  Content = "D";
  Content += String(ledState);
  
  Header += "Content-Length: ";
  Header += (int)(Content.length());
  Header += "\r\n\r\n";
  
  
  esp8266.print("AT+CIPSEND=");
  esp8266.print(ch_id);
  esp8266.print(",");
  esp8266.println(Header.length()+Content.length());
  delay(10);
  
  // for debug buffer serial error
  //while (esp8266.available() >0 )  {
  //  char c = esp8266.read();
  //  Serial.write(c);
  //  if (c == '>') {
  //      esp8266.print(Header);
  //      esp8266.print(Content);
  //  }
  //}
  
  if (esp8266.find(">")) {
      esp8266.print(Header);
      esp8266.print(Content);
      delay(10);
   }
 
//  Serial1.print("AT+CIPCLOSE=");
//  Serial1.println(ch_id);
 
 
}

void clearSerialBuffer(void) {
       while ( esp8266.available() > 0 ) {
         esp8266.read();
       }
}

void clearBuffer(void) {
       for (int i =0;i<BUFFER_SIZE;i++ ) {
         buffer[i]=0;
         buffer1[i]=0;
         buffer2[i]=0;
         buffer3[i]=0;
         buffer4[i]=0;
         buffer5[i]=0;
         buffer6[i]=0;
         buffer7[i]=0;
         buffer8[i]=0;
       }
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
