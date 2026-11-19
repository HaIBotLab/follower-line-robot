#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <Arduino.h>
#include "config.h"
#include <ESP32Servo.h>
Servo Servo;
const uint8_t head_default = 90;
const uint8_t look_left = 135;
const uint8_t look_right = 0;
const float distance_danger = 17;
unsigned long t = 0;
//const unsigned uint8_t t_forward = 2000;
//const unsigned uint8_t t_bonus = 1000;
enum State{follow_line, turn_right, Next, turn_left};
State state = follow_line;
//ham phu
inline void readLineSensors() {
    for (int i = 0; i < 5; i++) {
        lineState[i] = digitalRead(LINE_PINS[i]);
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
inline void Go_straight(unsigned long ms){
  unsigned long t0 = millis();
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  ledcWrite(ENA, base_speed);
  ledcWrite(ENB, base_speed);
  while (millis() - t0 < ms) { }
  stopMotors();
}

void obstacle() {
  switch (state) {
    case follow_line: {
      static unsigned long lastPing = 0;
      if (millis() - lastPing >= 50) {
        lastPing = millis();
        float d = readDistance();
        if (d > 0 && d <= distance_danger) state = turn_right;
      }
      break;
    }

    case turn_right: {
      Serial.println("turn_right");
      unsigned long t0 = millis();

      // 1. Quay phải đến khi hết thấy vật (tối đa 2 s)
      while (readDistance() > 0 && millis() - t0 < 2000) {
        Turn_right();
      }

      // 2. Quay thêm 500 ms
      t0 = millis();
      while (millis() - t0 < 500) {
        Turn_right();
      }
      stopMotors();

      // 3. Servo nhìn trái
      //Servo.write(look_left);
      //delay(500);

      // 4. Đi thẳng 1.5 s
      Serial.println("di thang 1");
      Go_straight(1500);

      state = turn_left;
      break;
    }

    case turn_left: {
      Serial.println("turn_left");
      unsigned long t0 = millis();

      // 1. Quay trái 500 ms
      while (millis() - t0 < 500) {
        Turn_left();
      }
      stopMotors();

      // 2. Đi thẳng 1.5 s
      Serial.println("di thang 2");
      Go_straight(1500);

      // 3. Tìm lại line (tối đa 3 s)
      t0 = millis();
      readLineSensors();
      while (lineState[0] == 1 && lineState[1] == 1 &&
             lineState[2] == 1 && lineState[3] == 1 &&
             lineState[4] == 1 && millis() - t0 < 3000) {
        readLineSensors();
        digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
        ledcWrite(ENA, 110);
        ledcWrite(ENB, 180);
      }
      stopMotors();

      Servo.write(head_default);
      state = follow_line;
      Serial.println("xong, ve follow_line");
      break;
    }
  }
}
#endif 
