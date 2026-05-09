// ex03.ino - 用 millis() 实现 SOS 闪烁信号 + 串口调试（115200波特率）
const int ledPin =2;

// 时间参数（单位：毫秒）
const int SHORT_ON  = 200;   // 短闪亮时间
const int SHORT_OFF = 200;   // 短闪灭时间
const int LONG_ON   = 600;   // 长闪亮时间
const int LONG_OFF  = 200;   // 长闪灭时间
const int PAUSE     = 2000;  // 一次SOS结束后的停顿时间

// 状态变量
unsigned long previousMillis = 0;
int phase = 0;   // 0:短闪, 1:长闪, 2:结束停顿
int count = 0;   // 当前阶段的闪烁次数计数
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200); // 开启串口，波特率设为115200
  digitalWrite(ledPin, LOW);
  Serial.println("系统启动，开始输出SOS信号");
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long interval;

  // 根据当前阶段和LED状态，决定下一次切换的间隔时间
  if (phase == 0) {
    interval = (ledState == HIGH) ? SHORT_ON : SHORT_OFF;
  } else if (phase == 1) {
    interval = (ledState == HIGH) ? LONG_ON : LONG_OFF;
  } else {
    interval = PAUSE;
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (phase == 2) {
      // 停顿结束，重置状态，开始下一轮SOS
      phase = 0;
      count = 0;
      ledState = LOW;
      digitalWrite(ledPin, LOW);
      Serial.println("--- 一次SOS信号发送完毕，开始停顿 ---");
    } else {
      // 翻转LED状态
      ledState = !ledState;
      digitalWrite(ledPin, ledState);

      // 每次LED熄灭时计数，判断阶段是否结束
      if (ledState == LOW) {
        count++;
        Serial.print("阶段：");
        Serial.print(phase == 0 ? "短闪" : "长闪");
        Serial.print(" | 第");
        Serial.print(count);
        Serial.println("次闪烁结束");

        if (phase == 0 && count == 3) {
          phase = 1;
          count = 0;
          Serial.println("切换到长闪阶段...");
        } else if (phase == 1 && count == 3) {
          phase = 2;
          count = 0;
          ledState = LOW;
          digitalWrite(ledPin, LOW);
          Serial.println("切换到停顿阶段...");
        }
      } else {
        Serial.print("LED状态变为：");
        Serial.println(ledState ? "亮" : "灭");
      }
    }
  }
}
