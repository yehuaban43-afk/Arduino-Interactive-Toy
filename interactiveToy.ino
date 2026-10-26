// ===== 接腳設定 =====
const int buttonPin = 2;
const int redPin = 3;
const int greenPin = 4;
const int bluePin = 5;
const int lightPin = A0;

// ===== 心情分數 =====
int mood = 10;

// ===== 按鈕防彈跳 =====
int lastReading = HIGH;
int buttonState = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 30;

// ===== 時間紀錄 =====
unsigned long lastTouchTime = 0;
unsigned long lastDecreaseTime = 0;
unsigned long darkStartTime = 0;
unsigned long lastPrintTime = 0;

// ===== 光線門檻 =====
const int darkThreshold = 700;
const int veryDarkThreshold = 900;

bool isDark = false;

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  unsigned long now = millis();

  lastTouchTime = now;
  lastDecreaseTime = now;

  Serial.println("Interactive Toy");
  Serial.println("Mood: 10");
}

void loop() {
  unsigned long now = millis();

  // ===== 讀取光線 =====
  int lightValue = analogRead(lightPin);

  bool currentlyDark = lightValue >= darkThreshold;

  if (currentlyDark && !isDark) {
    darkStartTime = now;
  }

  isDark = currentlyDark;

  // ===== 偵測按鈕 =====
  int reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    lastDebounceTime = now;
  }

  if (now - lastDebounceTime >= debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        if (mood < 20) {
          mood++;
        }

        lastTouchTime = now;
        lastDecreaseTime = now;

        Serial.print("Pet! Mood: ");
        Serial.println(mood);
      }
    }
  }

  lastReading = reading;

  // ===== 判斷黑暗是否超過 3 秒 =====
  bool darkTooLong =
    isDark && (now - darkStartTime >= 3000);

  // ===== 每秒檢查是否扣分 =====
  if (now - lastDecreaseTime >= 1000) {
    lastDecreaseTime = now;

    int decrease = 0;

    if (darkTooLong) {
      if (lightValue >= veryDarkThreshold) {
        decrease = 2;
      }
      else {
        decrease = 1;
      }
    }
    else if (now - lastTouchTime >= 5000) {
      decrease = 1;
    }

    if (decrease > 0) {
      mood -= decrease;

      if (mood < 0) {
        mood = 0;
      }

      Serial.print("Mood: ");
      Serial.println(mood);
    }
  }

  // ===== 每秒顯示光線數值 =====
  if (now - lastPrintTime >= 1000) {
    lastPrintTime = now;

    Serial.print("Light: ");
    Serial.println(lightValue);
  }

  // ===== 顯示娃娃心情 =====
  showMood(mood);
}

// ===== RGB LED 心情顏色 =====
void showMood(int value) {
  int r = 0;
  int g = 0;
  int b = 0;

  if (value <= 10) {
    // 紅色 → 綠色
    r = map(value, 0, 10, 255, 0);
    g = map(value, 0, 10, 0, 255);
    b = 0;
  }
  else {
    // 綠色 → 藍色
    r = 0;
    g = map(value, 10, 20, 255, 0);
    b = map(value, 10, 20, 0, 255);
  }

  // 共陽極 RGB LED
  analogWrite(redPin, 255 - r);
  analogWrite(bluePin, 255 - b);

  // D4 軟體 PWM
  unsigned int phase = micros() % 2048;
  int greenOffLevel = 255 - g;

  if (phase < greenOffLevel * 8UL) {
    digitalWrite(greenPin, HIGH);
  }
  else {
    digitalWrite(greenPin, LOW);
  }
}