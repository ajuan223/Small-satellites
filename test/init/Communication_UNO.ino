unsigned long lastUpdate = 0;
int counter = 0;

void setup() {
  Serial.begin(9600);  // UNO TX0/RX0 连接 ESP
}

void loop() {
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    counter++;
    String msg = String(counter);
    Serial.print("VALUE: ");
    Serial.println(msg);  // 发给 ESP，同时也是串口输出
  }
}
