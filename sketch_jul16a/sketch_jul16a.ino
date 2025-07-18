#include <Adafruit_MPU6050.h>
#include <Adafruit_LSM303DLH_Mag.h>
#include <Adafruit_Sensor.h>
#include <math.h>

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_accel,*mpu_gyro;
Adafruit_LSM303DLH_Mag_Unified mag = Adafruit_LSM303DLH_Mag_Unified(12345);

volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;

unsigned long lastUpdate = 0;
float sampleFreq = 100.0f;

float Kp = 1.5f;//收敛速度
float Ki = 0.005f;//修正速度

void updateQuaternionProducts() {
    q0q0 = q0 * q0;
    q0q1 = q0 * q1;
    q0q2 = q0 * q2;
    q0q3 = q0 * q3;
    q1q1 = q1 * q1;
    q1q2 = q1 * q2;
    q1q3 = q1 * q3;
    q2q2 = q2 * q2;
    q2q3 = q2 * q3;
    q3q3 = q3 * q3;
}

void MahonyAHRSupdate(float g_x, float g_y, float g_z, float a_x, float a_y, float a_z, float m_x, float m_y, float m_z) {
    float norm;
    float h_x, h_y, h_z, b_x, b_z;
    float v_x, v_y, v_z, w_x, w_y, w_z;
    float e_x, e_y, e_z;
    updateQuaternionProducts();
    
    //归一化
    //加速度
    norm = sqrt(a_x * a_x + a_y * a_y + a_z * a_z);
    if (norm == 0.0f) return;
    a_x /= norm;
    a_y /= norm;
    a_z /= norm;
    //磁力计
    norm = sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    if (norm == 0.0f) return; 
    m_x /= norm;
    m_y /= norm;
    m_z /= norm;

    //参考矢量
    //机体旋转至水平
    h_x = 2.0f * (m_x * (0.5f - q2q2 - q3q3) + m_y * (q1q2 - q0q3) + m_z * (q1q3 + q0q2));
    h_y = 2.0f * (m_x * (q1q2 + q0q3) + m_y * (0.5f - q1q1 - q3q3) + m_z * (q2q3 - q0q1));
    h_z = 2.0f * (m_x * (q1q3 - q0q2) + m_y * (q2q3 + q0q1) + m_z * (0.5f - q1q1 - q2q2));
    //水平转回机体
    b_x = sqrt(h_x * h_x + h_y * h_y);
    b_z = h_z;
    //重力
    v_x = 2.0f * (q1q3 - q0q2);
    v_y = 2.0f * (q0q1 + q2q3);
    v_z = q0q0 - q1q1 - q2q2 + q3q3;
    //地磁
    w_x = 2.0f * (b_x * (0.5f - q2q2 - q3q3) + b_z * (q1q3 - q0q2));
    w_y = 2.0f * (b_x * (q1q2 - q0q3) + b_z * (q0q1 + q2q3));
    w_z = 2.0f * (b_x * (q1q3 + q0q2) + b_z * (0.5f - q1q1 - q2q2));

    //误差
    e_x = (a_y * v_z - a_z * v_y) + (m_y * w_z - m_z * w_y);
    e_y = (a_z * v_x - a_x * v_z) + (m_z * w_x - m_x * w_z);
    e_z = (a_x * v_y - a_y * v_x) + (m_x * w_y - m_y * w_x);

    //校正
    //积分
    if (Ki > 0.0f) {
        exInt += e_x * (1.0f / sampleFreq);
        eyInt += e_y * (1.0f / sampleFreq);
        ezInt += e_z * (1.0f / sampleFreq);
    } else {
        exInt = 0.0f;
        eyInt = 0.0f;
        ezInt = 0.0f;
    }
    //比例
    g_x += Kp * e_x + Ki * exInt;
    g_y += Kp * e_y + Ki * eyInt;
    g_z += Kp * e_z + Ki * ezInt;

    //更新
    float dt = 1.0f / sampleFreq;
    q0 += (-q1 * g_x - q2 * g_y - q3 * g_z) * 0.5f * dt;
    q1 += (q0 * g_x + q2 * g_z - q3 * g_y) * 0.5f * dt;
    q2 += (q0 * g_y - q1 * g_z + q3 * g_x) * 0.5f * dt;
    q3 += (q0 * g_z + q1 * g_y - q2 * g_x) * 0.5f * dt;

    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 /= norm;
    q1 /= norm;
    q2 /= norm;
    q3 /= norm;
}

void setup(void) {
  Serial.begin(115200);
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  
  mpu_accel = mpu.getAccelerometerSensor();
  mpu_accel->printSensorDetails();
  mpu_gyro = mpu.getGyroSensor();
  mpu_gyro->printSensorDetails();
  Serial.println("MPU6050 Found!");

  // 初始化磁力计
  if (!mag.begin()) {
    Serial.println("Failed to find LSM303DLH_Mag chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Mag Found!");

  updateQuaternionProducts();
  lastUpdate = micros();
}

void loop(){
  sensors_event_t accel,gyro,event;
  mpu_accel->getEvent(&accel);
  mpu_gyro->getEvent(&gyro);
  mag.getEvent(&event);
    //读取传感器数据
    float g_x = gyro.gyro.x;
    float g_y = gyro.gyro.y;
    float g_z = gyro.gyro.z;

    float a_x = accel.acceleration.x;
    float a_y = accel.acceleration.y;
    float a_z = accel.acceleration.z+0.8;

    float m_x = event.magnetic.x;
    float m_y = event.magnetic.y;
    float m_z = event.magnetic.z;

    //更新dt
    unsigned long now = micros();
    float dt = (now - lastUpdate) / 1000000.0f;
    if (dt > 0) {
      sampleFreq = 1.0f / dt; 
    } 
    lastUpdate = now;

    MahonyAHRSupdate(g_x, g_y, g_z, a_x, a_y, a_z, m_x, m_y, m_z);

    float roll, pitch, yaw;
    roll  = atan2(2.0f * (q0q1 + q2q3), q0q0 - q1q1 - q2q2 + q3q3);
    pitch = -asin(2.0f * (q1q3 - q0q2));
    yaw   = atan2(2.0f * (q1q2 + q0q3), q0q0 + q1q1 - q2q2 - q3q3);

    //Serial.print("Roll: "); 
    Serial.print(roll * 180.0f / M_PI);
    Serial.print("\t");
    //Serial.print("\t Pitch: "); 
    Serial.print(pitch * 180.0f / M_PI);
    Serial.print("\t");
    //Serial.print("\t Yaw: "); 
    Serial.println(yaw * 180.0f / M_PI);
    
    delay(10); 

}