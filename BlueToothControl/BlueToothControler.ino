/*
Name:    BlueToothControler.ino
Created: 2017-07-28 오후 3:22:41
Author:  rhdrl
*/


#include <SoftwareSerial.h>
#include "Servo.h"

int pin_blueTx = 2;
int pin_blueRx = 3;
int pin_servo = 11;
int pin_icEnable = 10;
int pin_dcMotor1 = 9;
int pin_dcMotor2 = 8;
int angle = 0;
int dcPower = 0;
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