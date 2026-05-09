
const int ledPin = 2;
unsigned long previousMillis = 0;
const long interval = 1000;
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  digitalWrite(ledPin, ledState);
  Serial.println("系统启动，millis 基础定时练习");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);

    // 直接打印 毫秒数
    Serial.print("当前毫秒数：");
    Serial.print(currentMillis);
    Serial.print(" ms  |  LED状态：");
    Serial.println(ledState ? "亮" : "灭");
  }
}

