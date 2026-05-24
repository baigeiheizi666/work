#define LED1_PIN 2
#define LED2_PIN 4

int brightness = 0;
int fadeStep = 5;

void setup() {
  Serial.begin(115200);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
}

void loop() {
  analogWrite(LED1_PIN, brightness);
  analogWrite(LED2_PIN, 255 - brightness);

  // 串口显示双灯亮度
  Serial.print("LED1：");
  Serial.print(brightness);
  Serial.print(" | LED2：");
  Serial.println(255 - brightness);

  brightness += fadeStep;
  if (brightness <= 0 || brightness >= 255) {
    fadeStep = -fadeStep;
  }
  delay(30);
}