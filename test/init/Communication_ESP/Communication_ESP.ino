#include <ESP8266WiFi.h>

const char* ssid = "2025 X-Challenge";
const char* password = "lingyi@2025";

void setup() {
  Serial.begin(9600); // ESP 与 UNO 通信串口
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    Serial.print("[ESP RECEIVED] ");
    Serial.println(msg); // 显示到 USB-TTL 串口终端
  }
}
