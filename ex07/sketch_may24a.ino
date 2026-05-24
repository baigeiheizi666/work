#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "OnePlus Ace 3";
const char* password = "1234567890";
const int LED = 2;
WebServer server(80);

String makePage()
{
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<style>
body{text-align:center;font-size:18px;margin-top:40px;}
.slide{width:70%;margin:15px auto;}
</style>
</head>
<body>
<h1>单路LED调光</h1>
<p>亮度数值：<span id="val">0</span></p>
<input class="slide" type="range" id="slider" min="0" max="255" value="0">
<br>
<button onclick="fetch('/allon')">点亮</button>
<button onclick="fetch('/alloff')">熄灭</button>

<script>
let slider = document.getElementById('slider');
let showVal = document.getElementById('val');
slider.oninput = function(){
  showVal.innerText = this.value;
  fetch('/set?pwm='+this.value);
}
</script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot(){
  server.send(200,"text/html;charset=utf-8",makePage());
}

void setLed(){
  int pwm = server.arg("pwm").toInt();
  analogWrite(LED,pwm);
  server.send(200,"text/plain","ok");
}

void allOn(){
  analogWrite(LED,255);
  server.send(200,"text/plain","ok");
}

void allOff(){
  analogWrite(LED,0);
  server.send(200,"text/plain","ok");
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  WiFi.begin(ssid,password);
  Serial.print("WiFi连接中");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n网页访问地址：");
  Serial.println(WiFi.localIP());
  server.on("/",handleRoot);
  server.on("/set",setLed);
  server.on("/allon",allOn);
  server.on("/alloff",allOff);
  server.begin();
}

void loop(){
  server.handleClient();
}