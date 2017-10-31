
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
bool isDown = true; //전부 내려와 있을 경우에만 true. 중간이나 위는 false입니다.
bool isClosed = true; 
bool isModifyMode = false;
int second = 1000;
//stop 20171030_ksy
int stat_move = 0;
int stat_stop = 1;
int stat_time= 295; // 5~295

SoftwareSerial mySerial(pin_blueTx,pin_blueRx);
Servo m_servo;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(pin_icEnable, OUTPUT);
  pinMode(pin_dcMotor1, OUTPUT);
  pinMode(pin_dcMotor2, OUTPUT);
  m_servo.attach(pin_servo);
  m_servo.write(90);
  delay(1000);
  m_servo.detach();
  delay(500);
}

// the loop function runs over and over again until power down or reset
void loop() {
	CDS();
	MotorWrite();
	delay(1000);
	Serial.println(stat_move);
	Serial.println(stat_time);
	if(stat_stop==0){
		//안 멈춰있는 상태.
		stat_time += stat_move;
		if(stat_time < 5){
			stat_stop = -1;
		}
		else if(stat_time > 295){
			//다 내려왔으므로 isDown을 true로 변경
			stat_stop = 1;
			isDown = true;
		}
		else{
			changeDC();
		}
	}
	else{
		//멈춤 상태.
		if(stat_stop != stat_move){ //방향이 바뀔 경우
			stat_stop = 0;
			stat_time += stat_move;
		}
	}
}

/*
블루투스를 이용, 어플로 각도 조절, DC모터 조절
DC : -255 ~ 255
angle : 0 ~ 90
*/

void MotorWrite() {

  if (mySerial.available()) {
    String tmp = mySerial.readString();
    Serial.println(tmp);
    if (tmp.equals("Backward") || tmp.equals("Forward")) {
      m_servo.attach(pin_servo, 1000, 5000);
      if(tmp.equals("Backward")){
        angle = 0;
      }else if(tmp.equals("Forward")){
        angle = 180;
      }
      m_servo.write(angle);
      delay(4500);
      m_servo.detach();
      Serial.print("Angle : ");
      Serial.println(angle);
    }
    else if (tmp.equals("Open") || tmp.equals("Close") || tmp.equals("Stop")) {
      dcPower = 255;
      if (tmp.equals("Open")) {
        int speed = (-1)*dcPower;
        analogWrite(pin_icEnable, speed);
        digitalWrite(pin_dcMotor1, true);
        digitalWrite(pin_dcMotor2, false);
        Serial.print("Speed : ");
        Serial.println(speed);
      }
      else if (tmp.equals("Close")) {
        int speed = dcPower;
        analogWrite(pin_icEnable, speed);
        digitalWrite(pin_dcMotor1, false);
        digitalWrite(pin_dcMotor2, true);
        Serial.print("Speed : ");
        Serial.println(speed);
      }else if(tmp.equals("Stop")){
        int speed = 0;
        analogWrite(pin_icEnable, speed);
        digitalWrite(pin_dcMotor1, false);
        digitalWrite(pin_dcMotor2, false);
        Serial.print("Speed : ");
        Serial.println(speed);
      }
    }
  }
}

/*
	DC모터만 따로 움직이게끔 설정.
*/
void changeDC(){
	if(stat_move == 1){
		int speed = 255;
		analogWrite(pin_icEnable, (-1)*speed);
		digitalWrite(pin_dcMotor1, true);
		digitalWrite(pin_dcMotor2, false);
	}
	else if(stat_move == -1){
		int speed = 255;
		analogWrite(pin_icEnable, speed);
		digitalWrite(pin_dcMotor1, false);
		digitalWrite(pin_dcMotor2, true);
	}
}

void CDS(){
	if(isModifyMode == false){
		int cds_val = analogRead(pin_cds);
		if(0 <= cds_val && cds_val <500){
			//내리고 닫아야함.(TT)
			Serial.println("Down,Close");
			if(isDown == false){
				stat_move = 1;
			}
			else if(isClosed == false){
				m_servo.attach(pin_servo);
				m_servo.write(3600);
				m_servo.detach();
				isClosed = true;
			}
		}else if(500 <= cds_val && cds_val < 700){
			//열기만 함(TF)
			Serial.println("Down,Open");
			if(isDown == false){
				stat_move = 1;
			}
			else if(isClosed == true){
				m_servo.attach(pin_servo);
				m_servo.write(-3600);
				m_servo.detach();
				isClosed = false;
			}
			
		}else if(700 <= cds_val){
			//열고 올려야함(FF)
			Serial.println("Up,Open");
			if(isClosed == true){
				m_servo.attach(pin_servo);
				m_servo.write(-3600);
				m_servo.detach();
				isClosed = false;
			}
			else{
				stat_move = -1;
				isDown = false; //제일 Down인 상태가 아니므로
			}
		}
	}
}


