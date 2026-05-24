#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "OnePlus Ace 3";
const char* password = "1234567890";

const int LED = 2;
const int TOUCH = T0;

WebServer server(80);
bool isArm = false;
bool isAlarm = false;

// 触摸动态基准（布防时记录空闲值）
int touchBaseline = 0;
const int TOUCH_OFFSET = 50;   // 当触摸值比基准小超过此值时触发

// LED 非阻塞闪烁
unsigned long lastBlink = 0;
bool ledState = false;

String webPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
</head>
<body style="text-align:center;font-size:22px;margin-top:50px">
<h2>作业8 安防报警</h2>
<p>状态：<span id="sta">未布防</span></p>
<button onclick="fetch('/arm')">布防</button>
<button onclick="fetch('/disarm')">撤防</button>
<script>
setInterval(()=>{
  fetch('/sta').then(x=>x.text()).then(s=>{
    document.getElementById('sta').innerText=s
  })
},500)
</script>
</body>
</html>
)rawliteral";
}

void home() {
  server.send(200, "text/html;charset=utf-8", webPage());
}

void arm() {
  // 布防前，先读取当前触摸值作为基准（此时不应触摸）
  touchBaseline = touchRead(TOUCH);
  Serial.print("布防基准值: ");
  Serial.println(touchBaseline);
  
  isArm = true;
  isAlarm = false;
  digitalWrite(LED, LOW);
  Serial.println("系统已布防");
  server.send(200, "text/plain", "已布防");
}

void disarm() {
  isArm = false;
  isAlarm = false;
  digitalWrite(LED, LOW);
  Serial.println("系统已撤防");
  server.send(200, "text/plain", "未布防");
}

void status() {
  if (isAlarm) server.send(200, "text/plain", "报警中");
  else if (isArm) server.send(200, "text/plain", "已布防");
  else server.send(200, "text/plain", "未布防");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi已连接");
  Serial.print("IP地址：");
  Serial.println(WiFi.localIP());

  server.on("/", home);
  server.on("/arm", arm);
  server.on("/disarm", disarm);
  server.on("/sta", status);
  server.begin();
  Serial.println("HTTP服务器启动");
}

void loop() {
  server.handleClient();

  // 触摸检测：仅在布防且未报警时进行
  if (isArm && !isAlarm) {
    int currentTouch = touchRead(TOUCH);
    // 调试：每500ms打印一次（可选，调试后可注释）
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500) {
      lastPrint = millis();
      Serial.printf("当前触摸值: %d, 基准: %d, 差值: %d\n", currentTouch, touchBaseline, touchBaseline - currentTouch);
    }
    
    // 如果当前值比基准小超过偏移量，认为触摸触发
    if (touchBaseline - currentTouch > TOUCH_OFFSET) {
      isAlarm = true;
      Serial.println("⚠️ 警报触发！⚠️");
    }
  }

  // LED 非阻塞闪烁（仅在报警时）
  if (isAlarm) {
    unsigned long now = millis();
    if (now - lastBlink >= 100) {  // 100ms 闪烁一次
      lastBlink = now;
      ledState = !ledState;
      digitalWrite(LED, ledState);
    }
  } else {
    digitalWrite(LED, LOW);
    ledState = false;
  }
}