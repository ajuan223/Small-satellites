#include "AHRS.h"

AHRS ahrs;

void setup() {
  Serial.begin(9600);
  Serial.println(F("系统启动..."));

  if (!ahrs.begin()) {
    Serial.println(F("AHRS 初始化失败，程序暂停。"));
    while (1) { delay(10); }
  }
  Serial.println(F("AHRS 系统准备就绪。"));
  
}

void loop() {
  ahrs.update();
  float roll = ahrs.getRoll();
  float pitch = ahrs.getPitch();
  float yaw = ahrs.getYaw();

  String jsonData = String("{\"type\":\"data\",") +
                  "\"roll\":" + String(roll, 2) + "," +
                  "\"pitch\":" + String(pitch, 2) + "," +
                  "\"yaw\":" + String(yaw, 2) + "}";
  Serial.println(jsonData);
  delay(100);
}
