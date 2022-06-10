#include <Arduino.h>

#define PIN_SENSOR A0
#define PIN_LED_R 5
#define PIN_LED_Y 6
#define PIN_LED_G 7

#define HAPTIC_WEAK 254/1.5
#define HAPTIC_STRONG 254/1.2

int step = 0; // 이전 압력 강도를 저장하는 변수

void doHaptic(int until) { // 햅틱을 생성하는 함수
  for(int i=0; i<until; i++) {
    analogWrite(3, i);
    delayMicroseconds(500);
  }
  analogWrite(3, 0);
}

void setup() {
  // GPIO 설정
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_Y, OUTPUT);
}

void loop() {
  int sensorData = analogRead(PIN_SENSOR); // 압력 강도 측정

  if(sensorData < 200) { // 0단계(압력이 감지되지 않은 경우)
    // 초록색 LED ON
    digitalWrite(PIN_LED_G, HIGH);
    digitalWrite(PIN_LED_Y, LOW);
    digitalWrite(PIN_LED_R, LOW);

    if(step == 1) doHaptic(HAPTIC_WEAK); // 1단계 -> 0단계 이동 시 약한 햅틱

    step = 0; // 이전 압력 강도 저장
  } else if(sensorData >= 200 && sensorData < 400) { // 1단계 압력이 감지될 경우
    // 노란색 LED ON
    digitalWrite(PIN_LED_G, LOW);
    digitalWrite(PIN_LED_Y, HIGH);
    digitalWrite(PIN_LED_R, LOW);

    if(step == 0) doHaptic(HAPTIC_WEAK); // 0단계 -> 1단계 이동 시 약한 햅틱
    else if(step == 2) doHaptic(HAPTIC_STRONG); // 2단계 -> 1단계 이동 시 강한 햅틱

    step = 1;
  } else if(sensorData >= 400) { // 2단계 압력이 감지될 경우
    // 빨간색 LED ON
    digitalWrite(PIN_LED_G, LOW);
    digitalWrite(PIN_LED_Y, LOW);
    digitalWrite(PIN_LED_R, HIGH);

    if(step == 1) doHaptic(HAPTIC_STRONG); // 1단계 -> 2단계 이동 시 강한 햅틱

    step = 2;
  }
}