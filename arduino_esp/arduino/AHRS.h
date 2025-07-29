// AHRS.h
#ifndef AHRS_H
#define AHRS_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_LSM303DLH_Mag.h>
#include <Adafruit_Sensor.h>
#include <math.h>

class AHRS {
public:
  // Constructor
  AHRS();

  // Public methods
  bool begin();
  void update();
  float getRoll() const { return roll; }
  float getPitch() const { return pitch; }
  float getYaw() const { return yaw; }

private:
  // Sensor Objects
  Adafruit_MPU6050 mpu;
  Adafruit_Sensor *mpu_accel, *mpu_gyro;
  Adafruit_LSM303DLH_Mag_Unified mag;

  // AHRS algorithm variables
  volatile float q0, q1, q2, q3;
  float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
  float exInt, eyInt, ezInt;
  float Kp, Ki;

  // Timing and results
  unsigned long lastUpdate;
  float sampleFreq;
  float roll, pitch, yaw;

  // Private internal methods
  void updateQuaternionProducts();
  void MahonyAHRSupdate(float g_x, float g_y, float g_z, float a_x, float a_y, float a_z, float m_x, float m_y, float m_z);
};

#endif // AHRS_H