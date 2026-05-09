
const int ledPin = 2;

unsigned long previousMillis = 0;
const long interval = 500; // 500ms 切换一次状态，周期1秒（1Hz）
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200); // 开启串口，波特率设为115200
  digitalWrite(ledPin, ledState);
  Serial.println("系统启动，LED开始闪烁");
  Serial.print("LED引脚号：");
  Serial.println(ledPin);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    
    // 串口打印状态
    Serial.print("时间：");
    Serial.print(currentMillis / 1000.0, 1); // 保留1位小数，单位秒
    Serial.print("s | LED状态：");
    Serial.println(ledState ? "亮" : "灭");
  }
}


