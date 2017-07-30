#include <SoftwareSerial.h>
#define DEBUG true

SoftwareSerial esp8266(2,3);
void setup(){
  Serial.begin(9600);
  esp8266.begin(9600);

  pinMode(12, OUTPUT);
  digitalWrite(12,LOW);

  sendData("AT+RST\r\n",2000,DEBUG);
  sendData("AT+CIOBAUD?\r\n",2000,DEBUG);
  sendData("AT+CWMODE\r\n",1000,DEBUG);
  sendData("AT+CWLAP\r\n",3000,DEBUG);
  sendData("AT+CWJAP=\"iptime\",\"admin\"\r\n",5000,DEBUG);
  sendData("AT+CIFSR\r\n",1000,DEBUG);
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG);
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG);
}

void loop(){
  if(esp8266.available()){
    if(esp8266.find("+IPD,")){
      delay(1000);
      int connectionld = esp8266.read()-48;

      esp8266.find("pin=");
      int pinNumber = (esp8266.read()-48)*10;
      pinNumber += (esp8266.read()-48);
      digitalWrite(pinNumber, !digitalRead(pinNumber));

      String closeCommand="AT+CIPCLOSE=";
      closeCommand+=connectionld;
      closeCommand+="\r\n";
      sendData(closeCommand,1000,DEBUG);
    }
  }
}

String sendData(String command, const int timeout, boolean debug){
  String response = "";
  esp8266.print(command);
  long int time = millis();

  while((time+timeout)>millis()){
    while(esp8266.available()){
      char c = esp8266.read();
      response+=c;
    }
  }

  if(debug){
    Serial.print(response);
  }
  return response;
}

