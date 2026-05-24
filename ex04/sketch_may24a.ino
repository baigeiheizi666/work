#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 20

bool ledState = false;
unsigned long lastTouchTime = 0;
const unsigned long touchDelay = 500; // 延长检测时间：500毫秒防抖

void gotTouch() {
  // 延长检测时间：500ms内只触发一次，防止松手乱跳
  if (millis() - lastTouchTime > touchDelay) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    lastTouchTime = millis();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  touchAttachInterrupt(TOUCH_PIN, gotTouch, THRESHOLD);
}

void loop() {
  delay(100);
}