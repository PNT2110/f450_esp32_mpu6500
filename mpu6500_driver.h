#pragma once
#include <Wire.h>
#include <math.h>

#define MPU_ADDR        0x68

#define REG_SMPLRT_DIV   0x19
#define REG_CONFIG       0x1A
#define REG_GYRO_CONFIG  0x1B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_CONFIG2 0x1D
#define REG_ACCEL_XOUT_H 0x3B
#define REG_PWR_MGMT_1   0x6B
#define REG_WHO_AM_I     0x75

#define ACCEL_SCALE  8192.0f
#define GYRO_SCALE   65.5f

#define CALIB_SAMPLES  500

struct MPU6500_Raw {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    int16_t temp;
};

struct MPU6500_Calib {
    float ax_off, ay_off, az_off;
    float gx_off, gy_off, gz_off;
};

struct MPU6500_Angles {
    float roll;
    float pitch;
    float yaw;
};

struct MPU6500_Data {
    float ax, ay, az;
    float gx, gy, gz;
    float temp;
    float accNorm;
    float vibrationScore;
    float fusionAlpha;
    bool  accTrusted;
    MPU6500_Angles angle;
};

class MPU6500Driver {
public:
    MPU6500Driver() {}

    bool    begin(uint8_t sda = 21, uint8_t scl = 22);
    void    calibrate(uint16_t samples = CALIB_SAMPLES);
    void    update();

    MPU6500_Data    getData()   const { return _data;  }

private:
    MPU6500_Calib  _calib = {0, 0, 0, 0, 0, 0};
    MPU6500_Data   _data  = {};
    unsigned long  _lastUs = 0;

    float _ax_filt = 0.0f;
    float _ay_filt = 0.0f;
    float _az_filt = 1.0f;
    float _vibrationScore = 0.0f;

    void     writeReg(uint8_t reg, uint8_t val);
    uint8_t  readReg (uint8_t reg);
    void     readBurst(uint8_t reg, uint8_t* buf, uint8_t len);
    MPU6500_Raw readRaw();

    static inline int16_t toInt16(uint8_t hi, uint8_t lo) {
        return (int16_t)((uint16_t)(hi << 8) | lo);
    }
};
