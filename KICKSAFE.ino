#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// 압력센서 핀
const uint8_t FSR_PINS[6] = {A0, A1, A2, A3, A4, A5};
const int THRESHOLD = 300;

// 모터 핀 (듀얼 드라이브 기준)
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 9;

// RGB LED 핀 (디지털 방식)
const int RED_LED = 10;
const int GREEN_LED = 11;
const int BLUE_LED = 12;

unsigned long startTime = 0;
bool overThreshold = false;
bool triggered = false;

void setup() {
  // 핀 모드 설정
  for (int i = 0; i < 6; i++) pinMode(FSR_PINS[i], INPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(RED_LED, OUTPUT); pinMode(GREEN_LED, OUTPUT); pinMode(BLUE_LED, OUTPUT);

  // LCD 초기화
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");

  // 모터 시작
  startMotor();
}

void loop() {
  int count = 0;

  // 현재 눌린 센서 수 계산
  for (int i = 0; i < 6; i++) {
    int value = analogRead(FSR_PINS[i]);
    if (value >= THRESHOLD) count++;
  }

  // 5개 이상 눌림 처리
  if (count >= 5) {
    if (!overThreshold) {
      overThreshold = true;
      startTime = millis();
    } else {
      if (!triggered && millis() - startTime >= 5000) {
        // 5초 이상 유지됨 → OUT 조건 발동
        triggered = true;
        stopMotorGradually();
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("STOP");
        setColor(255, 0, 0);  // 빨간색
      } else if (!triggered) {
        // 5초 전이라도 경고 상태
        setColor(0, 0, 255);  // 파란색
      }
    }
  } else {
    // 기준 미만 → 초기화
    overThreshold = false;
    triggered = false;
    startMotor();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Ready");
    setColor(0, 255, 0);  // 초록색
  }

  delay(100);
}

void startMotor() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void stopMotorGradually() {
  // 모터 서서히 감속
  for (int i = 0; i < 3; i++) {
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    delay(200);
  }
}

void setColor(int r, int g, int b) {
  digitalWrite(RED_LED, r > 0);
  digitalWrite(GREEN_LED, g > 0);
  digitalWrite(BLUE_LED, b > 0);
}
