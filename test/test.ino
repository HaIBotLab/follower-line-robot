#include "config.h"
#include "obstacle.h"
void setup() {
  initGPIO();
  delay(1000);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  ledcWrite(ENA, 200);
  ledcWrite(ENB, 200);
  delay(3000);
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
}
void loop() {}