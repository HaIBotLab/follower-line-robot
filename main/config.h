#ifndef CONFIG_H
#define CONFIG_H
#define PWM_FREQ 1000
#define PWM_RES 8

// ===== Cảm biến dò line 3 mắt (analog, ADC1) =====
#define OUT_1 33
#define OUT_2 32
#define OUT_3 35
//#define OUT_4 34
//#define OUT_5 36
int count =0;
float lastError = 0;
uint8_t lineState[3] = {0, 0, 0};
const uint8_t LINE_PINS[3] = {
    OUT_1, OUT_2, OUT_3
};
const int8_t LINE_WEIGHTS[] = {
    -1, 0, 1
};
// ===== Driver L298N =====
#define IN1  25
#define IN2  26
#define ENA  27
#define IN3  18
#define IN4  19
#define ENB  23
// PID
#define Kp 30.0f
#define Ki 0.0f
#define Kd 0.1f

#define base_speed 100

// ===== Cảm biến siêu âm HC-SR04 =====
#define TRIG 16
#define ECHO 17

// ===== Servo =====
#define SIGNAL 21

inline void initGPIO(){
  Serial.begin(115200);
  ledcAttachChannel(ENA, PWM_FREQ, PWM_RES, 0);
  ledcAttachChannel(ENB, PWM_FREQ, PWM_RES, 1);
  //Line
for (int i = 0; i < 3; i++) {
        pinMode(LINE_PINS[i], INPUT);
    }
  //DC left
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  ledcAttach(ENA, PWM_FREQ, PWM_RES);
  //DC right
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  ledcAttach(ENB, PWM_FREQ, PWM_RES);
  //HC-SR04
  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);
  //servo
  pinMode(SIGNAL,OUTPUT);
}
#endif
