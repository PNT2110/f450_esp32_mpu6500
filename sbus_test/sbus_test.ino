








#define PIN_SBUS_RX 14

uint16_t CH[7] = {0}; 
bool sbusOk = false;
bool failsafe = false;
unsigned long tSbusPrev = 0;
unsigned long tPrint = 0;
uint32_t frameCount = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(100000, SERIAL_8E2, PIN_SBUS_RX, -1, true); 
  delay(500);
  Serial.println();
  Serial.println("========================================");
  Serial.println("  SBUS TEST - Chi doc tay cam SBUS");
  Serial.println("  Pin RX: GPIO 14");
  Serial.println("========================================");
  Serial.println();
}

void docSBUS() {
  static uint8_t buf[25], idx = 0;
  while (Serial1.available()) {
    uint8_t b = Serial1.read();
    if (idx == 0 && b != 0x0F) continue;
    buf[idx++] = b;
    if (idx >= 25) {
      idx = 0;
      if (buf[0] != 0x0F) continue;
      uint16_t r[6];
      r[0] = (buf[1] | buf[2] << 8) & 0x7FF;
      r[1] = (buf[2] >> 3 | buf[3] << 5) & 0x7FF;
      r[2] = (buf[3] >> 6 | buf[4] << 2 | buf[5] << 10) & 0x7FF;
      r[3] = (buf[5] >> 1 | buf[6] << 7) & 0x7FF;
      r[4] = (buf[6] >> 4 | buf[7] << 4) & 0x7FF;
      r[5] = (buf[7] >> 7 | buf[8] << 1 | buf[9] << 9) & 0x7FF;
      for (int i = 0; i < 6; i++)
        CH[i + 1] = constrain(map(r[i], 172, 1811, 1000, 2000), 1000, 2000);
      failsafe = (buf[23] >> 3) & 1;
      sbusOk = !((buf[23] >> 2) & 1) && !failsafe;
      tSbusPrev = millis();
      frameCount++;
    }
  }
  if (!tSbusPrev) { sbusOk = false; return; }
  if (millis() - tSbusPrev > 1000) { sbusOk = false; failsafe = true; }
}

void loop() {
  docSBUS();

  
  if (millis() - tPrint >= 200) {
    tPrint = millis();

    Serial.printf("SBUS: %s | FS: %s | Frames: %u\n",
                  sbusOk ? "OK" : "FAIL",
                  failsafe ? "YES" : "no",
                  frameCount);
    Serial.printf("  CH1(Roll):  %4d  |  CH2(Pitch): %4d\n", CH[1], CH[2]);
    Serial.printf("  CH3(Ga):    %4d  |  CH4(Yaw):   %4d\n", CH[3], CH[4]);
    Serial.printf("  CH5(Aux1):  %4d  |  CH6(Aux2):  %4d\n", CH[5], CH[6]);
    Serial.println("----------------------------------------");
  }
}
