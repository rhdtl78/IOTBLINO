
#include <SoftwareSerial.h>
#include "Servo.h"

int pin_blueTx = 2;
int pin_blueRx = 3;
int pin_servo = 5;
int pin_icEnable = 11;
int pin_dcMotor1 = 9;
int pin_dcMotor2 = 10;
int pin_cds = A0;
int angle = 0;
int dcPower = 0;
bool isDown = true;
bool isClosed = true;
bool isModifyMode = false;
int second = 1000;
int currentTime = 0;
int prevTime = 0;
unsigned long fifmin = 300000;
SoftwareSerial mySerial(pin_blueTx,pin_blueRx);
Servo m_servo;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(pin_icEnable, OUTPUT);
  pinMode(pin_dcMotor1, OUTPUT);
  pinMode(pin_dcMotor2, OUTPUT);
  delay(500);
}

// the loop function runs over and over again until power down or reset
void loop() {
  MotorWrite();
  CDS();
  delay(1000);
}

/*
블루투스를 이용, 어플로 각도 조절, DC모터 조절
DC : -255 ~ 255
angle : 0 ~ 90
*/

void MotorWrite() {

  if (mySerial.available()) {
    int tmp = mySerial.parseInt();
    mySerial.println(tmp);
    if (0 <= tmp && tmp <= 90) {
      m_servo.attach(pin_servo);
      angle = tmp;
      m_servo.write(angle);
      delay(2000);
      m_servo.detach();
      mySerial.print("Angle : ");
      mySerial.println(angle);
    }
    else if (100 <= tmp && tmp <= 611) {
      dcPower = tmp - 355;
      if (-255 <= dcPower && dcPower <= 0) {
        int speed = (-1)*dcPower;
        analogWrite(pin_icEnable, speed);
        digitalWrite(pin_dcMotor1, true);
        digitalWrite(pin_dcMotor2, false);
        mySerial.print("Speed : ");
        mySerial.println(speed);
      }
      else if (0 < dcPower && dcPower <= 255) {
        int speed = dcPower;
        analogWrite(pin_icEnable, speed);
        digitalWrite(pin_dcMotor1, false);
        digitalWrite(pin_dcMotor2, true);
        mySerial.print("Speed : ");
        mySerial.println(speed);
      }
    }
  }
}

void CDS(){
  if(isModifyMode == false){
    int cds_val = analogRead(pin_cds);
    if(0 <= cds_val && cds_val <500){
      //내리고 닫아야함.
      Serial.println("Down,Close");
      if(isDown == false){
        int speed = 255;
        analogWrite(pin_icEnable, (-1)*speed);
        digitalWrite(pin_dcMotor1, true);
        digitalWrite(pin_dcMotor2, false);
        isDown = true;
      }
      if(isClosed == false){
        m_servo.attach(pin_servo);
        m_servo.write(3600);
        m_servo.detach();
        isClosed = true;
      }
    }else if(500 <= cds_val && cds_val < 700){
      //열기만 함
      Serial.println("Down,Open");
      if(isClosed == true){
        m_servo.attach(pin_servo);
        m_servo.write(-3600);
        m_servo.detach();
        isClosed = false;
      }
      if(isDown == false){
        int speed = 255;
        analogWrite(pin_icEnable, (-1)*speed);
        digitalWrite(pin_dcMotor1, true);
        digitalWrite(pin_dcMotor2, false);
        isDown = true;
      }
    }else if(700 <= cds_val){
      //열고 올려야함
      Serial.println("Up,Open");
      if(isClosed == false){
        m_servo.attach(pin_servo);
        m_servo.write(-3600);
        m_servo.detach();
        isClosed = false;
      }
      if(isDown == true){
        int speed = 255;
        analogWrite(pin_icEnable, speed);
        digitalWrite(pin_dcMotor1, false);
        digitalWrite(pin_dcMotor2, true);
        isDown = false;
      }
    }
  }
}


