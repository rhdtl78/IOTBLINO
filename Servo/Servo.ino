/*
 DHCP-based IP printer
This sketch uses the DHCP extensions to the Ethernet library
to get an IP address via DHCP and print the address obtained.
using an Arduino Wiznet Ethernet shield.
Circuit:
* Ethernet shield attached to pins 10, 11, 12, 13
created 12 April 2011
modified 9 Apr 2012
by Tom Igoe
*/
//ip = 192.168.0.105


#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>

Servo servo;
int angle = 0;
int cdspin = A0;
int servopin = 7;
int cdsval = 0;

int led[5];

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED } ; //mac address
EthernetClient client;
EthernetServer server(90);
IPAddress ip(Ethernet.localIP());

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // start the Ethernet connection:
  IsConnected();
  // print your local IP address:
  printstatus();
  setLed();
  servo.attach(servopin);
  server.begin();
}

void setLed(){
  int j = 0;
  int k = 2;
  while(j < 5){
    led[j] = k;
   j++;
   k++;
  }

  int i = 0;
  while(i < 5){
    pinMode(led[i],OUTPUT);
    i++;
  }
}

void webpage(){
  client = server.available(); // try to get client
  if (client) { // got client?
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) { // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n') {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          // send web page
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino Web Page</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>Hello Arduino! Jiwoo AngNyeong</h1>");
          client.println("This web page from the Arduino server");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        // every line of text received from the client ends with \r\n
      } // end if (client.available())
    } // end while (client.connected())
    delay(1); // give the web browser time to receive the data
    client.stop(); // close the connection
  } // end if (client)
}

void IsConnected(){
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Serial.println("OfflineMode");
   }
}

void printstatus(){
  Serial.print("My IP Address : ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println(' ');
  Serial.print("CDS : ");
  Serial.println(cdsval);
}

void servocontrol(int sensor_val){
  if(sensor_val > 400){
    servo.write(90);
  }else {
    servo.write(10);
  }
}

void ledmeter(int sensor_val){
  int i = 0;
  while(i < 5){
      digitalWrite(led[i],LOW);
      i++;
  }
    
  if(sensor_val >= 1000){

  }else if(800 <= sensor_val && sensor_val < 1000){
    digitalWrite(led[0],HIGH);
  }else if(600 <= sensor_val && sensor_val < 800){
    i = 0;
    while(i < 2){
      digitalWrite(led[i],HIGH);
      i++;  
    }
  }else if(400 <= sensor_val && sensor_val < 600){
    i = 0;
    while(i < 3){
      digitalWrite(led[i],HIGH);
      i++;  
    }
  }else if(200 <= sensor_val && sensor_val < 400){
   i = 0;
    while(i < 4){
      digitalWrite(led[i],HIGH);
      i++;  
    }
  }else {
    i = 0;
    while(i < 5){
      digitalWrite(led[i],HIGH);
      i++;  
    }
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  cdsval = analogRead(cdspin);
  printstatus();
  servocontrol(cdsval);
  ledmeter(cdsval);
  webpage();

}
