#include <SoftwareSerial.h>
#include <EEPROM.h>

//use mega Serial 2 for serial monitor; Serial 1 on pins 19 (RX) and 18 (TX);// Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX).

SoftwareSerial esp8266(10, 11);
int eepromBase = 2;
int eepromBlock = 64;
int paramsBase = 1; //n=2; n=2+(n-1)*32

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  String saved = read(paramsBase);
  Serial.println("READ SAVED AFTER RESTART");
  Serial.println(saved);
  //Serial.setTimeout(5000);
  //esp8266.begin(9600);
  //esp8266.println("Init");
  //Serial.println("AT+CWJAP=\"Samuel Wang's Wi-Fi Network\",\"54dfbb24\"");
  //Serial.println("AT+CWJAP=\"WiFi 01\",\"8888888888\"");
  //delay(4000);
  //Serial.println("AT+CIPMUX=0");
  //delay(1000);
  //AT+CIPSTART="TCP","10.0.1.12",3000
  //AT+CIPSEND=44
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
    save(command, paramsBase);
    String saved = read(paramsBase);
    Serial.println("READ SAVED");
    Serial.println(saved);
    
    Serial.println("\nwait");
    //EEPROM.write(base+0, *command);
    //Serial.println((char)EEPROM.read(base+0));
    //esp8266.print(command);
  }
  //*/
}

void save(String input, int base) {
  char tmpb[eepromBlock];
  char buf[eepromBlock];
  int len = input.length();
  input.toCharArray(tmpb, len);
  int i;
  buf[0] = len - 2;
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
