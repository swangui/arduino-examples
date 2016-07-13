#include <SoftwareSerial.h>
#include <EEPROM.h>

#define DEBUG true
#define BUFFER_SIZE 128

const int ledPin =  13;

int eepromBase = 2;
int eepromBlock = 64;
int paramsBase = 1; //n=2; n=2+(n-1)*eepromBlock
int ledState = HIGH;
char buffer[BUFFER_SIZE];

// use mega Serial 2 for serial monitor;
// Serial 1 on pins 19 (RX) and 18 (TX);
// Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX).
// Digital Pin number lower than 10 on mega will not receive signal

SoftwareSerial esp8266(10, 11);

void setup()
{
  pinMode(ledPin, OUTPUT);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  esp8266.begin(9600);

  Serial.println("Saved ssid and pwd:");
  String setting = read(paramsBase);
  Serial.println(setting);
  
  int s_begin = setting.indexOf("ssid=");
  int s_end = setting.indexOf("&pwd=");
  int s_len = setting.length();
  String ssid = setting.substring(s_begin+5,s_end);
  String pwd = setting.substring(s_end+5,s_len);
  Serial.println(ssid);
  Serial.println(pwd);
  
  sendData("AT+RST\r\n", 2000, DEBUG); // reset module
  sendData("AT+CWMODE=3\r\n", 1000, DEBUG); // configure as access point
  
  if( s_begin > -1 && s_end > -1 && s_len > 10 ){
    Serial.println("try to connect wifi");
    sendData("AT+CWJAP=\""+ssid+"\",\""+pwd+"\"\r\n", 1000, DEBUG);
    delay(8000);
    sendData("AT+CIFSR\r\n", 5000, DEBUG);
    sendData("AT+CIPMUX=1\r\n",5000,DEBUG);
    //sendData("AT+CIPMODE=1\r\n",5000,DEBUG);
    sendData("AT+CIPSTART=4,\"TCP\",\"192.168.3.3\",3000\r\n",5000,DEBUG);
    sendData("AT+CIPSEND=4,135\r\n",5000,DEBUG);
    //sendData("GET http://192.168.3.3:3000/api\r\n\r\n",5000,DEBUG);
    sendData("POST http://192.168.3.3:3000/api HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 17\r\n\r\ntemp=444&humi=555\r\n\r\n",5000,DEBUG);
  }
    
  clearSerialBuffer();

  sendData("AT+CWSAP=\"arhome02\",\"password\",3,0\r\n", 2000, DEBUG);
  //  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  //sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // turn on server on port 80
  Serial.println("I've done all the shit");

  //digitalWrite(ledPin, ledState);
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
    Serial.println(command);
    esp8266.print(command);
  }
  //*/
  int ch_id, packet_len;
  char *pb;
  //esp8266.readBytesUntil('\n', buffer, BUFFER_SIZE);
  esp8266.readBytesUntil('\n', buffer, BUFFER_SIZE);
  //Serial.print("Full of shit : ");
  //Serial.print("0"); Serial.println(buffer);

  if (strncmp(buffer, "+IPD,", 5) == 0) {
    // request: +IPD,ch,len:data
    sscanf(buffer + 5, "%d,%d", &ch_id, &packet_len);
    if (packet_len > 0) {
      // read serial until packet_len character received
      // start from :
      pb = buffer + 5;
      while (*pb != ':') pb++;
      pb++;
      if (strncmp(pb, "GET / ", 6) == 0) {
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
        //*
        char b[BUFFER_SIZE];
        while (strncmp(b, "ssid", 4) != 0) {
          // Clear the ssid and pwd buffer posted from client
          // I am no expert. Don't know why I am doing it twice
          for (int i = 0; i < BUFFER_SIZE; i++ ) {
            b[i] = 0;
          }
          esp8266.readBytesUntil('\r\n', b, BUFFER_SIZE);
        }
        Serial.print("Saving => ");
        Serial.println((String)b);
        save((String)b,paramsBase);
        // Clear the ssid and pwd buffer posted from client
        for (int i = 0; i < BUFFER_SIZE; i++ ) {
          b[i] = 0;
        }

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
  esp8266.println(Header.length() + Content.length());
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
  esp8266.println(Header.length() + Content.length());
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
  for (int i = 0; i < BUFFER_SIZE; i++ ) {
    buffer[i] = 0;
  }
}

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command); // send the read character to the esp8266
  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }

  if (debug)
  {
    Serial.print(response);
  }
  return response;
}


void save(String input, int base) {
  char tmpb[eepromBlock];
  char buf[eepromBlock];
  int len = input.length();
  input.toCharArray(tmpb, len);
  int i;
  buf[0] = len;
  EEPROM.write(eepromBase + base, buf[0]);
  Serial.print((int)buf[0]);
  Serial.print(',');
  for (i = 1; i <= (int)buf[0]; i++) {
    buf[i] = tmpb[i - 1];
    EEPROM.write(eepromBase + base + i, buf[i]);
    Serial.print((byte)buf[i]);
    Serial.print(',');
  }
}

String read(int base) {
  int i;
  String buf = "";
  int len = EEPROM.read(eepromBase + base);
  for (i = 1; i <= len; i++) {
    buf += (char)EEPROM.read(eepromBase + base + i);
  }
  return buf;
}
