#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <Arduino.h>
#include "config.h"
#include <ESP32Servo.h>
Servo Servo;
const uint8_t head_default = 90;
const uint8_t look_left = 180;
const uint8_t look_right = 0;
const float distance_danger = 20;
unsigned long t = 0;
//const unsigned uint8_t t_forward = 2000;
//const unsigned uint8_t t_bonus = 1000;
enum State{follow_line, turn_right, Next, turn_left};
State state = follow_line;
//ham phu
inline void readLineSensors() {
  for (int i = 0; i < 3; i++) {
    int sum = 0;
    for (int k = 0; k < 7; k++) sum += digitalRead(LINE_PINS[i]);
    if (sum >= 5) lineState[i] = 1;       // 5 lần đều thấy line
    else if (sum == 0) lineState[i] = 0;  // 5 lần đều không thấy
  }
}
inline void initServo(){
  Servo.setPeriodHertz(50);
  Servo.attach(SIGNAL, 500, 2400);
  Servo.write(head_default);
  delay(10);
}
inline void initUltrasonic() {
  pinMode(TRIG , OUTPUT);
  pinMode(ECHO , INPUT);
  digitalWrite(TRIG , LOW);
}
inline float readDistance() {
    digitalWrite(TRIG , LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG , HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG , LOW);
    unsigned long duration = pulseIn(ECHO , HIGH, 15000);
    if (duration == 0) {
        return -1.0f;
    }
    return duration * 0.0343f / 2.0f;
}
inline void Turn_left(){
    ledcWrite(ENA, 200);   ledcWrite(ENB, 200);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH); 
}
inline void Turn_right(){
    ledcWrite(ENA, 150); ledcWrite(ENB, 150);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

inline void stopMotors() {
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
}
inline void Go_straight(){
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  ledcWrite(ENA, base_speed);
  ledcWrite(ENB, base_speed);
}

void obstacle(){
  switch (state){
  case follow_line:
  static unsigned long lastPing = 0;
  if (millis() - lastPing >= 50) {
    lastPing = millis();
    bool danger = true;
    for (int k = 0; k < 5; k++) {
      float d = readDistance();
      if (!(d > 0 && d <= distance_danger)) { danger = false; break; }  // 1 lần không thỏa là bỏ
      delay(30);                                                        // nghỉ giữa 2 lần đo
    }
    if (danger) state = turn_right;
  }
  break;
  case turn_right:
    //while(readDistance()>0){
    //Turn_right();
    //}
    t = millis();
    while (millis()-t <550){
    Turn_right();
    }
    stopMotors();
    Servo.write(look_left);
    delay(500);
    while(readDistance()>=distance_danger){
    Go_straight();
    }
    //while(readDistance()>0 && readDistance()<distance_danger){
    //Turn_right();
    //}
    if(readDistance()<0){
    Go_straight();
    delay(500);
    }
    state = turn_left;
    break;
  case turn_left:
    if(readDistance() < 0 || readDistance()>distance_danger){
    Turn_left();
    delay(360);
    }
    while(readDistance()>13){
    Go_straight();
    }
    t = millis();
    while(millis() - t <=700){
      Go_straight();
    }
    t = millis();
    while(millis() - t <=350){
      Turn_left();
    }
    readLineSensors();
    t = millis();
    uint8_t hits = 0;
    while (millis() - t < 4000) {     // timeout 4 s
      readLineSensors();
      if (lineState[0] || lineState[1] || lineState[2]) hits++;
      else hits = 0;
      if (hits >= 3) break;           // phải thấy line 10 lần liên tiếp
      Go_straight();
      }
    Servo.write(head_default); 
      state = follow_line;
    break;
    }
  }
#endif 
