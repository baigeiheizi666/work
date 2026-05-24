#include <WiFi.h>
#include <WebServer.h>

// 网络配置
const char* ssid = "OnePlus Ace 3";
const char* password = "1234567890";

// 硬件引脚
const int touchPin = T0;   // GPIO4 (TOUCH0)
// 可选：添加一个 LED 指示（非必须，但可辅助观察）
const int ledPin = 2;

WebServer server(80);

// 网页内容（HTML + CSS + JavaScript）
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
    <title>ESP32 触摸仪表盘</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            background: linear-gradient(135deg, #0f2027, #203a43, #2c5364);
            font-family: 'Segoe UI', 'Arial', sans-serif;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            color: white;
        }
        .dashboard {
            text-align: center;
            background: rgba(0,0,0,0.5);
            backdrop-filter: blur(10px);
            border-radius: 40px;
            padding: 40px 30px;
            width: 90%;
            max-width: 500px;
            box-shadow: 0 15px 35px rgba(0,0,0,0.3);
            border: 1px solid rgba(255,255,255,0.2);
        }
        h1 {
            font-size: 1.8rem;
            margin-bottom: 20px;
            font-weight: 500;
            letter-spacing: 2px;
        }
        .sensor-value {
            font-size: 6rem;
            font-weight: bold;
            background: rgba(0,0,0,0.6);
            display: inline-block;
            padding: 20px 50px;
            border-radius: 60px;
            margin: 20px 0;
            font-family: 'Courier New', monospace;
            letter-spacing: 4px;
            box-shadow: 0 0 20px rgba(0,255,255,0.2);
            transition: all 0.1s ease;
        }
        .unit {
            font-size: 1.5rem;
            margin-left: 10px;
        }
        .status {
            font-size: 1.2rem;
            margin-top: 20px;
            padding: 10px;
            border-radius: 30px;
            background: rgba(255,255,255,0.1);
        }
        .status span {
            font-weight: bold;
            color: #00ffcc;
        }
        footer {
            margin-top: 30px;
            font-size: 0.8rem;
            opacity: 0.7;
        }
        @keyframes pulse {
            0% { text-shadow: 0 0 0px #00ffcc; }
            100% { text-shadow: 0 0 20px #00ffcc; }
        }
        .live {
            animation: pulse 1s infinite alternate;
        }
    </style>
</head>
<body>
    <div class="dashboard">
        <h1>📊 触摸传感器仪表盘</h1>
        <div class="sensor-value" id="touchValue">---</div>
        <div class="status">
            🔵 实时触摸值 &nbsp;|&nbsp; 状态: <span id="touchStatus">未触摸</span>
        </div>
        <footer>✨ 手指靠近 GPIO4 (T0) 数值会下降 ✨</footer>
    </div>

    <script>
        function fetchTouchValue() {
            fetch('/touch')
                .then(response => response.text())
                .then(value => {
                    const val = parseInt(value);
                    const displayDiv = document.getElementById('touchValue');
                    const statusSpan = document.getElementById('touchStatus');
                    displayDiv.innerHTML = val + '<span class="unit"> </span>';
                    // 根据数值范围判断触摸状态（阈值可调，一般空闲80-100，触摸后<30）
                    if (val < 30) {
                        statusSpan.innerHTML = '⚠️ 手指已触碰 ⚠️';
                        statusSpan.style.color = '#ff9966';
                        displayDiv.style.color = '#ff9966';
                    } else if (val < 50) {
                        statusSpan.innerHTML = '👆 靠近中...';
                        statusSpan.style.color = '#ffcc00';
                        displayDiv.style.color = '#ffcc00';
                    } else {
                        statusSpan.innerHTML = '✅ 未触摸';
                        statusSpan.style.color = '#00ffcc';
                        displayDiv.style.color = '#00ffcc';
                    }
                })
                .catch(err => console.log('Error:', err));
        }

        // 每 100ms 更新一次，实现实时跳动效果
        setInterval(fetchTouchValue, 100);
        fetchTouchValue(); // 立即执行一次
    </script>
</body>
</html>
)rawliteral";

// 处理根请求，返回 HTML 页面
void handleRoot() {
    server.send(200, "text/html", index_html);
}

// 处理 /touch 请求，返回当前触摸传感器的数值（模拟量）
void handleTouch() {
    int touchValue = touchRead(touchPin);
    // 可选：根据触摸值控制 LED 亮度（作业不是必须，但增加可观测性）
    // 这里简单让 LED 随触摸值变化，靠近时亮起
    if (touchValue < 40) {
        digitalWrite(ledPin, HIGH);
    } else {
        digitalWrite(ledPin, LOW);
    }
    server.send(200, "text/plain", String(touchValue));
}

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // 连接 WiFi
    WiFi.begin(ssid, password);
    Serial.print("正在连接 WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi 已连接");
    Serial.print("IP 地址: ");
    Serial.println(WiFi.localIP());

    // 配置 Web 服务器路由
    server.on("/", handleRoot);
    server.on("/touch", handleTouch);
    server.begin();
    Serial.println("HTTP 服务器已启动，访问上述 IP 即可查看实时仪表盘");
}

void loop() {
    server.handleClient();  // 处理 HTTP 请求
    // 不需要其他操作，数据采集在 /touch 回调中完成
    delay(10);
}