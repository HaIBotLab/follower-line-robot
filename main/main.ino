#include "config.h"
#include "Obstacle.h" 
float error_header() {
    const int weight[3] = {-1, 0, 1};
    int sum_weight = 0;
    float error;
    count = 0;
    for (int i = 0; i < 3; i++) {
        if (lineState[i] == 1) {
            sum_weight += weight[i];
            count++;
        }
    }

    if (count == 0) {
        return 0;
    }

    error = (float)sum_weight / count;
    return error;
}
int PID() {
  static float error_prev = 0;
  static float integral = 0;
  static unsigned long t_prev = micros();

  unsigned long now = micros();
  float dt = (now - t_prev) / 1e6f;
  t_prev = now;
  if (dt < 0.001f) dt = 0.001f;   // tránh chia cho 0
  if (dt > 0.05f)  dt = 0.05f;    // sau khi mất line/né vật, tránh dt quá lớn

  float error = error_header();
  integral += error * dt;
  float derivative = (error - error_prev) / dt;

  int output = Kp * error + Ki * integral + Kd * derivative;
  error_prev = error;
  return output;
}
void Control_wheels(void) {
    float error = error_header();  
    if (count==3) {
        ledcWrite(ENA, 0);
        ledcWrite(ENB, 0);
        return;
    }
    if (count == 0) {            // mất line: quay về phía lệch cuối
    if (lastError > 0) { Turn_right(); }
    else               { Turn_left(); }
    return;
    }
    lastError = error;

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    int output = PID();
    int left_PWM  = constrain(base_speed + output, 0, 255);
    int right_PWM = constrain(base_speed - output, 0, 255);
    ledcWrite(ENA, left_PWM);
    ledcWrite(ENB, right_PWM);
    }
void setup() {
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH); delay(300); digitalWrite(2, LOW);
    initGPIO();
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    initServo(); 
    initUltrasonic();
}

void loop() {
  readLineSensors();
  obstacle();                          
  if (state == follow_line) Control_wheels();
  delay(5);
}
