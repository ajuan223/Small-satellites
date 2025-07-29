#include <ESP8266WiFi.h>

const char* ssid = "2025 X-Challenge";
const char* password = "lingyi@2025";
const char* host = "172.31.123.31";
const int port = 3456;

void setup() {
  // 启动串口。这个串口既用于和Uno通信，也用于连接电脑的串口监视器进行调试
  Serial.begin(9600); 
  
  // 连接到指定的Wi-Fi网络
  WiFi.begin(ssid, password);

  // 打印连接状态，直到连接成功
  Serial.print("正在连接Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(); // 换行

  // 打印连接成功信息和获取到的IP地址
  Serial.print("连接成功! IP地址为: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // 检查串口是否有来自Arduino Uno的数据
  WiFiClient tcpClient;
  if (Serial.available()) {
    // 读取整行数据 (直到遇到换行符 '\n')
    String msg = Serial.readStringUntil('\n');
    msg.remove(msg.length() - 1);
    Serial.print("[ESP收到消息] ");
    // Serial.println(msg);
    if(tcpClient.connect(host, port)){
        tcpClient.write(msg.c_str());
        Serial.println("send");
    }
    else {
        Serial.println("连接失败");
    }
  }
  delay(50);
}