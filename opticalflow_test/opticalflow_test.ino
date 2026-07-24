#include <Arduino.h>










#define FLOW_RX_PIN 16
#define FLOW_TX_PIN 17
#define FLOW_BAUD   115200

#define MICOLINK_MSG_HEAD        0xEF
#define MICOLINK_MAX_PAYLOAD_LEN 64
#define MICOLINK_MSG_ID_RANGE_SENSOR 0x51

HardwareSerial FlowSerial(2);

struct MicolinkMsg {
  uint8_t head = 0;
  uint8_t devId = 0;
  uint8_t sysId = 0;
  uint8_t msgId = 0;
  uint8_t seq = 0;
  uint8_t len = 0;
  uint8_t payload[MICOLINK_MAX_PAYLOAD_LEN] = {0};
  uint8_t checksum = 0;
  uint8_t status = 0;
  uint8_t payloadCnt = 0;
};

#pragma pack(push, 1)
struct RangeSensorPayload {
  uint32_t timeMs;
  uint32_t distanceMm;
  uint8_t strength;
  uint8_t precision;
  uint8_t tofStatus;
  uint8_t reserved1;
  int16_t flowVelX;       
  int16_t flowVelY;       
  uint8_t flowQuality;
  uint8_t flowStatus;
  uint16_t reserved2;
};
#pragma pack(pop)

MicolinkMsg msg;
RangeSensorPayload lastPayload = {};

uint32_t byteCount = 0;
uint32_t goodFrames = 0;
uint32_t badFrames = 0;
uint32_t otherFrames = 0;
uint32_t lastFrameMs = 0;
uint32_t lastPrintMs = 0;

bool checkChecksum(const MicolinkMsg &m) {
  uint8_t sum = 0;
  sum += m.head;
  sum += m.devId;
  sum += m.sysId;
  sum += m.msgId;
  sum += m.seq;
  sum += m.len;
  for (uint8_t i = 0; i < m.len; i++) sum += m.payload[i];
  return sum == m.checksum;
}

bool parseByte(uint8_t data) {
  switch (msg.status) {
    case 0:
      if (data == MICOLINK_MSG_HEAD) {
        msg.head = data;
        msg.payloadCnt = 0;
        msg.status = 1;
      }
      break;
    case 1:
      msg.devId = data;
      msg.status = 2;
      break;
    case 2:
      msg.sysId = data;
      msg.status = 3;
      break;
    case 3:
      msg.msgId = data;
      msg.status = 4;
      break;
    case 4:
      msg.seq = data;
      msg.status = 5;
      break;
    case 5:
      msg.len = data;
      msg.payloadCnt = 0;
      if (msg.len == 0) {
        msg.status = 7;
      } else if (msg.len > MICOLINK_MAX_PAYLOAD_LEN) {
        msg.status = 0;
        badFrames++;
      } else {
        msg.status = 6;
      }
      break;
    case 6:
      msg.payload[msg.payloadCnt++] = data;
      if (msg.payloadCnt >= msg.len) {
        msg.status = 7;
      }
      break;
    case 7:
      msg.checksum = data;
      msg.status = 0;
      if (checkChecksum(msg)) return true;
      badFrames++;
      break;
    default:
      msg.status = 0;
      msg.payloadCnt = 0;
      break;
  }
  return false;
}

void handleFrame() {
  if (msg.msgId != MICOLINK_MSG_ID_RANGE_SENSOR) {
    otherFrames++;
    return;
  }

  if (msg.len != sizeof(RangeSensorPayload)) {
    badFrames++;
    return;
  }

  memcpy(&lastPayload, msg.payload, sizeof(lastPayload));
  goodFrames++;
  lastFrameMs = millis();
}

void readFlowSerial() {
  while (FlowSerial.available()) {
    uint8_t c = (uint8_t)FlowSerial.read();
    byteCount++;
    if (parseByte(c)) handleFrame();
  }
}

void printStatus() {
  const uint32_t now = millis();
  if (now - lastPrintMs < 200) return;
  lastPrintMs = now;

  const uint32_t age = lastFrameMs ? (now - lastFrameMs) : 0xFFFFFFFFUL;
  const float heightM = lastPayload.distanceMm > 0 ? lastPayload.distanceMm / 1000.0f : 0.0f;
  const float velXcmS = lastPayload.flowVelX * heightM;
  const float velYcmS = lastPayload.flowVelY * heightM;

  Serial.print("bytes=");
  Serial.print(byteCount);
  Serial.print(" frames=");
  Serial.print(goodFrames);
  Serial.print(" bad=");
  Serial.print(badFrames);
  Serial.print(" age=");
  if (age == 0xFFFFFFFFUL) Serial.print("NO_FRAME");
  else Serial.print(age);

  Serial.print(" | dist=");
  Serial.print(lastPayload.distanceMm);
  Serial.print("mm ");
  Serial.print(heightM, 3);
  Serial.print("m");

  Serial.print(" strength=");
  Serial.print(lastPayload.strength);
  Serial.print(" tofStatus=");
  Serial.print(lastPayload.tofStatus);

  Serial.print(" | flowRawX=");
  Serial.print(lastPayload.flowVelX);
  Serial.print(" flowRawY=");
  Serial.print(lastPayload.flowVelY);
  Serial.print(" flowVelX=");
  Serial.print(velXcmS, 1);
  Serial.print("cm/s flowVelY=");
  Serial.print(velYcmS, 1);
  Serial.print("cm/s");

  Serial.print(" q=");
  Serial.print(lastPayload.flowQuality);
  Serial.print(" flowStatus=");
  Serial.println(lastPayload.flowStatus);
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println();
  Serial.println("=== MTF-01P optical flow test ===");
  Serial.println("UART sensor: Serial2, RX=GPIO16, TX=GPIO17, baud=115200");
  Serial.println("Move sensor over textured floor. flowRawX/Y and quality should change.");

  FlowSerial.setRxBufferSize(1024);
  FlowSerial.begin(FLOW_BAUD, SERIAL_8N1, FLOW_RX_PIN, FLOW_TX_PIN, false);
}

void loop() {
  readFlowSerial();
  printStatus();

  if (Serial.available()) {
    char c = (char)Serial.read();
    if (c == 'r' || c == 'R') {
      byteCount = goodFrames = badFrames = otherFrames = 0;
      lastFrameMs = 0;
      Serial.println("Stats reset.");
    }
  }
}
