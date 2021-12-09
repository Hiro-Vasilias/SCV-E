//this code is for actuator motor using the L298N Module.



#include <Arduino.h>

int Actuator1 = 4;
int Actuator2 = 5;

void setup() {
  // put your setup code here, to run once:

  pinMode(Actuator1, OUTPUT);
  pinMode(Actuator2, OUTPUT);
  
}



void loop() {
  // put your main code here, to run repeatedly:     digitalWrite(motor1pin1, HIGH);
  

  digitalWrite(Actuator1, HIGH);
  digitalWrite(Actuator2, LOW);
  delay(5000);

//  digitalWrite(Actuator1, LOW);
//  digitalWrite(Actuator2, HIGH);
//  delay(5000);
}