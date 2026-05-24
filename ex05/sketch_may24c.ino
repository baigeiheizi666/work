#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 20

int level = 1;        // 档位1/2/3
int brightness = 0;
int fadeStep = 5;
// 三档对应呼吸间隔
int speedArr[] = {40,20,8};

unsigned long lastTouchTime = 0;
const int touchHoldTime = 500; // 延长检测防抖时间

// 触摸中断函数
void touchChangeLevel(){
  if(millis() - lastTouchTime > touchHoldTime){
    level++;
    if(level > 3) level = 1;
    lastTouchTime = millis();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN,OUTPUT);
  touchAttachInterrupt(TOUCH_PIN,touchChangeLevel,THRESHOLD);
}

void loop() {
  analogWrite(LED_PIN,brightness);
  brightness += fadeStep;
  
  if(brightness <= 0 || brightness >= 255){
    fadeStep = -fadeStep;
  }
  delay(speedArr[level-1]);
}