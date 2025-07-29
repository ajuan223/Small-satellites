// AHRS.cpp
#include "AHRS.h"

AHRS::AHRS()
  : mag(12345), q0(1.0f), q1(0.0f), q2(0.0f), q3(0.0f),
    exInt(0.0f), eyInt(0.0f), ezInt(0.0f),
    lastUpdate(0), sampleFreq(100.0f),
    Kp(1.5f), Ki(0.005f) {
  // Constructor initializes values
}

bool AHRS::begin() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    return false;
  }
  Serial.println("MPU6050 Found!");
  mpu_accel = mpu.getAccelerometerSensor();
  mpu_gyro = mpu.getGyroSensor();

  if (!mag.begin()) {
    Serial.println("Failed to find LSM303DLH_Mag chip");
    return false;
  }
  Serial.println("Mag Found!");
  lastUpdate = micros();
  return true;
}

void AHRS::update() {
  // Read sensor data
  sensors_event_t accel_event, gyro_event, mag_event;
  mpu_accel->getEvent(&accel_event);
  mpu_gyro->getEvent(&gyro_event);
  mag.getEvent(&mag_event);

  // Update sample frequency
  unsigned long now = micros();
  float dt = (now - lastUpdate) / 1000000.0f;
  if (dt > 0.0f) {
    sampleFreq = 1.0f / dt;
  }
  lastUpdate = now;

  // Run the Mahony filter
  MahonyAHRSupdate(
    gyro_event.gyro.x, gyro_event.gyro.y, gyro_event.gyro.z,
    accel_event.acceleration.x, accel_event.acceleration.y, accel_event.acceleration.z + 0.8,
    mag_event.magnetic.x, mag_event.magnetic.y, mag_event.magnetic.z);

  // Calculate angles from quaternion
  roll = atan2(2.0f * (q0q1 + q2q3), q0q0 - q1q1 - q2q2 + q3q3) * 180.0f / M_PI;
  pitch = -asin(2.0f * (q1q3 - q0q2)) * 180.0f / M_PI;
  yaw = atan2(2.0f * (q1q2 + q0q3), q0q0 + q1q1 - q2q2 - q3q3) * 180.0f / M_PI;
}

// All the original algorithm code is now part of the class
void AHRS::updateQuaternionProducts() {
    q0q0 = q0 * q0; q0q1 = q0 * q1; q0q2 = q0 * q2; q0q3 = q0 * q3;
    q1q1 = q1 * q1; q1q2 = q1 * q2; q1q3 = q1 * q3;
    q2q2 = q2 * q2; q2q3 = q2 * q3;
    q3q3 = q3 * q3;
}

void AHRS::MahonyAHRSupdate(float g_x, float g_y, float g_z, float a_x, float a_y, float a_z, float m_x, float m_y, float m_z) {
    float norm;
    float h_x, h_y, h_z, b_x, b_z;
    float v_x, v_y, v_z, w_x, w_y, w_z;
    float e_x, e_y, e_z;
    updateQuaternionProducts();

    norm = sqrt(a_x * a_x + a_y * a_y + a_z * a_z);
    if (norm == 0.0f) return;
    a_x /= norm; a_y /= norm; a_z /= norm;

    norm = sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    if (norm == 0.0f) return;
    m_x /= norm; m_y /= norm; m_z /= norm;

    h_x = 2.0f * (m_x * (0.5f - q2q2 - q3q3) + m_y * (q1q2 - q0q3) + m_z * (q1q3 + q0q2));
    h_y = 2.0f * (m_x * (q1q2 + q0q3) + m_y * (0.5f - q1q1 - q3q3) + m_z * (q2q3 - q0q1));
    h_z = 2.0f * (m_x * (q1q3 - q0q2) + m_y * (q2q3 + q0q1) + m_z * (0.5f - q1q1 - q2q2));
    b_x = sqrt(h_x * h_x + h_y * h_y);
    b_z = h_z;

    v_x = 2.0f * (q1q3 - q0q2);
    v_y = 2.0f * (q0q1 + q2q3);
    v_z = q0q0 - q1q1 - q2q2 + q3q3;

    w_x = 2.0f * (b_x * (0.5f - q2q2 - q3q3) + b_z * (q1q3 - q0q2));
    w_y = 2.0f * (b_x * (q1q2 - q0q3) + b_z * (q0q1 + q2q3));
    w_z = 2.0f * (b_x * (q1q3 + q0q2) + b_z * (0.5f - q1q1 - q2q2));

    e_x = (a_y * v_z - a_z * v_y) + (m_y * w_z - m_z * w_y);
    e_y = (a_z * v_x - a_x * v_z) + (m_z * w_x - m_x * w_z);
    e_z = (a_x * v_y - a_y * v_x) + (m_x * w_y - m_y * w_x);

    if (Ki > 0.0f) {
        exInt += e_x * (1.0f / sampleFreq);
        eyInt += e_y * (1.0f / sampleFreq);
        ezInt += e_z * (1.0f / sampleFreq);
    } else {
        exInt = 0.0f; eyInt = 0.0f; ezInt = 0.0f;
    }

    g_x += Kp * e_x + Ki * exInt;
    g_y += Kp * e_y + Ki * eyInt;
    g_z += Kp * e_z + Ki * ezInt;

    float dt = 1.0f / sampleFreq;
    q0 += (-q1 * g_x - q2 * g_y - q3 * g_z) * 0.5f * dt;
    q1 += (q0 * g_x + q2 * g_z - q3 * g_y) * 0.5f * dt;
    q2 += (q0 * g_y - q1 * g_z + q3 * g_x) * 0.5f * dt;
    q3 += (q0 * g_z + q1 * g_y - q2 * g_x) * 0.5f * dt;

    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 /= norm; q1 /= norm; q2 /= norm; q3 /= norm;
}