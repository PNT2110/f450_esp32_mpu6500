#ifndef MTF01P_DRIVER_H
#define MTF01P_DRIVER_H

#include <Arduino.h>

#define MICOLINK_MSG_HEAD        0xEF
#define MICOLINK_MAX_PAYLOAD_LEN 64
#define MICOLINK_MAX_LEN         (MICOLINK_MAX_PAYLOAD_LEN + 7)

enum {
    MICOLINK_MSG_ID_RANGE_SENSOR = 0x51,
};

typedef struct {
    uint8_t head;
    uint8_t dev_id;
    uint8_t sys_id;
    uint8_t msg_id;
    uint8_t seq;
    uint8_t len;
    uint8_t payload[MICOLINK_MAX_PAYLOAD_LEN];
    uint8_t checksum;
    
    uint8_t status;
    uint8_t payload_cnt;
} MICOLINK_MSG_t;

#pragma pack(push, 1)
typedef struct {
    uint32_t time_ms;
    uint32_t distance;          
    uint8_t  strength;          
    uint8_t  precision;         
    uint8_t  tof_status;        
    uint8_t  reserved1;
    int16_t  flow_vel_x;        
    int16_t  flow_vel_y;        
    uint8_t  flow_quality;      
    uint8_t  flow_status;       
    uint16_t reserved2;
} MICOLINK_PAYLOAD_RANGE_SENSOR_t;
#pragma pack(pop)

class MTF01PDriver {
private:
  HardwareSerial* serial;
  MICOLINK_MSG_t msg;

  bool micolink_check_sum() {
    uint8_t length = msg.len + 6;
    uint8_t checksum = 0;
    checksum += msg.head;
    checksum += msg.dev_id;
    checksum += msg.sys_id;
    checksum += msg.msg_id;
    checksum += msg.seq;
    checksum += msg.len;
    for(uint8_t i = 0; i < msg.len; i++) {
        checksum += msg.payload[i];
    }
    return (checksum == msg.checksum);
  }

  bool micolink_parse_char(uint8_t data) {
    switch(msg.status) {
      case 0:
        if(data == MICOLINK_MSG_HEAD) {
            msg.head = data;
            msg.status++;
        }
        break;
      case 1:
        msg.dev_id = data;
        msg.status++;
        break;
      case 2:
        msg.sys_id = data;
        msg.status++;
        break;
      case 3:
        msg.msg_id = data;
        msg.status++;
        break;
      case 4:
        msg.seq = data;
        msg.status++;
        break;
      case 5:
        msg.len = data;
        if (msg.len == 0) msg.status += 2;
        else if (msg.len > MICOLINK_MAX_PAYLOAD_LEN) msg.status = 0;
        else msg.status++;
        break;
      case 6:
        msg.payload[msg.payload_cnt++] = data;
        if(msg.payload_cnt == msg.len) {
            msg.payload_cnt = 0;
            msg.status++;
        }
        break;
      case 7:
        msg.checksum = data;
        msg.status = 0;
        if(micolink_check_sum()) return true;
        break;
      default:
        msg.status = 0;
        msg.payload_cnt = 0;
        break;
    }
    return false;
  }

public:
  float altitudeM = 0;
  
  
  int16_t raw_flow_x = 0;
  int16_t raw_flow_y = 0;
  
  uint8_t rangeQuality = 0;
  uint8_t flowQuality = 0;
  uint8_t rangeStatus = 0;
  uint8_t flowStatus = 0;
  uint32_t lastDistanceMm = 0;
  uint16_t lastPayloadLen = 0;

  uint32_t byteCount = 0;
  uint32_t frameCount = 0;
  uint32_t badFrames = 0; 

  unsigned long lastRangeMs = 0;
  unsigned long lastFlowMs = 0;

  void begin(HardwareSerial* s, uint32_t baud, int rxPin, int txPin, bool invert = false) {
    serial = s;
    serial->begin(baud, SERIAL_8N1, rxPin, txPin, invert);
    msg.status = 0;
    msg.payload_cnt = 0;
  }

  void update(uint16_t maxBytes = 256) {
    if (!serial) return;
    uint16_t processed = 0;
    while (serial->available() && processed++ < maxBytes) {
      uint8_t c = serial->read();
      byteCount++;
      if (micolink_parse_char(c)) {
        if (msg.msg_id == MICOLINK_MSG_ID_RANGE_SENSOR && msg.len == sizeof(MICOLINK_PAYLOAD_RANGE_SENSOR_t)) {
            MICOLINK_PAYLOAD_RANGE_SENSOR_t payload;
            memcpy(&payload, msg.payload, msg.len);
            
            frameCount++;
            lastPayloadLen = msg.len;
            rangeStatus = payload.tof_status;
            flowStatus = payload.flow_status;
            lastDistanceMm = payload.distance;
            
            if (payload.distance > 0) {
              altitudeM = payload.distance / 1000.0f;
              rangeQuality = payload.strength;
              lastRangeMs = millis();
            }
            
            raw_flow_x = payload.flow_vel_x;
            raw_flow_y = payload.flow_vel_y;
            flowQuality = payload.flow_quality;
            lastFlowMs = millis();
        } else {
            badFrames++;
        }
      }
    }
  }
};

#endif
