#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <PMS.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define DHTPIN 5       // DHT 센서 핀 번호
#define DHTTYPE DHT22  // DHT 센서 종류

#define MODENUM 3  // 화면 출력 모드 개수
#define BTNPIN 7   // 푸시버튼 핀 번호

SoftwareSerial pmsSerial(
    2, 3);  // PMS 센서 통신에 사용할 소프트웨어 시리얼 설정 (RX, TX)

PMS pms(pmsSerial);  // PMS 설정 (시리얼 객체)
PMS::DATA pms_data;  // PMS 센서의 데이터를 저장하는 변수 선언

DHT dht(DHTPIN, DHTTYPE);  // DHT 설정 (핀 번호, 종류)

LiquidCrystal_I2C lcd(0x27, 20, 4);  // LCD 설정 (주소값, 가로, 세로)

float hmd, tmp, hix;        // DHT22 센서 데이터를 저장하는 변수
unsigned long nxtTime = 0;  // 다음 작동 시간을 저장하는 변수

int mode = 0;  // 모드 변경을 위한 변수 선언

// 센서 데이터를 읽는 함수
void readData() {
  // DHT22 데이터 읽기
  hmd = dht.readHumidity();                     // 습도 측정
  tmp = dht.readTemperature();                  // 온도 측정
  hix = dht.computeHeatIndex(tmp, hmd, false);  // 체감온도 계산

  // PMS 센서 데이터 읽기
  pms.read(pms_data);
}

// 시리얼 모니터에 데이터 출력하는 함수
void debugData() {
  Serial.print("현재 모드: ");
  Serial.println(mode);
  Serial.print("Humidity: ");
  Serial.println(hmd);
  Serial.print("Temperature: ");
  Serial.println(tmp);
  Serial.print("Heat index: ");
  Serial.println(hix);
  Serial.print("PMS1.0: ");
  Serial.println(pms_data.PM_AE_UG_1_0);
  Serial.print("PMS2.5: ");
  Serial.println(pms_data.PM_AE_UG_2_5);
  Serial.print("PMS10: ");
  Serial.println(pms_data.PM_AE_UG_10_0);
}

// LCD 화면에 출력하는 함수
void showData() {
  switch (mode) {
    case 0:                 // 모드1 (모든 센서값 간단하게 출력)
      lcd.setCursor(0, 0);  // 출력 시작 위치 (가로, 세로)
      lcd.print("H: ");     // 출력 내용
      lcd.setCursor(3, 0);
      lcd.print(hmd);
      lcd.setCursor(10, 0);
      lcd.print("T: ");
      lcd.setCursor(13, 0);
      lcd.print(tmp);
      lcd.setCursor(0, 1);
      lcd.print("PM1.0:");
      lcd.setCursor(7, 1);
      lcd.print(pms_data.PM_AE_UG_1_0);
      lcd.print("   ");  // 2자리수 출력 후 1자리수 출력 시 남는 잔상 제거
      lcd.setCursor(10, 1);
      lcd.print("PM2.5:");
      lcd.setCursor(17, 1);
      lcd.print(pms_data.PM_AE_UG_2_5);
      lcd.print("   ");
      lcd.setCursor(0, 2);
      lcd.print("PM10:");
      lcd.setCursor(7, 2);
      lcd.print(pms_data.PM_AE_UG_10_0);
      lcd.print("   ");
      break;

    case 1:  // 모드2 (DHT 센서 - 온도, 습도, 체감온도 출력)
      lcd.setCursor(0, 0);
      lcd.print("Humidity: ");
      lcd.setCursor(13, 0);
      lcd.print(hmd);
      lcd.setCursor(0, 1);
      lcd.print("Temperature: ");
      lcd.setCursor(13, 1);
      lcd.print(tmp);
      lcd.setCursor(0, 2);
      lcd.print("Heat index: ");
      lcd.setCursor(13, 2);
      lcd.print(hix);
      break;

    case 2:  // 모드3 (PMS 센서 - 미세먼지 농도 출력)
      lcd.setCursor(0, 0);
      lcd.print("PMS 1.0:");
      lcd.setCursor(9, 0);
      lcd.print(pms_data.PM_AE_UG_1_0);
      lcd.print("   ");
      lcd.setCursor(0, 1);
      lcd.print("PMS 2.5:");
      lcd.setCursor(9, 1);
      lcd.print(pms_data.PM_AE_UG_2_5);
      lcd.print("   ");
      lcd.setCursor(0, 2);
      lcd.print("PMS 10:");
      lcd.setCursor(9, 2);
      lcd.print(pms_data.PM_AE_UG_10_0);
      lcd.print("   ");
      break;

    default:
      break;
  }
}

// 푸시버튼 입력을 감지하는 함수 (모드 변경하는 함수)
void readButton() {
  if (digitalRead(BTNPIN) == LOW) {
    mode = (mode + 1) % MODENUM;  // 모드 변경 (0부터 MODENUM-1까지)

    // 모드 변경될 때마다 LCD 화면 초기화
    lcd.clear();  // LCD 화면 전체 지우기
    showData();  // 빈 LCD 화면이 나오지 않게 데이터 바로 출력하기

    delay(50);  // 플로팅 현상 방지를 위해 50ms 딜레이
    while (true) {
      if (digitalRead(BTNPIN) == HIGH) {
        delay(50);  // 플로팅 현상 방지를 위해 50ms 딜레이
        break;
      }
    }
  }
}

void setup() {
  Serial.begin(9600);     // 시리얼 모니터 출력용
  pmsSerial.begin(9600);  // PMS 센서 통신용
  dht.begin();            // DHT 센서
  lcd.init();             // LCD 초기화
  lcd.backlight();        // LCD 백라이트 켜기

  pinMode(BTNPIN, INPUT_PULLUP);  // 푸시버튼 핀 설정 (풀업 입력)
}

void loop() {
  readData();  // 센서 데이터 읽기

  if (nxtTime <= millis()) {  // 다음 실행 시간이 현재 시간보다 과거인 경우
    showData();               // LCD 디스플레이에 데이터 보여줌
    nxtTime = millis() + 1000;  // 다음 실행 시간을 현재 시간에서 1초 뒤로 설정
  }

  readButton();  // 버튼 신호 감지
}