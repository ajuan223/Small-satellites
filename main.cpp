#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "common.h"
// 硬件引脚配置
#define CARRIER_PIN 18 // 38kHz载波输出引脚（需支持PWM）
#define DATA_PIN 22 
   // 数据信号输出引脚
#define KEY_PIN 4    // 按键引脚

// NEC协议参数
const uint16_t NEC_HDR_MARK = 9000;    // 引导码高电平时间(us)
const uint16_t NEC_HDR_SPACE = 4500;   // 引导码低电平时间(us)
const uint16_t NEC_BIT_MARK = 560;     // 数据位高电平时间(us)
const uint16_t NEC_ONE_SPACE = 1690;   // 逻辑"1"低电平时间(us)
const uint16_t NEC_ZERO_SPACE = 560;   // 逻辑"0"低电平时间(us)

char datas[4] = {'a', 'b', 'c', 'd'};
uint32_t necData = 0; // 格式：地址 + 地址反码 + 命令 + 命令反码
// // 设定时间
// const uint16_t DATATIME = 1690;// 数据位电平时间(us)
// const uint16_t STARTTIME = 560;

uint8_t txValue = 0;
BLEServer *pServer = NULL;                   // BLEServer指针 pServer
BLECharacteristic *pTxCharacteristic = NULL; // BLECharacteristic指针 pTxCharacteristic
bool deviceConnected = false;                // 本次连接状态
bool oldDeviceConnected = false;             // 上次连接状态

// See the following for generating UUIDs: https://www.uuidgenerator.net/
#define SERVICE_UUID "12a59900-17cc-11ec-9621-0242ac130002" // UART service UUID
#define CHARACTERISTIC_UUID_RX "12a59e0a-17cc-11ec-9621-0242ac130002"
#define CHARACTERISTIC_UUID_TX "12a5a148-17cc-11ec-9621-0242ac130002"

// 示例：发送abcd（可自定义）
// 拼接四字节数据
uint32_t pinjie(const char *data)
{
  return ((uint32_t)data[0] << 24) + ((uint32_t)data[1] << 16) + ((uint32_t)data[2] << 8) + (uint32_t)data[3];
}

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue(); // 接收信息

    if (rxValue.length() > 0)
    { // 向串口输出收到的值
      // Serial.print("RX: ");
      
      for (int i = 0; i < rxValue.length(); i++){
        // Serial.print(rxValue[i]);
        digitalWrite(2, HIGH);
        datas[i] = rxValue[i];
      }
      necData = pinjie(datas);
      // Serial.println();
    }
  }
};




// PWM配置
const int PWM_CHANNEL = 18;  // LEDC通道0
const int PWM_FREQ = 38000; // 38kHz载波
const int PWM_RES = 8;      // 8位分辨率（占空比可调）

//按键状态
typedef enum{
  IDLE,      // 空闲状态
  DEBOUNCE,  // 消抖状态
  PRESSED,   // 按下状态
  LONG_CHECK // 长按检测状态
}KeyState;
static KeyState key_state = IDLE;
static unsigned short key_timer = 0;
#define LONG_PRESS_TIME 5

// 初始化载波生成
void setupCarrier()
{
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(CARRIER_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 128); // 50%占空比（默认关闭）

}

// 按bit发送数据
void sendBit(bool data)
{
  digitalWrite(DATA_PIN, HIGH); // 开启与门

  delayMicroseconds(NEC_BIT_MARK);
  if (data != 0)
  {
    digitalWrite(DATA_PIN, LOW); // 关闭与门
    delayMicroseconds(NEC_ONE_SPACE);
  }
  else
  {
    digitalWrite(DATA_PIN, LOW); // 关闭与门
    delayMicroseconds(NEC_ZERO_SPACE);
  }
}

// 起始标志
void sendStart()
{
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(10);
  digitalWrite(DATA_PIN, HIGH);
  delayMicroseconds(NEC_HDR_MARK);
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(NEC_HDR_SPACE);
}

// 发送结束脉冲
void sendEnd()
{
  digitalWrite(DATA_PIN, HIGH);
  delayMicroseconds(NEC_BIT_MARK);
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(NEC_HDR_SPACE); // 添加此行
}

// 发送数据
void sendData(uint32_t data)
{
  for (int i = 31; i >= 0; i--)
  {
    sendBit((data >> i) & 0x01);
  }
}

// bool is_physical_ley_pressed(){
//   return (digitalRead(KEY_PIN) == LOW);
// }
#define key_down (digitalRead(KEY_PIN) == LOW)

void check_key_task(){
  // bool key_down;
  // key_down = is_physical_ley_pressed();
   switch (key_state)
  {
  case IDLE:
  {
    if (key_down)
    {
      key_state = DEBOUNCE;
      key_timer = 0;
    }
    break;
  }
    case DEBOUNCE:{
      if(key_down){
        key_timer++;
        if(key_timer >= 1){
          key_state = PRESSED;
          key_timer = 0;
        }
      }else{
        key_state = IDLE;
      }
      break;
    }
    case PRESSED:{
      if(key_down){
        key_timer++;
        if(key_timer >= LONG_PRESS_TIME){
          key_state = LONG_CHECK;
        }
      }else{//短按事件
        sendStart();
        delayMicroseconds(10);
        sendData(necData);
        delayMicroseconds(10);
        sendEnd();
        delay(10);
        key_state = IDLE;
      }
      break;
    }
    case LONG_CHECK:{//长按事件
      while (key_down){
        sendStart();
        delayMicroseconds(10);
        sendData(necData);
        delayMicroseconds(10);
        sendEnd();
        delay(10);
      }
        if (!key_down)
        {
          key_state = IDLE;
        }
      break;
    }
    }
}

// // 按键检测
// void key_Scan(){
//   if(digitalRead(KEY_PIN) == LOW){
//     delay(10);//按键消抖
//     if (digitalRead(KEY_PIN) == LOW){
//       delay(500);//检测长按
//       if (digitalRead(KEY_PIN) == LOW){
//         sendStart();
//         while (digitalRead(KEY_PIN) == LOW){
//           sendData(necData);
//           delay(100);
//         }
//       }else{
//         sendStart();
//         sendData(necData);
//         delay(100);
//       }
//       sendEnd();
//     }
//   }
// }

void setup(){
  pinMode(DATA_PIN, OUTPUT);

  setupCarrier();
  pinMode(KEY_PIN, INPUT_PULLUP);

  pinMode(2, OUTPUT);

  Serial.begin(115200);

  // 创建一个 BLE 设备
  BLEDevice::init("NO.1group");

  // 创建一个 BLE 服务
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); // 设置回调
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // 创建一个 BLE 特征
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks()); // 设置回调

  pService->start();                  // 开始服务
  pServer->getAdvertising()->start(); // 开始广播
  // Serial.println(" 等待一个客户端连接，且发送通知... ");
}

void loop(){
  // key_Scan();
  check_key_task();
  // delay(200); // 冷却
  // deviceConnected 已连接

  // disconnecting  断开连接
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);                  // 留时间给蓝牙缓冲
    pServer->startAdvertising(); // 重新广播
    Serial.println(" 开始广播 ");
    oldDeviceConnected = deviceConnected;
  }

  // connecting  正在连接
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}