#include <Arduino.h>
#include "mpu6500_driver.h"

bool MPU6500Driver::begin(uint8_t sda, uint8_t scl) {
    Wire.begin(sda, scl);
    Wire.setClock(400000);

    uint8_t who = readReg(REG_WHO_AM_I);
    Serial.printf("[MPU] WHO_AM_I = 0x%02X (mong doi 0x70)\n", who);
    if (who != 0x70) {
        Serial.println("[MPU] KHONG TIM THAY MPU6500!");
        return false;
    }

    writeReg(REG_PWR_MGMT_1, 0x80);
    delay(100);

    writeReg(REG_PWR_MGMT_1, 0x01);
    delay(10);

    writeReg(REG_SMPLRT_DIV, 0x03);

    writeReg(REG_CONFIG, 0x04);

    writeReg(REG_GYRO_CONFIG, 0x08);

    writeReg(REG_ACCEL_CONFIG, 0x08);

    writeReg(REG_ACCEL_CONFIG2, 0x04);

    _lastUs = micros();
    Serial.println("[MPU] Khoi tao thanh cong.");
    return true;
}

void MPU6500Driver::calibrate(uint16_t samples) {
    Serial.println("[MPU] Dang hieu chuan Gyro & Accel, vui long giu yen...");
    long gx_sum = 0, gy_sum = 0, gz_sum = 0;
    long ax_sum = 0, ay_sum = 0, az_sum = 0;

    for (uint16_t i = 0; i < samples; i++) {
        MPU6500_Raw r = readRaw();
        gx_sum += r.gx;
        gy_sum += r.gy;
        gz_sum += r.gz;
        ax_sum += r.ax;
        ay_sum += r.ay;
        az_sum += r.az;
        delay(4);
    }

    _calib.gx_off = (float)gx_sum / samples / GYRO_SCALE;
    _calib.gy_off = (float)gy_sum / samples / GYRO_SCALE;
    _calib.gz_off = (float)gz_sum / samples / GYRO_SCALE;

    float ax_avg = (float)ax_sum / samples / ACCEL_SCALE;
    float ay_avg = (float)ay_sum / samples / ACCEL_SCALE;
    float az_avg = (float)az_sum / samples / ACCEL_SCALE;

    
    
    if (fabs(ax_avg) > 0.7f) {
        _calib.ax_off = ax_avg - (ax_avg > 0 ? 1.0f : -1.0f);
        _calib.ay_off = 0.0f;
        _calib.az_off = 0.0f;
    }
    else if (fabs(ay_avg) > 0.7f) {
        _calib.ax_off = 0.0f;
        _calib.ay_off = ay_avg - (ay_avg > 0 ? 1.0f : -1.0f);
        _calib.az_off = 0.0f;
    }
    else {
        _calib.ax_off = 0.0f;
        _calib.ay_off = 0.0f;
        _calib.az_off = az_avg - (az_avg > 0 ? 1.0f : -1.0f);
    }

    Serial.printf("[MPU] Calib xong. G_Bias: %.2f %.2f %.2f | A_Bias: %.2f %.2f %.2f\n", 
        _calib.gx_off, _calib.gy_off, _calib.gz_off,
        _calib.ax_off, _calib.ay_off, _calib.az_off);
}

void MPU6500Driver::update() {

    unsigned long nowUs = micros();
    float dt = (nowUs - _lastUs) * 1e-6f;
    if (dt <= 0.0f || dt > 1.0f) dt = 0.001f;
    _lastUs = nowUs;

    MPU6500_Raw r = readRaw();

    _data.ax = r.ax / ACCEL_SCALE - _calib.ax_off;
    _data.ay = r.ay / ACCEL_SCALE - _calib.ay_off;
    _data.az = r.az / ACCEL_SCALE - _calib.az_off;
    _data.gx = r.gx / GYRO_SCALE  - _calib.gx_off;
    _data.gy = r.gy / GYRO_SCALE  - _calib.gy_off;
    _data.gz = r.gz / GYRO_SCALE  - _calib.gz_off;
    _data.temp = r.temp / 340.0f + 36.53f;
    _data.accNorm = sqrtf(_data.ax * _data.ax + _data.ay * _data.ay + _data.az * _data.az);
    float gyroAbs = fabsf(_data.gx) + fabsf(_data.gy) + fabsf(_data.gz);
    float accErrScore = constrain(fabsf(_data.accNorm - 1.0f) * 220.0f, 0.0f, 100.0f);
    float gyroScore = constrain((gyroAbs - 180.0f) * 0.20f, 0.0f, 100.0f);
    float vibMoi = max(accErrScore, gyroScore);
    _vibrationScore = _vibrationScore * 0.90f + vibMoi * 0.10f;
    _data.vibrationScore = _vibrationScore;
    _data.accTrusted = (_data.accNorm >= 0.80f && _data.accNorm <= 1.25f && _data.vibrationScore < 70.0f);

    float lpf_acc = 0.05f;
    _ax_filt = _ax_filt * (1.0f - lpf_acc) + _data.ax * lpf_acc;
    _ay_filt = _ay_filt * (1.0f - lpf_acc) + _data.ay * lpf_acc;
    _az_filt = _az_filt * (1.0f - lpf_acc) + _data.az * lpf_acc;

    float accRoll  = atan2f(_ay_filt, _az_filt)                             * RAD_TO_DEG;
    float accPitch = atan2f(-_ax_filt, sqrtf(_ay_filt*_ay_filt + _az_filt*_az_filt)) * RAD_TO_DEG;

    const float alpha = _data.accTrusted ? 0.98f : 0.995f;
    _data.fusionAlpha = alpha;
    _data.angle.roll  = alpha * (_data.angle.roll  + _data.gx * dt) + (1.0f-alpha) * accRoll;
    _data.angle.pitch = alpha * (_data.angle.pitch + _data.gy * dt) + (1.0f-alpha) * accPitch;

    _data.angle.yaw += _data.gz * dt;

    if (_data.angle.yaw >  180.0f) _data.angle.yaw -= 360.0f;
    if (_data.angle.yaw < -180.0f) _data.angle.yaw += 360.0f;
}

void MPU6500Driver::writeReg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

uint8_t MPU6500Driver::readReg(uint8_t reg) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0;
}

void MPU6500Driver::readBurst(uint8_t reg, uint8_t* buf, uint8_t len) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)MPU_ADDR, len);
    for (uint8_t i = 0; i < len; i++) {
        buf[i] = Wire.available() ? Wire.read() : 0;
    }
}

MPU6500_Raw MPU6500Driver::readRaw() {
    uint8_t buf[14];
    readBurst(REG_ACCEL_XOUT_H, buf, 14);
    MPU6500_Raw r;
    r.ax   = toInt16(buf[0],  buf[1]);
    r.ay   = toInt16(buf[2],  buf[3]);
    r.az   = toInt16(buf[4],  buf[5]);
    r.temp = toInt16(buf[6],  buf[7]);
    r.gx   = toInt16(buf[8],  buf[9]);
    r.gy   = toInt16(buf[10], buf[11]);
    r.gz   = toInt16(buf[12], buf[13]);
    return r;
}
