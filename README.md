# ESP32 F450 MPU6500 Quadcopter

Firmware điều khiển quadcopter F450/F330 dùng ESP32, MPU6500, SBUS, GPS u-blox, la bàn QMC5883 và cảm biến TOF/optical-flow MTF-01P. Dự án có web UI để theo dõi telemetry, trạng thái cảm biến, PID và hiệu chỉnh một số tham số khi chạy.

## Tính năng chính

- Điều khiển 4 ESC bằng PWM 400 Hz theo cấu hình X-frame.
- Đọc IMU MPU6500 qua I2C, lọc complementary, notch và LPF cho gyro.
- Nhận tín hiệu RC qua SBUS, có điều kiện ARM/DISARM và failsafe mất tín hiệu.
- Giữ độ cao bằng MTF-01P, kèm lọc vận tốc độ cao và giới hạn spike.
- Tích hợp GPS u-blox, QMC5883 compass và optical flow để hiển thị/trợ giúp telemetry.
- Web UI chạy trên ESP32 để xem trạng thái, pin, cảm biến, map, PID và gửi lệnh cấu hình.
- Có sketch test riêng cho SBUS, optical flow và xung ESC.

## Phần cứng tham khảo

- ESP32 DevKit / ESP32-WROOM-32.
- MPU6500 IMU, địa chỉ I2C `0x68`.
- QMC5883 compass, địa chỉ I2C `0x0D`.
- Receiver SBUS vào GPIO14.
- GPS UART: RX GPIO34, TX GPIO32.
- MTF-01P UART: RX GPIO16, TX GPIO17.
- ESC: M1 GPIO26, M2 GPIO27, M3 GPIO25, M4 GPIO33 theo khai báo trong firmware.
- Đo pin qua GPIO35 với mạch chia áp.

Chi tiết dây nối và thông số đang nằm trong `07-hardware_config.txt`.

## Thư viện cần cài

- ESP32 board package cho Arduino IDE.
- `ESP32Servo`.
- `SparkFun u-blox GNSS Arduino Library`.

Các thư viện `WiFi`, `WebServer`, `Wire`, `Preferences` và FreeRTOS đi kèm ESP32 core.

## Cấu hình Wi-Fi

Thông tin Wi-Fi thật không được commit lên GitHub. Tạo file `local_config.h` trong thư mục sketch theo mẫu:

```cpp
#pragma once

#define HOME_WIFI_SSID     "TEN_WIFI"
#define HOME_WIFI_PASS     "MAT_KHAU_WIFI"
```

Nếu không có file này, firmware vẫn biên dịch với giá trị mẫu `YOUR_WIFI_SSID` / `YOUR_WIFI_PASSWORD`.

## Cấu trúc thư mục

- `f450_esp32_mpu6500.ino`: firmware chính.
- `mpu6500_driver.*`: driver MPU6500.
- `mtf01p_driver.h`: driver MTF-01P theo khung MICOLINK.
- `web_ui.h`: giao diện web nhúng vào firmware.
- `sbus_test/`, `opticalflow_test/`, `test_xung_esc/`: sketch test từng phần.
- `Lưu đồ PID/`: sơ đồ mô phỏng/luồng PID.

## Lưu ý an toàn

Tháo toàn bộ cánh quạt trước khi nạp code, test ESC, test ARM hoặc chỉnh PID. Kiểm tra chiều motor, chiều prop, nguồn 5V/BEC và GND chung trước khi cấp nguồn pin chính.
