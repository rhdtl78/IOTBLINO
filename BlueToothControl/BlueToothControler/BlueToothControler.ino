
#include <SoftwareSerial.h>
#include "Servo.h"

const int pin_blueTx = 2;
const int pin_blueRx = 3;
const int pin_servo = 5;
const int pin_icEnable = 11;
const int pin_dcMotor1 = 9;
const int pin_dcMotor2 = 10;
const int pin_cds = A0;
const int dcPower = 255;
bool isDown = true; //전부 내려와 있을 경우에만 true. 중간이나 위는 false입니다.
bool isClosed = true; 
bool isManualMode = false;
//stop 20171030_ksy
int stat_move = 2;
int stat_stop = 2;
int stat_time= 285; // 5~285
const int time_limit = 285;
String tmp;
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
  if(mySerial.available()){
    tmp = mySerial.readString();
    Serial.println(tmp);
    if(tmp.equals("Manual")){
      isManualMode = true;
    }else if(tmp.equals("Auto")){
      isManualMode = false;
    }
  }
  
  if(isManualMode){
    Bluetooth();
  }
  else{
    CDS();
  }
  CheckState();
	delay(1000);
}

/*
블루투스를 이용, 어플로 각도 조절, DC모터 조절
*/

void Bluetooth() {
  if (tmp.equals("Backward") || tmp.equals("Forward")) {
    if(tmp.equals("Backward") && !isClosed){
      ServoWrite(0);
      isClosed = true;
    }else if(tmp.equals("Forward") && isClosed){
      ServoWrite(3600);
      isClosed = false;
    }
  }
  else if (tmp.equals("Open") || tmp.equals("Close") || tmp.equals("Stop")) {
    if (tmp.equals("Open")) {
      stat_move = -1;
    }
    else if (tmp.equals("Close")) {
      stat_move = 1;      
    }else{
      stat_stop = 2;
      stat_move = 2;
    }
  }
}

/*
	DC모터만 따로 움직이게끔 설정.
*/
void changeDC(){
  if(stat_stop != 0){
    digitalWrite(pin_dcMotor1, false);
    digitalWrite(pin_dcMotor2, false);
  }
  else{
    analogWrite(pin_icEnable, (-1)*stat_move*dcPower);
    if(stat_move == 1){
      digitalWrite(pin_dcMotor1, true);
      digitalWrite(pin_dcMotor2, false);
    }
    else if(stat_move == -1){
      digitalWrite(pin_dcMotor1, false);
      digitalWrite(pin_dcMotor2, true);
    }
  } 
}

void CDS(){
  int cds_val = analogRead(pin_cds);
  if(cds_val <500){
    //내리고 닫아야함.(TT)
    Serial.println("Down,Close");
    if(isDown == false){
      stat_move = 1;
    }
    else if(isClosed == false){
      ServoWrite(0);
      isClosed = true;
    }
  }else if(cds_val < 700){
    //열기만 함(TF)
    Serial.println("Down,Open");
    if(isDown == false){
      stat_move = 1;
    }
    else if(isClosed == true){
      ServoWrite(3600);
      isClosed = false;
    }
    
  }else{
    //열고 올려야함(FF)
    Serial.println("Up,Open");
    if(isClosed == true){
      ServoWrite(3600);
      isClosed = false;
    }
    else{
      stat_move = -1;
      isDown = false; //제일 Down인 상태가 아니므로
    }
  }
}

void ServoWrite(int angle){
  m_servo.attach(pin_servo, 1000, 3000);
  m_servo.write(angle);
  delay(2500);
  m_servo.detach();
  Serial.print("Angle : ");
  Serial.println(angle);
}

void CheckState(){
  Serial.println(stat_move);
  Serial.println(stat_time);
  if(stat_stop==0){
    //안 멈춰있는 상태.
    stat_time += stat_move;
    if(stat_time < 5){
      stat_stop = -1;
    }
    else if(stat_time >= time_limit){
      //다 내려왔으므로 isDown을 true로 변경
      stat_stop = 1;
      isDown = true;
    }
  }
  else if(stat_stop != stat_move){
    //멈춤 상태에서 방향이 바뀔 경우
      stat_stop = 0;
      stat_time += stat_move;
  }
  changeDC();
}

