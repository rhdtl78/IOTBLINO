/*
 Name:    TwoWayMotor.ino
 Created: 2017-07-20 오후 7:29:05
 Author:  rhdrl
*/

int enablePin = 11;
int in1Pin = 10;
int in2Pin = 9;
int switchPin = 7;
int potPin = 0;
boolean reverse;
boolean clicked;
int speed;

void setup() {
  // put your setup code here, to run once:
  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(switchPin, INPUT);
  pinMode(potPin, INPUT);
  reverse = false;
  speed = 255;
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  speed = analogRead(potPin) / 4;
  twoway();

}

void twoway() {

  clicked = digitalRead(switchPin);
  if (clicked) {
    if (reverse) {
      reverse = false;
    }
    else {
      reverse = true;
    }
  }

  if (reverse) {
    Serial.println("true");
  }
  else {
    Serial.println("false");
  }

  analogWrite(enablePin, speed);
  digitalWrite(in1Pin, !reverse);
  digitalWrite(in2Pin, reverse);
}
