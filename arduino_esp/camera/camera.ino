#include <WiFi.h>
#include <ESP32Servo.h>
#include "esp_camera.h"
#include "board_config.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define SERVO1_PIN 12
#define SERVO2_PIN 13

const char* ssid = "2025 X-Challenge";
const char* password = "lingyi@2025";

const char* backend_host = "172.31.123.31"; 
const uint16_t backend_tcp_port = 3456;

WiFiClient tcpClient;

unsigned long lastCaptureTime = 0;
const long captureInterval = 100; // 0.1s

unsigned long lastReconnectAttempt = 0;
const long reconnectInterval = 5000; // 5秒

// 声明函数
bool initCamera();
char* base64_encode(const unsigned char *data, size_t input_length);
void captureAndSendImage();
void handleServerCommands();

Servo servo1;
Servo servo2;

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // 為了網路傳輸效率，建議使用較小的解析度
  config.frame_size = FRAMESIZE_QVGA; // 320x240, 適合快速傳輸
  config.jpeg_quality = 12; // 數值越小，壓縮率越高，圖片越小
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  if (!initCamera()) {
    Serial.println("Rebooting due to camera init failure...");
    delay(5000);
    ESP.restart();
  }

  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo1.attach(SERVO1_PIN,500,2400);
  servo2.attach(SERVO2_PIN,500,2400);
  servo1.write(90);
  servo2.write(90);

  // 連接到 WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!tcpClient.connected()) {
    // 如果断开，每隔5秒尝试重连一次
    if (millis() - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = millis();
      Serial.printf("Attempting to connect to TCP server %s:%d\n", backend_host, backend_tcp_port);
      if (tcpClient.connect(backend_host, backend_tcp_port)) {
        Serial.println("TCP connection successful!");
      } else {
        Serial.println("TCP connection failed.");
      }
    }
  } else {
    // 如果已连接，则执行双向通信任务
    handleServerCommands();  // 检查并处理来自服务器的指令
    delay(50);
    captureAndSendImage();   // 检查是否到了发送图片的时间
  }
  delay(200);
}

void handleServerCommands() {
  auto servo1num=0, servo2num=0;
  auto flag1=false, flag2=false;
  while (tcpClient.available()){
    char msg = tcpClient.read();
      auto* servo=&servo2num;
      auto num=0;
      if(msg=='1') {
        servo=&servo1num;
        flag1=true;
        msg = tcpClient.read();
        msg = tcpClient.read();
      }
      else {
        flag2=true;
        msg = tcpClient.read();
        msg = tcpClient.read();
      }
      while(msg!=';'){
        num*=10;
        Serial.println(num);
        Serial.println("11111");
        num+=msg - '0';
        //Serial.println(msg);
        Serial.println(num);
        Serial.println("22222");
        msg = tcpClient.read();
        // Serial.println(msg);
      }
      *servo=num;
      msg = tcpClient.read();
      Serial.println("44444");
  }
  if (flag1 && servo1num) servo1.write(servo1num);
  if (flag2 && servo2num) servo2.write(servo2num);
}

void captureAndSendImage() {
  if (millis() - lastCaptureTime > captureInterval) {
    lastCaptureTime = millis();

    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) { Serial.println("Capture failed"); return; }

    char *b64_data = base64_encode(fb->buf, fb->len);
    esp_camera_fb_return(fb);

    if (!b64_data) { Serial.println("Base64 failed"); return; }
    
    String json = "{\"type\":\"image\",\"data\":\"";
    json += b64_data;
    json += "\"}";
    free(b64_data);

    // 通过已建立的连接发送数据
    tcpClient.print(json);
    Serial.println("Image data sent.");
  }
}

static const char b64_table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

// 它接收原始二进制数据，返回一个需要手动释放内存的 char* 指针
char* base64_encode(const unsigned char *data, size_t input_length) {
  // 计算编码后字符串的长度
  size_t output_length = 4 * ((input_length + 2) / 3);
  
  // 分配内存来存放编码后的字符串 (+1 用于存放字符串结束符'\0')
  char *encoded_data = (char*) malloc(output_length + 1);
  if (encoded_data == NULL) return NULL;

  for (size_t i = 0, j = 0; i < input_length;) {
    uint32_t octet_a = i < input_length ? data[i++] : 0;
    uint32_t octet_b = i < input_length ? data[i++] : 0;
    uint32_t octet_c = i < input_length ? data[i++] : 0;

    uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

    encoded_data[j++] = b64_table[(triple >> 3 * 6) & 0x3F];
    encoded_data[j++] = b64_table[(triple >> 2 * 6) & 0x3F];
    encoded_data[j++] = b64_table[(triple >> 1 * 6) & 0x3F];
    encoded_data[j++] = b64_table[(triple >> 0 * 6) & 0x3F];
  }

  // 根据原始数据长度，在末尾添加'='作为填充
  static const int mod_table[] = {0, 2, 1};
  for (int i = 0; i < mod_table[input_length % 3]; i++) {
    encoded_data[output_length - 1 - i] = '=';
  }

  // 添加字符串结束符
  encoded_data[output_length] = '\0';

  return encoded_data;
}
