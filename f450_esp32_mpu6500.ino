#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <Preferences.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "mpu6500_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "web_ui.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <math.h>

#if defined(__has_include)
#  if __has_include("local_config.h")
#    include "local_config.h"
#  endif
#endif

#ifndef HOME_WIFI_SSID
#define HOME_WIFI_SSID     "YOUR_WIFI_SSID"
#endif

#ifndef HOME_WIFI_PASS
#define HOME_WIFI_PASS     "YOUR_WIFI_PASSWORD"
#endif

#define WIFI_CHECK_MS      2000UL
#define WIFI_STA_TIMEOUT   12000UL
#define WIFI_STA_RETRY_MS  10000UL
#define SBUS_TIMEOUT_MS    350UL
#define SBUS_CENTER_MIN    1450
#define SBUS_CENTER_MAX    1550
#define SBUS_LOW_MAX       1120

#define PIN_SDA         21
#define PIN_SCL         22
#define PIN_SBUS_RX     14
#define PIN_BATT        35
#define PIN_GPS_RX      34
#define PIN_GPS_TX      32
#define PIN_MTF_RX      16 
#define PIN_MTF_TX      17
const int PIN_ESC[4] = {26, 27, 25, 33};

#define ESC_MIN_US       1000
#define ESC_MAX_US       2000
#define MUC_GA_NGHI      1150
#define GA_KHOI_DONG_MAX 1150
#define GA_HOAT_DONG_MIN 1200

#define ALT_HOLD_GA_TAM     1500
#define ALT_HOLD_GA_DEADBAND 80
#define ALT_HOLD_CH6_ON_US  1600
#define ALT_HOLD_CH6_OFF_US 1400
#define ALT_HOLD_BU_LIMIT   220
#define ALT_HOLD_ALT_KP     0.70f
#define ALT_HOLD_VZ_MAX_MPS 0.35f
#define ALT_HOLD_STICK_VZ_MAX_MPS 0.35f
#define ALT_HOLD_TARGET_MIN_M 0.10f
#define ALT_HOLD_TARGET_MAX_M 5.00f
#define ALT_VEL_LPF_ALPHA   0.22f
#define ALT_VEL_RAW_LIMIT_MPS 1.50f
#define ALT_VEL_SPIKE_LIMIT_MPS 2.50f
#define ALT_SAMPLE_JUMP_MAX_M 0.18f
#define ALT_SPIKE_BU_LIMIT 80
#define ALT_FILTER_SAMPLES  5

#define GIOI_HAN_GOC     20.0f
#define GOC_AN_TOAN      45.0f
#define MAX_RATE_DPS     180.0f
#define MAX_YAW_RATE_DPS 60.0f
#define PID_OUT_LIMIT    380.0f
#define PID_INT_LIMIT    80.0f
#define MOTOR_CORR_LIMIT 320.0f
#define STICK_DEADBAND   20
#define YAW_STICK_DEADBAND 45
#define YAW_HOLD_ERR_LIMIT_DEG 45.0f
#define DEFAULT_ROLL_OFFSET  -3.0f
#define DEFAULT_PITCH_OFFSET  0.7f
#define NHIP_FLIGHT_MS   4

#define PIN_YEU_V        10.8f
#define PIN_CAN_V        10.5f
#define PIN_DOC_MIN_V    1.0f
#define HA_CANH_GIAM_US_S 30.0f
#define HA_CANH_ALT_GIAM_M_S 0.18f
#define HA_CANH_GAN_DAT_OFFSET_M 0.08f
#define HA_CANH_IDLE_MS  3000UL

#define FLOW_MIN_QUALITY  10
#define FLOW_MAP_MAX_DT_S 0.20f

#define CONFIG_VERSION   45
#define DEFAULT_RP_KP    1.10f 
#define DEFAULT_RP_KI    0.12f
#define DEFAULT_RP_KD    0.025f
#define DEFAULT_ANGLE_KP 6.00f
#define DEFAULT_ANGLE_KI 0.00f
#define DEFAULT_ANGLE_KD 0.00f
#define DEFAULT_YAW_KP   5.0f
#define DEFAULT_YAW_KI   1.0f
#define DEFAULT_YAW_KD   0.08f
#define DEFAULT_YAW_HOLD_KP 0.00f
#define DEFAULT_YAW_HOLD_KI 0.00f
#define DEFAULT_YAW_HOLD_KD 0.00f
#define DEFAULT_ALT_VZ_KP 120.0f
#define DEFAULT_ALT_VZ_KI 35.0f
#define DEFAULT_ALT_VZ_KD 0.0f
#define DEFAULT_OPT_KP   0.30f
#define DEFAULT_OPT_KI   0.10f
#define DEFAULT_OPT_KD   0.05f
#define DEFAULT_NOTCH_HZ 80.0f
#define DEFAULT_NOTCH_BW 20.0f

#define DTERM_LPF_HZ          70.0f
#define ANGLE_DTERM_LPF_HZ    40.0f
#define ALT_DTERM_LPF_HZ      20.0f
#define FLOW_DTERM_LPF_HZ     35.0f
#define GYRO_LPF2_CUTOFF_HZ   90.0f
#define ANGLE_PID_INT_LIMIT    5.0f
#define ALT_PID_INT_LIMIT      2.5f
#define FLOW_PID_INT_LIMIT     30.0f
#define FLOW_PID_OUT_LIMIT_DEG 12.0f

#define GPS_BAUD_INIT    38400UL
#define GPS_BAUD_TARGET  115200UL
#define GPS_FORCE_NMEA   0
#define GPS_TIMEOUT_MS   2000UL
#define MTF_TIMEOUT_MS   1000UL
#define GPS_MIN_SATS     6
#define GPS_MAX_HDOP     2.5f
#define GPS_POS_JUMP_MAX_M    35.0f
#define GPS_REACQUIRE_MS      3000UL
#define GPS_POS_LPF_ALPHA_MIN 0.25f
#define GPS_POS_LPF_ALPHA_MAX 0.60f
#define GPS_ALT_LPF_ALPHA     0.35f
#define GPS_COMPASS_MIN_SPD   5.0f
#define GPS_COMPASS_WARN_DEG  60.0f

#define QMC5883_ADDR     0x0D
#define QMC5883_ADDR_ALT 0x2C
#define QMC_READ_MS      100UL

#define QMC_MOUNT_OFFSET 90.0f
#define QMC_HEADING_LPF  0.18f
#define QMC_YAW_CORR_ALPHA     0.005f
#define QMC_YAW_CORR_MAX_STEP  0.12f
#define QMC_YAW_FAST_CORR_ALPHA 0.008f
#define QMC_YAW_FAST_MAX_STEP   0.18f
#define QMC_HEADING_MAX_STEP   95.0f
#define QMC_HEADING_RESYNC_FRAMES 6
#define QMC_FIELD_JUMP_RATIO   0.60f
#define QMC_FIELD_MIN_XY       30.0f
#define QMC_FIELD_MAX_XY       30000.0f
#define QMC_CAL_SPAN_MIN       800.0f
#define QMC_YAW_HOLD_DEADBAND  0.5f
#define QMC_MPU_YAW_REJECT_DEG 55.0f
#define QMC_MPU_YAW_RECOVER_DEG 35.0f
#define QMC_MPU_YAW_BAD_FRAMES 5
#define YAW_VIB_REJECT_SCORE   55.0f
#define YAW_SAFE_OUT_LIMIT     180.0f
#define YAW_LOW_THR_OUT_LIMIT  120.0f
#define YAW_LOW_THR_MARGIN_US  150
#define YAW_HOLD_SETTLE_MS     700UL

#include "mtf01p_driver.h"

#define ENABLE_WEB_UI    1
#define ENABLE_COMPASS   1
#define ENABLE_ALT_HOLD  1
#define ENABLE_POS_HOLD  1

#define IMU_SWAP_RP      1
#define IMU_ROLL_SIGN   -1.0f
#define IMU_PITCH_SIGN   1.0f
#define IMU_YAW_SIGN     1.0f
#define IMU_GX_SIGN     -1.0f
#define IMU_GY_SIGN      1.0f
#define IMU_GZ_SIGN      1.0f

const float MIX_ROLL[4]  = {-1, -1, +1, +1};
const float MIX_PITCH[4] = {+1, -1, -1, +1};
const float MIX_YAW[4]   = {+1, -1, +1, -1};

struct NotchFilter {
  float b0, b1, b2, a1, a2;
  float x1=0, x2=0, y1=0, y2=0;
  void reset() { x1=x2=y1=y2=0; }
  void init(float fc, float bw, float fs) {
    reset();
    float ny = fs*0.5f;
    fc = constrain(fc, 20.0f, ny-10.0f);
    bw = constrain(bw, 5.0f, 50.0f);
    float q = constrain(fc/bw, 0.7f, 30.0f);
    float w0 = 2.0f*PI*fc/fs;
    float alpha = sin(w0)/(2.0f*q);
    float a0 = 1.0f+alpha;
    b0=1.0f/a0; b1=-2.0f*cos(w0)/a0; b2=1.0f/a0;
    a1=b1; a2=(1.0f-alpha)/a0;
  }
  float apply(float in) {
    float out = b0*in + b1*x1 + b2*x2 - a1*y1 - a2*y2;
    x2=x1; x1=in; y2=y1; y1=out;
    return out;
  }
};

struct BiquadLPF2 {
  float b0, b1, b2, a1, a2;
  float x1=0, x2=0, y1=0, y2=0;
  void reset() { x1=x2=y1=y2=0; }
  void init(float cutHz, float sampleHz) {
    reset();
    float fr = cutHz/sampleHz;
    float ohm = tanf(PI*fr);
    float c = 1.0f + 2.0f*cosf(PI/4.0f)*ohm + ohm*ohm;
    b0 = ohm*ohm/c; b1 = 2.0f*b0; b2 = b0;
    a1 = 2.0f*(ohm*ohm-1.0f)/c;
    a2 = (1.0f - 2.0f*cosf(PI/4.0f)*ohm + ohm*ohm)/c;
  }
  float apply(float in) {
    float out = b0*in + b1*x1 + b2*x2 - a1*y1 - a2*y2;
    x2=x1; x1=in; y2=y1; y1=out;
    return out;
  }
};

struct KalmanFilter1D {
  float x=0, P=1, Q=1, R=1, K=0;
  bool ready=false;
  void init(float q, float r, float v=0) { Q=q; R=r; x=v; P=1; K=0; ready=false; }
  void reset() { ready=false; x=0; P=1; K=0; }
  float update(float z) {
    if(!ready) { x=z; P=1; ready=true; return x; }
    P+=Q; K=P/(P+R); x+=K*(z-x); P=(1-K)*P; return x;
  }
  float updateAngle(float z, float wrap=360.0f) {
    if(!ready) { x=z; P=1; ready=true; return x; }
    P+=Q; K=P/(P+R);
    float inn=z-x; float h=wrap*0.5f;
    while(inn>h) inn-=wrap; while(inn<-h) inn+=wrap;
    x+=K*inn;
    while(x>=wrap) x-=wrap; while(x<0) x+=wrap;
    P=(1-K)*P; return x;
  }
};

struct PID {
  float kp=0, ki=0, kd=0;
  float iSum=0, prevMeas=0, dFilt=0;
  float outLim=PID_OUT_LIMIT, iLim=PID_INT_LIMIT;
  BiquadLPF2 dLpf;
  bool hasPrev=false, dReady=false;

  void setLimits(float oLim, float iL, float dCutHz) {
    outLim=max(1.0f,fabsf(oLim)); iLim=max(0.0f,fabsf(iL));
    dLpf.init(constrain(dCutHz,10.0f,125.0f*0.45f), 1000.0f/NHIP_FLIGHT_MS);
    dReady=true;
  }
  void reset() { iSum=0; prevMeas=0; dFilt=0; hasPrev=false; dLpf.reset(); }
  float compute(float target, float current, float dt) {
    if(dt<=0||dt>0.05f) dt=NHIP_FLIGHT_MS/1000.0f;
    float err = target - current;
    iSum = constrain(iSum + err*dt, -iLim, iLim);
    float dRaw = 0;
    if(hasPrev) dRaw = -(current-prevMeas)/dt;
    else hasPrev=true;
    prevMeas = current;
    dFilt = dReady ? dLpf.apply(dRaw) : dRaw;
    return constrain(kp*err + ki*iSum + kd*dFilt, -outLim, outLim);
  }
};

MPU6500Driver mpu;
Servo dongCo[4];
WebServer srv(80);
Preferences nvs;
SFE_UBLOX_GNSS myGPS;
HardwareSerial& GpsSerial = Serial;  
HardwareSerial MtfSerial(2);         
SemaphoreHandle_t dataMutex;
MTF01PDriver mtf;

NotchFilter notchR, notchP, notchY;
BiquadLPF2 gyroLpfR, gyroLpfP, gyroLpfY;
KalmanFilter1D kfCompass, kfGpsLat, kfGpsLon, kfFlowX, kfFlowY;
float notchFreq=DEFAULT_NOTCH_HZ, notchBw=DEFAULT_NOTCH_BW;

PID pidRateR, pidRateP, pidYaw, pidAngleR, pidAngleP, pidAngleY, pidAlt, pidFlowX, pidFlowY;

struct {
  bool armed=false, pidOn=false, altHold=false;
  bool imuOk=false, sbusOk=false, gpsOk=false, compassOk=false, altOk=false;
  bool failsafe=false, safety=false, lowBat=false, critBat=false, autoLand=false;
} sys;

float gocRoll=0, gocPitch=0, gocYaw=0;
float rollOffset=DEFAULT_ROLL_OFFSET, pitchOffset=DEFAULT_PITCH_OFFSET;
float gx_filt=0, gy_filt=0, yawRate=0;
float gx_raw=0, gy_raw=0, gz_raw=0;
float yawGyroRaw=0;
float yawCompassOff=0;
float compassOffset=0;
float yawCompareErrDeg=0;
float rollOut=0, pitchOut=0, yawOut=0;
float dt_flight=NHIP_FLIGHT_MS/1000.0f;
unsigned long tFlightPrev=0;
bool yawFusionReady=false, yawCompareOk=false;
uint8_t yawCompareBadCnt=0;

float batV=0, batPct=0, heSoPin=5.0f;
float flowAngleDeg=0.0f;
float mpuTemp=0, espTemp=0;

int CH[7]    = {0,1500,1500,1000,1500,1000,1000};
int tamCH[5] = {0,1500,1500,1000,1500};
int esc[4]   = {1000,1000,1000,1000};
int mucGa    = ESC_MIN_US;

float tgtAngleR=0, tgtAngleP=0, tgtAngleY=0;
float tgtRateR=0, tgtRateP=0, tgtRateY=0;
float tgtAlt=0;
float tgtAltVel=0;
float altVelMps=0, altVelRawMps=0;
bool altVelHealthy=true;
uint16_t altVelSpikeCount=0;
float hoverThrottle=MUC_GA_NGHI;
float pidOutR=0, pidOutP=0, pidOutY=0, pidOutAlt=0;
float gaAutoLand=MUC_GA_NGHI;
bool altHoldActive=false, yawHoldActive=false;
unsigned long yawHoldSettleStartMs=0;

volatile float flowVelX=0, flowVelY=0, flowPosX=0, flowPosY=0;

unsigned long tArmGesture=0, tDisarmGesture=0;
unsigned long tAutoLandPrev=0, tAutoLandIdle=0;
bool armGesture=false, disarmGesture=false;

unsigned long wifiCheckPrev=0, wifiRetryPrev=0;
bool wifiOk=false;
uint8_t wifiSignal=0, sbusSignal=0;

struct {
  bool ok=false, fix=false, filterReady=false, rawMode=false;
  uint8_t sats=0;
  float hdop=99.9f;
  double lat=0, lon=0;
  float altM=0, speedKmh=0, courseDeg=0;
  uint32_t pollCount=0, rawBytes=0;
  unsigned long lastMs=0, lastByteMs=0;
  char state[22]="INIT";
} gps;

bool gpsHomeSet=false;
double gpsHomeLat=0, gpsHomeLon=0;

struct {
  bool ok=false;
  int16_t rawX=0, rawY=0, rawZ=0;
  float headingDeg=0;
  unsigned long lastMs=0;
  uint8_t failCnt=0;
  char state[18]="NOT FOUND";
} compass;

uint8_t qmcAddr=QMC5883_ADDR;
int16_t qmcMinX=32767, qmcMinY=32767, qmcMaxX=-32768, qmcMaxY=-32768;
float qmcOffX=0, qmcOffY=0;
bool qmcCalibrated=false, qmcHeadingReady=false;
uint8_t qmcHeadingJumpCnt=0, qmcFieldJumpCnt=0;
float qmcLastMagXY=0;

struct {
  bool ok=false, filterReady=false, groundReady=false;
  float altM=0, rawM=0, filteredM=0, groundM=0;
  float rawBuf[ALT_FILTER_SAMPLES]={0};
  uint8_t bufCnt=0, bufIdx=0, quality=0, flowQuality=0;
  uint32_t bytes=0, frames=0, rangeFrames=0, flowFrames=0, badFrames=0;
  unsigned long lastMs=0, flowLastMs=0;
  int32_t lastDistMm=0;
  uint16_t lastPayloadLen=0;
  char state[24]="NO DATA";
} alt;

struct {
  uint8_t imu=0, gps=0, compass=0, mtf=0, flow=0;
  bool accTrusted=false, gpsTrusted=false, compassTrusted=false, mtfTrusted=false, flowTrusted=false;
  float accNorm=0, vibScore=100;
} scores;

struct SharedData {
  float rollOut=0, pitchOut=0, yawOut=0, yawRate=0;
  float gx=0, gy=0;
  float pidR=0, pidP=0, pidY=0, pidAlt=0;
  float tgtRateR=0, tgtRateP=0, tgtRateY=0, tgtAngleR=0, tgtAngleP=0, tgtAngleY=0, tgtAlt=0;
  float flowX=0, flowY=0, flowPosX=0, flowPosY=0;
  double gpsLat=0, gpsLon=0; float gpsAlt=0, gpsSpeed=0, gpsCourse=0, gpsHdop=99.9f;
  float heading=0, altM=0, altRaw=0, altFilt=0, altGround=0, altVel=0, altVelRaw=0, tgtAltVel=0;
  float batV=0, batPct=0, mpuT=0, espT=0, yawCompareErr=0;
  float accNorm=0, vibScore=100, compassHead=0;
  uint8_t gpsSat=0, altQuality=0, flowQuality=0;
  uint8_t imuScore=0, gpsScore=0, compassScore=0, mtfScore=0, flowScore=0;
  uint8_t wifiSig=0, sbusSig=0;
  int16_t magX=0, magY=0, magZ=0;
  int32_t altDistMm=0; uint16_t altPayloadLen=0;
  int CH[7]={0,1500,1500,1000,1500,1000,1000};
  int esc[4]={1000,1000,1000,1000};
  bool armed=false, pidOn=false, altHold=false;
  bool imuOk=false, sbusOk=false, gpsOk=false, gpsFix=false, compassOk=false, altOk=false;
  bool failsafe=false, safety=false, lowBat=false, critBat=false, autoLand=false;
  bool accTrusted=false, gpsTrusted=false, compassTrusted=false, mtfTrusted=false, flowTrusted=false;
  bool yawCompareOk=false;
  bool altVelOk=true;
  bool gpsHomeSet=false; double gpsHomeLat=0, gpsHomeLon=0;
  uint32_t gpsPollCnt=0, altBytes=0, altFrames=0, altRangeF=0, altFlowF=0, altBadF=0;
  uint16_t altVelSpikes=0;
  unsigned long gpsLastMs=0, altLastMs=0, flowLastMs=0;
  char gpsState[22]="INIT", compassState[18]="NOT FOUND", altState[24]="NO DATA";
  char altSrc[14]="NONE", yawSrc[14]="GYRO";
  float nF=DEFAULT_NOTCH_HZ, nB=DEFAULT_NOTCH_BW, rOff=DEFAULT_ROLL_OFFSET, pOff=DEFAULT_PITCH_OFFSET;
  float fAng=0.0f;
} shared;

void copyStr(char* dst, size_t len, const char* src) {
  if(!dst||!len) return;
  strncpy(dst, src?src:"", len-1); dst[len-1]=0;
}

float wrapDeg180(float d) { while(d>180) d-=360; while(d<-180) d+=360; return d; }
float wrapDeg360(float d) { while(d<0) d+=360; while(d>=360) d-=360; return d; }
float angleDiff180(float tgt, float cur) { return wrapDeg180(tgt-cur); }

uint8_t qualityScore(float val, float good, float bad) {
  if(val<=good) return 100; if(val>=bad) return 0;
  return (uint8_t)constrain(100.0f-((val-good)*100.0f/(bad-good)), 0.0f, 100.0f);
}

void taiCauHinh() {
  nvs.begin("drone_v30", false);
  uint32_t ver = nvs.getUInt("cfgVer", 0);
  if(ver == 46 && nvs.getUInt("restorePre46", 0) < 1) {
    nvs.putFloat("rpKp", 0.95f); nvs.putFloat("rpKi", 0.11f); nvs.putFloat("rpKd", 0.01f);
    nvs.putFloat("rrKp", 0.95f); nvs.putFloat("rrKi", 0.11f); nvs.putFloat("rrKd", 0.01f);
    nvs.putFloat("prKp", 0.95f); nvs.putFloat("prKi", 0.11f); nvs.putFloat("prKd", 0.01f);
    nvs.putFloat("aKp", 6.5f); nvs.putFloat("arKp", 6.5f); nvs.putFloat("apKp", 6.5f);
    nvs.putFloat("arKi", 0.0f); nvs.putFloat("apKi", 0.0f); nvs.putFloat("arKd", 0.0f); nvs.putFloat("apKd", 0.0f);
    nvs.putFloat("yKp", 6.0f); nvs.putFloat("yKi", 1.2f); nvs.putFloat("yKd", 0.08f);
    nvs.putFloat("yhKp", 0.0f); nvs.putFloat("yhKi", 0.0f); nvs.putFloat("yhKd", 0.0f);
    nvs.putFloat("aKp2", 120.0f); nvs.putFloat("aKi2", 35.0f); nvs.putFloat("aKd2", 0.0f);
    nvs.putFloat("optKp", 0.27f); nvs.putFloat("optKi", 0.42f); nvs.putFloat("optKd", 0.003f);
    nvs.putUInt("cfgVer", CONFIG_VERSION);
    nvs.putUInt("angleCfgVer", 1);
    nvs.putUInt("flowCfgVer", 1);
    nvs.putUInt("yawCfgVer", 3);
    nvs.putUInt("restorePre46", 1);
    ver = CONFIG_VERSION;
  }
  if(ver < CONFIG_VERSION) {
    nvs.putFloat("rpKp", DEFAULT_RP_KP); nvs.putFloat("rpKi", DEFAULT_RP_KI); nvs.putFloat("rpKd", DEFAULT_RP_KD);
    nvs.putFloat("aKp", DEFAULT_ANGLE_KP); nvs.putFloat("yKp", DEFAULT_YAW_KP);
    nvs.putFloat("yKi", DEFAULT_YAW_KI); nvs.putFloat("yKd", DEFAULT_YAW_KD);
    nvs.putFloat("aKp2", DEFAULT_ALT_VZ_KP); nvs.putFloat("aKi2", DEFAULT_ALT_VZ_KI); nvs.putFloat("aKd2", DEFAULT_ALT_VZ_KD);
    nvs.putFloat("nF", DEFAULT_NOTCH_HZ); nvs.putFloat("nB", DEFAULT_NOTCH_BW);
    nvs.putFloat("rOff", DEFAULT_ROLL_OFFSET); nvs.putFloat("pOff", DEFAULT_PITCH_OFFSET);
    nvs.putUInt("cfgVer", CONFIG_VERSION);
  }
  if(nvs.getUInt("altCfgVer", 0) < 1) {
    nvs.putFloat("aKp2", DEFAULT_ALT_VZ_KP);
    nvs.putFloat("aKi2", DEFAULT_ALT_VZ_KI);
    nvs.putFloat("aKd2", DEFAULT_ALT_VZ_KD);
    nvs.putUInt("altCfgVer", 1);
  }
  if(nvs.getUInt("angleCfgVer", 0) < 1) {
    nvs.putFloat("aKp", DEFAULT_ANGLE_KP);
    nvs.putFloat("arKp", DEFAULT_ANGLE_KP);
    nvs.putFloat("apKp", DEFAULT_ANGLE_KP);
    nvs.putFloat("arKi", DEFAULT_ANGLE_KI);
    nvs.putFloat("apKi", DEFAULT_ANGLE_KI);
    nvs.putFloat("arKd", DEFAULT_ANGLE_KD);
    nvs.putFloat("apKd", DEFAULT_ANGLE_KD);
    nvs.putUInt("angleCfgVer", 1);
  }
  if(nvs.getUInt("flowCfgVer", 0) < 2) {
    nvs.putFloat("optKp", DEFAULT_OPT_KP);
    nvs.putFloat("optKi", DEFAULT_OPT_KI);
    nvs.putFloat("optKd", DEFAULT_OPT_KD);
    nvs.putUInt("flowCfgVer", 2);
  }
  if(nvs.getUInt("flowAxisCfgVer", 0) < 2) {
    nvs.putFloat("fAng", 0.0f);
    nvs.putUInt("flowAxisCfgVer", 2);
  }
  if(nvs.getUInt("yawCfgVer", 0) < 3) {
    nvs.putFloat("yKp", DEFAULT_YAW_KP);
    nvs.putFloat("yKi", DEFAULT_YAW_KI);
    nvs.putFloat("yKd", DEFAULT_YAW_KD);
    nvs.putFloat("yhKp", DEFAULT_YAW_HOLD_KP);
    nvs.putFloat("yhKi", DEFAULT_YAW_HOLD_KI);
    nvs.putFloat("yhKd", DEFAULT_YAW_HOLD_KD);
    nvs.putUInt("yawCfgVer", 3);
  }
  float rpKp=nvs.getFloat("rpKp",DEFAULT_RP_KP), rpKi=nvs.getFloat("rpKi",DEFAULT_RP_KI), rpKd=nvs.getFloat("rpKd",DEFAULT_RP_KD);
  pidRateR.kp=nvs.getFloat("rrKp",rpKp); pidRateR.ki=nvs.getFloat("rrKi",rpKi); pidRateR.kd=nvs.getFloat("rrKd",rpKd);
  pidRateP.kp=nvs.getFloat("prKp",rpKp); pidRateP.ki=nvs.getFloat("prKi",rpKi); pidRateP.kd=nvs.getFloat("prKd",rpKd);
  float aKp=nvs.getFloat("aKp",DEFAULT_ANGLE_KP);
  pidAngleR.kp=nvs.getFloat("arKp",aKp); pidAngleR.ki=nvs.getFloat("arKi",DEFAULT_ANGLE_KI); pidAngleR.kd=nvs.getFloat("arKd",DEFAULT_ANGLE_KD);
  pidAngleP.kp=nvs.getFloat("apKp",aKp); pidAngleP.ki=nvs.getFloat("apKi",DEFAULT_ANGLE_KI); pidAngleP.kd=nvs.getFloat("apKd",DEFAULT_ANGLE_KD);
  pidAngleY.kp=nvs.getFloat("yhKp",DEFAULT_YAW_HOLD_KP); pidAngleY.ki=nvs.getFloat("yhKi",DEFAULT_YAW_HOLD_KI); pidAngleY.kd=nvs.getFloat("yhKd",DEFAULT_YAW_HOLD_KD);
  pidYaw.kp=nvs.getFloat("yKp",DEFAULT_YAW_KP); pidYaw.ki=nvs.getFloat("yKi",DEFAULT_YAW_KI); pidYaw.kd=nvs.getFloat("yKd",DEFAULT_YAW_KD);
  pidAlt.kp=nvs.getFloat("aKp2",DEFAULT_ALT_VZ_KP); pidAlt.ki=nvs.getFloat("aKi2",DEFAULT_ALT_VZ_KI); pidAlt.kd=nvs.getFloat("aKd2",DEFAULT_ALT_VZ_KD);
  if(pidAlt.kp==0.0f && pidAlt.ki==0.0f && pidAlt.kd==0.0f) {
    pidAlt.kp=DEFAULT_ALT_VZ_KP; pidAlt.ki=DEFAULT_ALT_VZ_KI; pidAlt.kd=DEFAULT_ALT_VZ_KD;
  }
  pidFlowX.kp=pidFlowY.kp=nvs.getFloat("optKp",DEFAULT_OPT_KP);
  pidFlowX.ki=pidFlowY.ki=nvs.getFloat("optKi",DEFAULT_OPT_KI);
  pidFlowX.kd=pidFlowY.kd=nvs.getFloat("optKd",DEFAULT_OPT_KD);
  heSoPin=nvs.getFloat("hSPin",5.0f);
  flowAngleDeg=nvs.getFloat("fAng",0.0f);
  notchFreq=constrain(nvs.getFloat("nF",DEFAULT_NOTCH_HZ),20.0f,110.0f);
  notchBw=constrain(nvs.getFloat("nB",DEFAULT_NOTCH_BW),5.0f,50.0f);
  rollOffset=constrain(nvs.getFloat("rOff",DEFAULT_ROLL_OFFSET),-30.0f,30.0f);
  pitchOffset=constrain(nvs.getFloat("pOff",DEFAULT_PITCH_OFFSET),-30.0f,30.0f);
}

void luuCauHinh() {
  nvs.putFloat("rpKp",(pidRateR.kp+pidRateP.kp)*0.5f);
  nvs.putFloat("rpKi",(pidRateR.ki+pidRateP.ki)*0.5f);
  nvs.putFloat("rpKd",(pidRateR.kd+pidRateP.kd)*0.5f);
  nvs.putFloat("rrKp",pidRateR.kp); nvs.putFloat("rrKi",pidRateR.ki); nvs.putFloat("rrKd",pidRateR.kd);
  nvs.putFloat("prKp",pidRateP.kp); nvs.putFloat("prKi",pidRateP.ki); nvs.putFloat("prKd",pidRateP.kd);
  nvs.putFloat("aKp",(pidAngleR.kp+pidAngleP.kp)*0.5f);
  nvs.putFloat("arKp",pidAngleR.kp); nvs.putFloat("arKi",pidAngleR.ki); nvs.putFloat("arKd",pidAngleR.kd);
  nvs.putFloat("apKp",pidAngleP.kp); nvs.putFloat("apKi",pidAngleP.ki); nvs.putFloat("apKd",pidAngleP.kd);
  nvs.putFloat("yhKp",pidAngleY.kp); nvs.putFloat("yhKi",pidAngleY.ki); nvs.putFloat("yhKd",pidAngleY.kd);
  nvs.putFloat("yKp",pidYaw.kp); nvs.putFloat("yKi",pidYaw.ki); nvs.putFloat("yKd",pidYaw.kd);
  nvs.putFloat("aKp2",pidAlt.kp); nvs.putFloat("aKi2",pidAlt.ki); nvs.putFloat("aKd2",pidAlt.kd);
  nvs.putFloat("optKp",pidFlowX.kp); nvs.putFloat("optKi",pidFlowX.ki); nvs.putFloat("optKd",pidFlowX.kd);
  nvs.putFloat("hSPin",heSoPin); nvs.putFloat("nF",notchFreq); nvs.putFloat("nB",notchBw);
  nvs.putFloat("fAng",flowAngleDeg);
  nvs.putFloat("rOff",rollOffset); nvs.putFloat("pOff",pitchOffset);
  nvs.putUInt("cfgVer", CONFIG_VERSION);
}

void resetAllPID() {
  pidRateR.reset(); pidRateP.reset(); pidYaw.reset();
  pidAngleR.reset(); pidAngleP.reset(); pidAngleY.reset();
  pidAlt.reset(); pidFlowX.reset(); pidFlowY.reset();
}

void initFilters() {
  float fs = 1000.0f/NHIP_FLIGHT_MS;
  notchR.init(notchFreq, notchBw, fs);
  notchP.init(notchFreq, notchBw, fs);
  notchY.init(notchFreq, notchBw, fs);
  gyroLpfR.init(GYRO_LPF2_CUTOFF_HZ, fs);
  gyroLpfP.init(GYRO_LPF2_CUTOFF_HZ, fs);
  gyroLpfY.init(GYRO_LPF2_CUTOFF_HZ, fs);
  
  pidRateR.setLimits(PID_OUT_LIMIT, PID_INT_LIMIT, DTERM_LPF_HZ);
  pidRateP.setLimits(PID_OUT_LIMIT, PID_INT_LIMIT, DTERM_LPF_HZ);
  pidYaw.setLimits(PID_OUT_LIMIT, PID_INT_LIMIT, DTERM_LPF_HZ);
  pidAngleR.setLimits(MAX_RATE_DPS, ANGLE_PID_INT_LIMIT, ANGLE_DTERM_LPF_HZ);
  pidAngleP.setLimits(MAX_RATE_DPS, ANGLE_PID_INT_LIMIT, ANGLE_DTERM_LPF_HZ);
  pidAngleY.setLimits(MAX_YAW_RATE_DPS, ANGLE_PID_INT_LIMIT, ANGLE_DTERM_LPF_HZ);
  pidAlt.setLimits(ALT_HOLD_BU_LIMIT, ALT_PID_INT_LIMIT, ALT_DTERM_LPF_HZ);
  pidFlowX.setLimits(FLOW_PID_OUT_LIMIT_DEG, FLOW_PID_INT_LIMIT, FLOW_DTERM_LPF_HZ);
  pidFlowY.setLimits(FLOW_PID_OUT_LIMIT_DEG, FLOW_PID_INT_LIMIT, FLOW_DTERM_LPF_HZ);
  
  kfCompass.init(0.8f, 1.5f);
  kfGpsLat.init(0.01f, 0.05f);
  kfGpsLon.init(0.01f, 0.05f);
  kfFlowX.init(50.0f, 80.0f);
  kfFlowY.init(50.0f, 80.0f);
}

void docIMU() {
  if(!sys.imuOk) return;
  mpu.update();
  MPU6500_Data d = mpu.getData();
  mpuTemp = d.temp;
  scores.accNorm = d.accNorm;
  scores.vibScore = d.vibrationScore;
  scores.accTrusted = d.accTrusted;
  scores.imu = qualityScore(d.vibrationScore, 15.0f, 85.0f);
  if(!d.accTrusted && scores.imu>70) scores.imu=70;

#if IMU_SWAP_RP
  float imuRollA=d.angle.pitch, imuPitchA=d.angle.roll, imuRollR=d.gy, imuPitchR=d.gx;
#else
  float imuRollA=d.angle.roll, imuPitchA=d.angle.pitch, imuRollR=d.gx, imuPitchR=d.gy;
#endif
  gocRoll  = IMU_ROLL_SIGN*imuRollA + rollOffset;
  gocPitch = IMU_PITCH_SIGN*imuPitchA + pitchOffset;
  float yawGyro = IMU_YAW_SIGN*d.angle.yaw;
  yawGyroRaw = yawGyro;
  if(scores.compassTrusted) {
    float h180 = compass.headingDeg - compassOffset;
    h180 = wrapDeg360(h180);
    if(h180>180) h180-=360;
    if(!yawFusionReady || !sys.armed) {
      yawCompassOff = angleDiff180(h180, yawGyro);
      yawFusionReady = true;
      yawCompareOk = true;
      yawCompareBadCnt = 0;
    }
    float curYaw = wrapDeg180(yawGyro + yawCompassOff);
    float errY = angleDiff180(h180, curYaw);
    yawCompareErrDeg = errY;
    float errAbs = fabsf(errY);
    bool vibHigh = scores.vibScore > YAW_VIB_REJECT_SCORE;
    bool yawFastCorr = false;

    if(sys.armed) {
      if(errAbs > QMC_MPU_YAW_REJECT_DEG) {
        if(++yawCompareBadCnt >= QMC_MPU_YAW_BAD_FRAMES) {
          yawFastCorr = (mucGa <= GA_HOAT_DONG_MIN + 30);
          yawCompareOk = true;
        }
      } else {
        yawCompareBadCnt = 0;
        yawCompareOk = true;
      }
    } else {
      yawCompareBadCnt = 0;
      yawCompareOk = true;
    }

    float corrAlpha = yawFastCorr ? QMC_YAW_FAST_CORR_ALPHA : QMC_YAW_CORR_ALPHA;
    float corrStep = yawFastCorr ? QMC_YAW_FAST_MAX_STEP : QMC_YAW_CORR_MAX_STEP;
    if(vibHigh && !yawFastCorr) corrStep *= 0.5f;
    yawCompassOff += constrain(errY*corrAlpha, -corrStep, corrStep);
    copyStr(shared.yawSrc, sizeof(shared.yawSrc), yawFastCorr ? "GYRO+QMC FAST" : "GYRO+QMC");
  } else {
    yawCompareOk = false;
    yawCompareBadCnt = 0;
    copyStr(shared.yawSrc, sizeof(shared.yawSrc), "GYRO");
  }
  gocYaw = wrapDeg180(yawGyro + yawCompassOff);

  
  gx_raw = IMU_GX_SIGN*imuRollR;
  gy_raw = IMU_GY_SIGN*imuPitchR;
  gz_raw = IMU_GZ_SIGN*d.gz;
  gx_filt  = gyroLpfR.apply(notchR.apply(gx_raw));
  gy_filt  = gyroLpfP.apply(notchP.apply(gy_raw));
  yawRate  = gyroLpfY.apply(notchY.apply(gz_raw));

  rollOut=gocRoll; pitchOut=gocPitch; yawOut=gocYaw;
}

unsigned long tSbusPrev=0;

bool sbusInCenterBand(int v) {
  return v>=SBUS_CENTER_MIN && v<=SBUS_CENTER_MAX;
}

bool sbusLooksLikeReceiverFailsafe() {
  return CH[3]<=SBUS_LOW_MAX &&
         sbusInCenterBand(CH[1]) &&
         sbusInCenterBand(CH[2]) &&
         sbusInCenterBand(CH[4]) &&
         sbusInCenterBand(CH[5]) &&
         sbusInCenterBand(CH[6]);
}

void docSBUS() {
  static uint8_t buf[25], idx=0;
  while(Serial1.available()) {
    uint8_t b=Serial1.read();
    if(idx==0 && b!=0x0F) continue;
    buf[idx++]=b;
    if(idx>=25) {
      idx=0; if(buf[0]!=0x0F) continue;
      uint16_t r[6];
      r[0]=(buf[1]|buf[2]<<8)&0x7FF;       r[1]=(buf[2]>>3|buf[3]<<5)&0x7FF;
      r[2]=(buf[3]>>6|buf[4]<<2|buf[5]<<10)&0x7FF; r[3]=(buf[5]>>1|buf[6]<<7)&0x7FF;
      r[4]=(buf[6]>>4|buf[7]<<4)&0x7FF;     r[5]=(buf[7]>>7|buf[8]<<1|buf[9]<<9)&0x7FF;
      for(int i=0;i<6;i++) CH[i+1]=constrain(map(r[i],172,1811,1000,2000),1000,2000);
      bool lostFrame = (buf[23]>>2)&1;
      bool frameFailsafe = (buf[23]>>3)&1;
      bool rxFailsafePattern = sbusLooksLikeReceiverFailsafe();
      sys.failsafe = lostFrame || frameFailsafe || rxFailsafePattern;
      sys.sbusOk = !sys.failsafe;
      tSbusPrev=millis();
      sbusSignal = sys.sbusOk ? 100 : 0;
    }
  }
  if(!tSbusPrev) { sys.sbusOk=false; sys.failsafe=true; sbusSignal=0; return; }
  unsigned long age = millis()-tSbusPrev;
  if(age>SBUS_TIMEOUT_MS) { sys.sbusOk=false; sys.failsafe=true; sbusSignal=0; }
  else if(sys.sbusOk && !sys.failsafe) {
    sbusSignal = (age<80) ? 100 : (uint8_t)constrain(map((long)age,80L,(long)SBUS_TIMEOUT_MS,100L,10L),0L,100L);
  } else sbusSignal=0;
}

bool gpsInitDone = false;
uint32_t gpsCurrentBaud = 0;
const uint32_t GPS_RAW_BAUDS[] = {GPS_BAUD_TARGET, GPS_BAUD_INIT, 9600UL};
uint8_t gpsRawBaudIdx = 0;
bool gpsRawPosValid(double lat, double lon);
bool gpsHasValidPosition();

bool gpsBeginAt(uint32_t baud) {
  GpsSerial.begin(baud, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX, false);
  delay(150);
  if(myGPS.begin(GpsSerial)) {
    gpsCurrentBaud = baud;
    return true;
  }
  return false;
}

void gpsApplyConfig() {
  myGPS.setUART1Output(COM_TYPE_UBX);
  myGPS.setNavigationFrequency(10);
  myGPS.setAutoPVT(true);
  myGPS.setDynamicModel(DYN_MODEL_AIRBORNE2g);
}

double parseNmeaCoord(const char* val, const char* hemi) {
  if(!val || !*val || !hemi || !*hemi) return 0;
  double raw = atof(val);
  int deg = (int)(raw / 100.0);
  double minutes = raw - (double)deg * 100.0;
  double out = (double)deg + minutes / 60.0;
  if(hemi[0]=='S' || hemi[0]=='W') out = -out;
  return out;
}

bool nmeaChecksumOk(const char* line) {
  if(!line || line[0]!='$') return false;
  const char* star = strchr(line, '*');
  if(!star) return true;
  uint8_t sum = 0;
  for(const char* p=line+1; p<star; p++) sum ^= (uint8_t)*p;
  uint8_t got = (uint8_t)strtoul(star+1, NULL, 16);
  return sum == got;
}

void gpsAcceptRawPosition(double newLat, double newLon, float newAlt, uint8_t sats, float hdop, bool fix, const char* state) {
  gps.sats = sats;
  gps.hdop = hdop > 0 ? hdop : gps.hdop;
  gps.fix = fix;
  gps.ok = true;
  if(fix && gpsRawPosValid(newLat, newLon)) {
    gps.lat = newLat;
    gps.lon = newLon;
    gps.altM = newAlt;
    gps.filterReady = true;
  }
  copyStr(gps.state, sizeof(gps.state), state);
}

void gpsParseNmeaLine(char* line) {
  if(!line || !nmeaChecksumOk(line)) return;
  char* star = strchr(line, '*');
  if(star) *star = 0;
  char* f[20] = {0};
  uint8_t n = 0;
  for(char* tok = strtok(line, ","); tok && n < 20; tok = strtok(NULL, ",")) f[n++] = tok;
  if(n < 1 || !f[0]) return;

  size_t typeLen = strlen(f[0]);
  const char* type = typeLen >= 3 ? f[0] + typeLen - 3 : f[0];
  if(strcmp(type, "GGA") == 0 && n >= 10) {
    int fixQ = atoi(f[6]);
    uint8_t sats = (uint8_t)constrain(atoi(f[7]), 0, 99);
    float hdop = atof(f[8]);
    double lat = parseNmeaCoord(f[2], f[3]);
    double lon = parseNmeaCoord(f[4], f[5]);
    float altM = atof(f[9]);
    bool fix = fixQ > 0;
    gpsAcceptRawPosition(lat, lon, altM, sats, hdop, fix, fix ? "NMEA FIX" : "NMEA NO FIX");
  } else if(strcmp(type, "RMC") == 0 && n >= 9) {
    bool fix = f[2] && f[2][0] == 'A';
    double lat = parseNmeaCoord(f[3], f[4]);
    double lon = parseNmeaCoord(f[5], f[6]);
    gps.speedKmh = atof(f[7]) * 1.852f;
    gps.courseDeg = atof(f[8]);
    gpsAcceptRawPosition(lat, lon, gps.altM, gps.sats, gps.hdop, fix, fix ? "NMEA FIX" : "NMEA NO FIX");
  }
}

uint32_t rdU4(const uint8_t* p) {
  return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
}

int32_t rdI4(const uint8_t* p) {
  return (int32_t)rdU4(p);
}

void gpsParseUbxPvt(const uint8_t* p, uint16_t len) {
  if(!p || len < 92) return;
  uint8_t fixType = p[20];
  uint8_t flags = p[21];
  uint8_t sats = p[23];
  double lon = (double)rdI4(p+24) * 1e-7;
  double lat = (double)rdI4(p+28) * 1e-7;
  float altM = (float)rdI4(p+36) / 1000.0f;
  float gSpeedKmh = (float)rdI4(p+60) * 0.0036f;
  float headDeg = (float)rdI4(p+64) * 1e-5f;
  float pdop = (float)((uint16_t)p[76] | ((uint16_t)p[77]<<8)) / 100.0f;
  bool fix = (fixType >= 2) && (flags & 0x01);
  gps.sats = sats;
  gps.hdop = pdop > 0 ? pdop : gps.hdop;
  gps.speedKmh = gSpeedKmh;
  gps.courseDeg = headDeg;
  gps.fix = fix;
  gps.ok = true;
  gps.lastMs = millis();
  if(fix && gpsRawPosValid(lat, lon)) {
    gps.lat = lat;
    gps.lon = lon;
    gps.altM = altM;
    gps.filterReady = true;
    copyStr(gps.state, sizeof(gps.state), gpsFixOk() ? "UBX FIX" : "UBX WEAK");
  } else {
    if(!gpsHasValidPosition()) { gps.lat=0; gps.lon=0; gps.altM=0; }
    copyStr(gps.state, sizeof(gps.state), fix ? "UBX NO POS" : "UBX NO FIX");
  }
}

void gpsParseUbxByte(uint8_t c) {
  static uint8_t st=0, cls=0, id=0, ckA=0, ckB=0;
  static uint16_t len=0, idx=0;
  static uint8_t payload[100];
  auto ck=[&](uint8_t b){ ckA += b; ckB += ckA; };
  switch(st) {
    case 0: st = (c==0xB5) ? 1 : 0; break;
    case 1: st = (c==0x62) ? 2 : 0; break;
    case 2: cls=c; ckA=ckB=0; ck(c); st=3; break;
    case 3: id=c; ck(c); st=4; break;
    case 4: len=c; ck(c); st=5; break;
    case 5: len |= ((uint16_t)c<<8); ck(c); idx=0; st = (len<=sizeof(payload)) ? 6 : 0; break;
    case 6:
      payload[idx++] = c; ck(c);
      if(idx >= len) st=7;
      break;
    case 7:
      if(c == ckA) st=8;
      else st=0;
      break;
    case 8:
      if(c == ckB && cls==0x01 && id==0x07) gpsParseUbxPvt(payload, len);
      st=0;
      break;
  }
}

void gpsPollUbxPvt() {
  static const uint8_t pollPvt[] = {0xB5,0x62,0x01,0x07,0x00,0x00,0x08,0x19};
  GpsSerial.write(pollPvt, sizeof(pollPvt));
}

void gpsStartRawMode(uint8_t idx, const char* state) {
  gpsRawBaudIdx = idx % (sizeof(GPS_RAW_BAUDS)/sizeof(GPS_RAW_BAUDS[0]));
  gpsCurrentBaud = GPS_RAW_BAUDS[gpsRawBaudIdx];
  GpsSerial.begin(gpsCurrentBaud, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX, false);
  gps.rawMode = true;
  gps.ok = false;
  gps.fix = false;
  gps.filterReady = false;
  gpsInitDone = true;
  gps.rawBytes = 0;
  gps.pollCount = 0;
  gps.lastByteMs = 0;
  gps.lastMs = 0;
  copyStr(gps.state, sizeof(gps.state), state);
}

void gpsReadRawNmea() {
  static char line[128];
  static uint8_t idx = 0;
  while(GpsSerial.available()) {
    uint8_t ub = (uint8_t)GpsSerial.read();
    char c = (char)ub;
    gps.rawBytes++;
    gps.pollCount = gps.rawBytes;
    gps.lastByteMs = millis();
    gps.ok = true;
    gpsParseUbxByte(ub);
    if(c == '$') {
      idx = 0;
      line[idx++] = c;
    } else if(c == '\n' || c == '\r') {
      if(idx > 6) {
        line[idx] = 0;
        gps.lastMs = millis();
        gpsParseNmeaLine(line);
      }
      idx = 0;
    } else if(idx > 0 && idx < sizeof(line)-1) {
      line[idx++] = c;
    } else if(idx >= sizeof(line)-1) {
      idx = 0;
    }
  }
  if(gps.lastByteMs && millis()-gps.lastByteMs < GPS_TIMEOUT_MS && !gps.fix && !gpsHasValidPosition()) {
    gps.ok = true;
    copyStr(gps.state, sizeof(gps.state), "RAW DATA");
  }
}

void khoiTaoGPS() {
#if GPS_FORCE_NMEA
  gpsStartRawMode(0, "RAW WAIT");
  return;
#endif
  bool connected = false;
  
  
  
  connected = gpsBeginAt(GPS_BAUD_TARGET);
  delay(100);
  if(myGPS.begin(GpsSerial)) {
    gpsCurrentBaud = GPS_BAUD_TARGET;
    connected = true;
  } else {
    
    if(gpsBeginAt(GPS_BAUD_INIT)) {
      connected = true;
    } else {
      
      if(gpsBeginAt(9600)) {
        connected = true;
      }
    }
  }

  if(!connected) {
    gpsStartRawMode(0, "RAW WAIT");
    return;
  }

  
  gpsApplyConfig();
  delay(50);
  myGPS.setSerialRate(GPS_BAUD_TARGET);
  delay(50);
  GpsSerial.updateBaudRate(GPS_BAUD_TARGET);
  delay(100);

  
  if(!myGPS.begin(GpsSerial)) {
    bool rebound = false;
    const uint32_t fallbackBauds[] = {GPS_BAUD_TARGET, GPS_BAUD_INIT, 9600UL};
    for(uint8_t i=0; i<sizeof(fallbackBauds)/sizeof(fallbackBauds[0]); i++) {
      if(gpsBeginAt(fallbackBauds[i])) {
        rebound = true;
        break;
      }
    }
    if(!rebound) {
      gpsStartRawMode(0, "RAW WAIT");
      return;
    }
  }

  gpsApplyConfig();
  myGPS.saveConfiguration();

  gpsInitDone = true;
  gps.ok = false;
  gps.fix = false;
  gps.filterReady = false;
  gps.rawMode = false;
  copyStr(gps.state, sizeof(gps.state), "WAITING FIX");
}

float gpsKhoangCach(double lat1, double lon1, double lat2, double lon2) {
  const double R=6371000.0, d2r=PI/180.0;
  double p1=lat1*d2r, p2=lat2*d2r, dP=(lat2-lat1)*d2r, dL=(lon2-lon1)*d2r;
  double a=sin(dP*0.5)*sin(dP*0.5)+cos(p1)*cos(p2)*sin(dL*0.5)*sin(dL*0.5);
  return (float)(R*2.0*atan2(sqrt(a),sqrt(1.0-a)));
}

bool gpsFixOk() {
  return gps.fix && gps.sats>=GPS_MIN_SATS && gps.hdop>0 && gps.hdop<=GPS_MAX_HDOP;
}

bool gpsRawPosValid(double lat, double lon) {
  return fabs(lat)>0.001 && fabs(lon)>0.001 &&
         lat>=-90 && lat<=90 && lon>=-180 && lon<=180;
}

bool gpsHasValidPosition() {
  return gpsRawPosValid(gps.lat, gps.lon);
}

float gpsAlpha() {
  float hs = constrain((GPS_MAX_HDOP-gps.hdop)/(GPS_MAX_HDOP-0.7f),0,1);
  float ss = constrain(((float)gps.sats-GPS_MIN_SATS)/6.0f,0,1);
  return GPS_POS_LPF_ALPHA_MIN + (GPS_POS_LPF_ALPHA_MAX-GPS_POS_LPF_ALPHA_MIN)*(0.55f*hs+0.45f*ss);
}

void docGPS() {
  if(!gpsInitDone) return;
  if(gps.rawMode) {
    static unsigned long tPollPrev = 0;
    if(millis() - tPollPrev >= 200) {
      tPollPrev = millis();
      gpsPollUbxPvt();
    }
    gpsReadRawNmea();
    if(gps.lastByteMs && (millis()-gps.lastByteMs > GPS_TIMEOUT_MS)) {
      gps.ok=false; gps.fix=false; gps.speedKmh=0;
      copyStr(gps.state, sizeof(gps.state), "RAW TIMEOUT");
    }
    if(!gps.lastByteMs && millis() - tPollPrev > GPS_TIMEOUT_MS) {
      gpsStartRawMode(gpsRawBaudIdx + 1, "RAW SCAN");
      tPollPrev = millis();
    }
    sys.gpsOk = gps.ok;
    scores.gpsTrusted = gps.ok && gps.fix && gpsFixOk() && gpsHasValidPosition();
    if(!gps.ok) scores.gps=0;
    else if(!gps.fix) scores.gps=20;
    else {
      float hs=constrain((GPS_MAX_HDOP-gps.hdop)/(GPS_MAX_HDOP-0.7f),0,1);
      float ss=constrain(((float)gps.sats-GPS_MIN_SATS)/8.0f,0,1);
      scores.gps = (uint8_t)constrain(55+30*hs+15*ss, 0, 100);
      if(!scores.gpsTrusted && scores.gps>65) scores.gps=65;
    }
    return;
  } 
  if(myGPS.getPVT()) {
    gps.pollCount++;
    gps.lastMs = millis();
    gps.ok = true;

    uint8_t fixType = myGPS.getFixType(); 
    gps.fix = (fixType >= 2);
    gps.sats = myGPS.getSIV();
    gps.hdop = myGPS.getPDOP() / 100.0f; 
    float newSpd = myGPS.getGroundSpeed() / 1000.0f * 3.6f; 
    gps.courseDeg = myGPS.getHeading() / 100000.0f; 
    gps.speedKmh = newSpd;

    double newLat = myGPS.getLatitude() / 1e7;
    double newLon = myGPS.getLongitude() / 1e7;
    float newAlt = myGPS.getAltitudeMSL() / 1000.0f; 
    bool posValid = gpsRawPosValid(newLat, newLon);

    if(gps.fix && gpsFixOk() && posValid) {
      
      
      if(gps.filterReady && gps.lastMs) {
        float maxJump = GPS_POS_JUMP_MAX_M + max(0.0f, gps.speedKmh/3.6f)*0.5f;
        if(gpsKhoangCach(gps.lat,gps.lon,newLat,newLon) > maxJump) {
          copyStr(gps.state, sizeof(gps.state), "JUMP REJECT");
          return;
        }
      }

      float a = gpsAlpha();
      if(!gps.filterReady) {
        gps.lat=newLat; gps.lon=newLon; gps.altM=newAlt;
        gps.filterReady=true;
        kfGpsLat.reset(); kfGpsLon.reset();
      } else {
        gps.lat = gps.lat*(1-a) + newLat*a;
        gps.lon = gps.lon*(1-a) + newLon*a;
        gps.altM = gps.altM*(1-GPS_ALT_LPF_ALPHA) + newAlt*GPS_ALT_LPF_ALPHA;
        gps.lat = (double)kfGpsLat.update((float)gps.lat);
        gps.lon = (double)kfGpsLon.update((float)gps.lon);
      }
      gps.ok = true;
      copyStr(gps.state, sizeof(gps.state), "FIX OK");
    } else if(gps.fix && posValid) {
      gps.lat = newLat;
      gps.lon = newLon;
      gps.altM = newAlt;
      gps.ok = true;
      copyStr(gps.state, sizeof(gps.state), "FIX WEAK");
    } else if(gps.fix) {
      gps.ok = true;
      if(!gpsHasValidPosition()) { gps.lat=0; gps.lon=0; gps.altM=0; }
      copyStr(gps.state, sizeof(gps.state), "FIX NO POS");
    } else {
      copyStr(gps.state, sizeof(gps.state), "NO FIX");
    }
  }
  
  if(gps.lastMs && (millis()-gps.lastMs > GPS_TIMEOUT_MS)) {
    gps.ok=false; gps.fix=false; gps.speedKmh=0;
    copyStr(gps.state, sizeof(gps.state), "TIMEOUT");
  }

  sys.gpsOk = gps.ok;
  
  
  scores.gpsTrusted = gps.ok && gps.fix && gpsFixOk() && gps.filterReady;
  if(!gps.ok) scores.gps=0;
  else if(!gps.fix) scores.gps=20;
  else {
    float hs=constrain((GPS_MAX_HDOP-gps.hdop)/(GPS_MAX_HDOP-0.7f),0,1);
    float ss=constrain(((float)gps.sats-GPS_MIN_SATS)/8.0f,0,1);
    scores.gps = (uint8_t)constrain(55+30*hs+15*ss, 0, 100);
    if(!scores.gpsTrusted && scores.gps>65) scores.gps=65;
  }
}

bool qmcWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(qmcAddr); Wire.write(reg); Wire.write(val);
  return Wire.endTransmission()==0;
}

void khoiTaoCompass() {
#if ENABLE_COMPASS
  sys.compassOk=false; compass.ok=false;
  copyStr(compass.state, sizeof(compass.state), "NOT FOUND");

  qmcAddr=QMC5883_ADDR;
  Wire.beginTransmission(qmcAddr);
  if(Wire.endTransmission()!=0) {
    qmcAddr=QMC5883_ADDR_ALT;
    Wire.beginTransmission(qmcAddr);
    if(Wire.endTransmission()!=0) return;
  }

  qmcWrite(0x0B, 0x80); delay(100); 
  qmcWrite(0x0B, 0x08); delay(20);  
  qmcWrite(0x0A, 0x0F); delay(100); 

  qmcMinX=qmcMinY=32767; qmcMaxX=qmcMaxY=-32768;
  qmcOffX=qmcOffY=0; qmcCalibrated=false; qmcHeadingReady=false; qmcLastMagXY=0;
  qmcHeadingJumpCnt=0; qmcFieldJumpCnt=0;
  compass.failCnt=0; compass.lastMs=millis();
  compass.ok=true; sys.compassOk=true;
  scores.compass=50;
  copyStr(compass.state, sizeof(compass.state), "OK");
#endif
}

void docCompass() {
#if ENABLE_COMPASS
  if(!sys.compassOk) return;
  unsigned long now=millis();
  if(now-compass.lastMs < QMC_READ_MS) return;
  compass.lastMs=now;

  Wire.beginTransmission(qmcAddr); Wire.write(0x01);
  if(Wire.endTransmission(false)!=0) { if(++compass.failCnt>=5) { compass.ok=false; sys.compassOk=false; copyStr(compass.state,sizeof(compass.state),"READ FAIL"); } scores.compassTrusted=false; scores.compass=0; return; }
  if(Wire.requestFrom(qmcAddr,(uint8_t)6)<6) { if(++compass.failCnt>=5) { compass.ok=false; sys.compassOk=false; } scores.compassTrusted=false; scores.compass=0; return; }

  uint8_t b[6]; for(int i=0;i<6;i++) b[i]=Wire.read();
  int16_t x=(int16_t)(b[1]<<8|b[0]), y=(int16_t)(b[3]<<8|b[2]), z=(int16_t)(b[5]<<8|b[4]);
  compass.rawX=x; compass.rawY=y; compass.rawZ=z;

  
  if(!sys.armed) {
    if(x<qmcMinX) qmcMinX=x; if(y<qmcMinY) qmcMinY=y;
    if(x>qmcMaxX) qmcMaxX=x; if(y>qmcMaxY) qmcMaxY=y;
    float spanX=(float)(qmcMaxX-qmcMinX), spanY=(float)(qmcMaxY-qmcMinY);
    if(spanX>=QMC_CAL_SPAN_MIN && spanY>=QMC_CAL_SPAN_MIN) {
      qmcOffX=((float)qmcMinX+(float)qmcMaxX)*0.5f;
      qmcOffY=((float)qmcMinY+(float)qmcMaxY)*0.5f;
      qmcCalibrated=true;
    }
  }

  float mx=(float)x-(qmcCalibrated?qmcOffX:0), my=(float)y-(qmcCalibrated?qmcOffY:0);
  float magXY=sqrtf(mx*mx+my*my);
  if(magXY<QMC_FIELD_MIN_XY || magXY>QMC_FIELD_MAX_XY) { copyStr(compass.state,sizeof(compass.state),"MAG NOISE"); scores.compassTrusted=false; scores.compass=20; return; }

  float headNew = wrapDeg360(atan2f(my,mx)*RAD_TO_DEG + QMC_MOUNT_OFFSET);
  bool fieldJump = (qmcLastMagXY>1) && (fabsf(magXY-qmcLastMagXY)/qmcLastMagXY > QMC_FIELD_JUMP_RATIO);
  bool headJump = qmcHeadingReady && fabsf(angleDiff180(headNew,compass.headingDeg)) > QMC_HEADING_MAX_STEP;
  if(fieldJump) {
    qmcFieldJumpCnt++;
    qmcHeadingJumpCnt = 0;
    if(qmcFieldJumpCnt < QMC_HEADING_RESYNC_FRAMES) {
      copyStr(compass.state,sizeof(compass.state),"FIELD JUMP");
      scores.compassTrusted=false; scores.compass=35; qmcLastMagXY=magXY; return;
    }
    qmcLastMagXY = magXY;
    qmcFieldJumpCnt = 0;
  } else {
    qmcFieldJumpCnt = 0;
  }
  if(headJump) {
    qmcHeadingJumpCnt++;
    if(qmcHeadingJumpCnt < QMC_HEADING_RESYNC_FRAMES) {
      copyStr(compass.state,sizeof(compass.state),"HEAD JUMP");
      scores.compassTrusted=false; scores.compass=35; qmcLastMagXY=magXY; return;
    }
    compass.headingDeg = headNew;
    qmcHeadingJumpCnt = 0;
    kfCompass.reset();
  } else {
    qmcHeadingJumpCnt = 0;
  }

  if(!qmcHeadingReady) { compass.headingDeg=headNew; qmcHeadingReady=true; kfCompass.reset(); }
  else {
    compass.headingDeg = wrapDeg360(compass.headingDeg + angleDiff180(headNew,compass.headingDeg)*QMC_HEADING_LPF);
    compass.headingDeg = kfCompass.updateAngle(compass.headingDeg, 360.0f);
  }

  compass.failCnt=0; compass.ok=true; sys.compassOk=true; qmcLastMagXY=magXY;
  scores.compass = qmcCalibrated ? 95 : 70;
  scores.compassTrusted = true;
  copyStr(compass.state,sizeof(compass.state), qmcCalibrated?"OK CAL":"OK RAW");
#endif
}


void capNhatAltState() {
  unsigned long now=millis();
  
  alt.frames = mtf.frameCount;
  alt.bytes = mtf.byteCount;
  alt.rangeFrames = mtf.frameCount;
  alt.flowFrames = mtf.frameCount;
  alt.badFrames = mtf.badFrames;
  alt.lastMs = mtf.lastRangeMs;
  alt.flowLastMs = mtf.lastFlowMs;
  alt.lastDistMm = mtf.lastDistanceMm;
  alt.lastPayloadLen = mtf.lastPayloadLen;
  
  if(!mtf.byteCount) { alt.ok=false; copyStr(alt.state,sizeof(alt.state),"NO DATA"); scores.mtf=0; scores.mtfTrusted=false; }
  else if(!mtf.lastRangeMs) { alt.ok=false; scores.mtf=10; scores.mtfTrusted=false; }
  else if(now-mtf.lastRangeMs>2000UL) { alt.ok=false; copyStr(alt.state,sizeof(alt.state),"TIMEOUT"); scores.mtf=25; scores.mtfTrusted=false; }
  else { alt.ok=true; copyStr(alt.state,sizeof(alt.state),"FILTERED OK"); scores.mtf=100; scores.mtfTrusted=true; }

  if(mtf.lastFlowMs && now-mtf.lastFlowMs>2000UL) {
    mtf.flowQuality=0; flowVelX=flowVelY=0;
    scores.flowTrusted=false; scores.flow=0;
  } else if (mtf.lastFlowMs) {
    scores.flowTrusted=true; scores.flow=100;
  }
  sys.altOk = alt.ok;
  copyStr(shared.altSrc,sizeof(shared.altSrc), alt.ok?"MTF":(mtf.lastRangeMs?"MTF_OLD":"NONE"));
}

void datMocMatDat() {
  alt.groundM = alt.filteredM; 
  alt.groundReady=true;
}

void resetFlow() {
  flowVelX = flowVelY = 0;
  flowPosX = flowPosY = 0;
  alt.flowQuality = 0;
  mtf.lastFlowMs = 0;
}

unsigned long tBatPrev=0;

void docPin() {
  uint32_t sum=0;
  for(int i=0;i<32;i++) sum+=analogReadMilliVolts(PIN_BATT);
  float v=(sum/32.0f)/1000.0f*heSoPin;
  batV = (batV<1)?v:(batV*0.85f+v*0.15f);
  batPct = constrain((batV-10.5f)/(12.6f-10.5f)*100.f,0,100);
  espTemp = temperatureRead();
  sys.critBat = batV<=PIN_CAN_V || batV<PIN_DOC_MIN_V;
  sys.lowBat = sys.critBat || batV<=PIN_YEU_V;
}

void ghiTatMotor() { mucGa=ESC_MIN_US; for(int i=0;i<4;i++) dongCo[i].writeMicroseconds(esc[i]=ESC_MIN_US); }
void ghiIdleMotor() { mucGa=MUC_GA_NGHI; for(int i=0;i<4;i++) dongCo[i].writeMicroseconds(esc[i]=MUC_GA_NGHI); }

void resetAltHoldState(int throttleBase) {
  sys.altHold=false;
  altHoldActive=false;
  tgtAlt=alt.altM;
  tgtAltVel=0;
  hoverThrottle=constrain(throttleBase, MUC_GA_NGHI, ESC_MAX_US);
  pidAlt.reset();
}

void resetYawHomeTheoLaBan() {
  pidAngleY.reset();
  pidYaw.reset();
  tgtRateY=0;
  if(compass.ok) {
    compassOffset = compass.headingDeg;
    yawCompassOff = angleDiff180(0.0f, yawGyroRaw);
    gocYaw = 0;
    yawOut = 0;
    tgtAngleY = compass.headingDeg;
    yawFusionReady = true;
    yawCompareOk = true;
    yawCompareBadCnt = 0;
    yawCompareErrDeg = 0;
    yawHoldActive = false;
    yawHoldSettleStartMs = 0;
  } else {
    tgtAngleY = gocYaw;
    yawFusionReady = false;
    yawCompareOk = false;
    yawCompareBadCnt = 0;
    yawHoldActive = false;
    yawHoldSettleStartMs = 0;
  }
}

bool coTheArm() {
  if(!sys.imuOk||!sys.sbusOk||sys.failsafe) return false;
  if(scores.imu<45||!scores.accTrusted) return false;
  docPin();
  if(sys.critBat) return false;
  if(CH[3]>GA_KHOI_DONG_MAX) return false;
  if(fabsf(rollOut)>10||fabsf(pitchOut)>10) return false;
  return true;
}

bool kichHoat() {
  if(!coTheArm()) return false;
  sys.autoLand=false; gaAutoLand=MUC_GA_NGHI; tAutoLandPrev=0; tAutoLandIdle=0;
  for(int i=1;i<=4;i++) tamCH[i]=CH[i]; tamCH[4]=1500;
  resetAllPID();
  datMocMatDat(); resetFlow();
  kfFlowX.reset(); kfFlowY.reset();
  sys.armed=true; sys.pidOn=true; sys.safety=false;
  resetYawHomeTheoLaBan();
  resetAltHoldState(MUC_GA_NGHI);
  return true;
}

void ngatKichHoat() {
  sys.armed=false; sys.pidOn=false;
  resetAltHoldState(MUC_GA_NGHI);
  yawHoldActive=false;
  yawHoldSettleStartMs=0;
  yawFusionReady=false; yawCompareOk=false; yawCompareBadCnt=0;
  sys.safety=false; sys.autoLand=false; gaAutoLand=MUC_GA_NGHI;
  tAutoLandPrev=0; tAutoLandIdle=0;
  resetAllPID(); datMocMatDat(); resetFlow();
  kfFlowX.reset(); kfFlowY.reset();
}

bool apDungAutoLand(int &ga) {
  if(!sys.armed) return false;
  if(sys.lowBat && !sys.autoLand) {
    sys.autoLand=true;
    gaAutoLand=constrain((float)ga,(float)MUC_GA_NGHI,(float)ESC_MAX_US);
    tAutoLandPrev=millis(); tAutoLandIdle=0;
  }
  if(!sys.autoLand) return false;
  unsigned long now=millis();
  float dt2 = tAutoLandPrev ? (now-tAutoLandPrev)*0.001f : (NHIP_FLIGHT_MS/1000.0f);
  tAutoLandPrev=now;
  if(dt2<=0||dt2>1) dt2=NHIP_FLIGHT_MS/1000.0f;
  gaAutoLand -= HA_CANH_GIAM_US_S*dt2;
  float landFloorM = alt.groundReady ? (alt.groundM + HA_CANH_GAN_DAT_OFFSET_M) : 0.30f;
  landFloorM = constrain(landFloorM, ALT_HOLD_TARGET_MIN_M, 0.45f);
  bool ganDat = alt.ok && alt.altM <= landFloorM;
  if(!ganDat && sys.altHold && gaAutoLand < (float)(GA_HOAT_DONG_MIN + 20)) {
    gaAutoLand = (float)(GA_HOAT_DONG_MIN + 20);
  }
  if(gaAutoLand<MUC_GA_NGHI) gaAutoLand=MUC_GA_NGHI;
  if(ga>(int)gaAutoLand) ga=(int)gaAutoLand;
  else if((float)ga<gaAutoLand) gaAutoLand=(float)ga;
  if(ganDat && ga<=GA_HOAT_DONG_MIN) {
    if(!tAutoLandIdle) tAutoLandIdle=now;
    if(now-tAutoLandIdle>=HA_CANH_IDLE_MS) { ngatKichHoat(); ghiTatMotor(); return true; }
  } else tAutoLandIdle=0;
  return false;
}

void kiemTraCuChi() {
  if(!sys.armed && CH[5]>1750) {
    if(!armGesture) { armGesture=true; tArmGesture=millis(); }
    else if(millis()-tArmGesture>3000) { kichHoat(); armGesture=false; }
  } else armGesture=false;
  if(sys.armed && CH[5]<1250) {
    if(!disarmGesture) { disarmGesture=true; tDisarmGesture=millis(); }
    else if(millis()-tDisarmGesture>1000) { ngatKichHoat(); disarmGesture=false; }
  } else disarmGesture=false;
}

float stickToAngle(int ch, int center) {
  int d=ch-center;
  if(abs(d)<STICK_DEADBAND) d=0;
  return constrain((float)d/500.0f*GIOI_HAN_GOC, -GIOI_HAN_GOC, GIOI_HAN_GOC);
}

float stickToYawRate(int ch, int center) {
  int d=ch-center;
  if(abs(d)<YAW_STICK_DEADBAND) d=0;
  return constrain((float)d/500.0f*MAX_YAW_RATE_DPS, -MAX_YAW_RATE_DPS, MAX_YAW_RATE_DPS);
}

void tronMotor(int ga, float roll, float pitch, float yaw) {
  roll=constrain(roll,-MOTOR_CORR_LIMIT,MOTOR_CORR_LIMIT);
  pitch=constrain(pitch,-MOTOR_CORR_LIMIT,MOTOR_CORR_LIMIT);
  yaw=constrain(yaw,-MOTOR_CORR_LIMIT,MOTOR_CORR_LIMIT);
  float corr[4], maxC=-9999, minC=9999;
  for(int i=0;i<4;i++) {
    corr[i] = MIX_ROLL[i]*roll + MIX_PITCH[i]*pitch + MIX_YAW[i]*yaw;
    maxC=max(maxC,corr[i]); minC=min(minC,corr[i]);
  }
  float scale=1.0f;
  float up=(float)ESC_MAX_US-ga, down=(float)ga-MUC_GA_NGHI;
  if(maxC>up && maxC>0) scale=min(scale,up/maxC);
  if(-minC>down && minC<0) scale=min(scale,down/(-minC));
  scale=constrain(scale,0.0f,1.0f);
  for(int i=0;i<4;i++) {
    esc[i]=constrain((int)((float)ga+corr[i]*scale+0.5f), MUC_GA_NGHI, ESC_MAX_US);
    dongCo[i].writeMicroseconds(esc[i]);
  }
}

void capNhatMotor() {
  tgtAngleR=tgtAngleP=0; tgtRateR=tgtRateP=tgtRateY=0;
  pidOutR=pidOutP=pidOutY=pidOutAlt=0;

  if(sys.failsafe) {
    if(sys.armed) ngatKichHoat();
    ghiTatMotor(); return;
  }
  if(sys.armed && (fabsf(rollOut)>=GOC_AN_TOAN||fabsf(pitchOut)>=GOC_AN_TOAN)) {
    sys.safety=true; ngatKichHoat(); ghiTatMotor(); return;
  }
  if(!sys.armed) { ghiTatMotor(); return; }

  int ga = constrain(map(CH[3],1000,2000,MUC_GA_NGHI,ESC_MAX_US), MUC_GA_NGHI, ESC_MAX_US);
  if(apDungAutoLand(ga)) return;
  mucGa=ga;

  if(ga<GA_HOAT_DONG_MIN) {
    resetAllPID(); resetAltHoldState(MUC_GA_NGHI);
    yawHoldActive=false;
    yawHoldSettleStartMs=0;
    tgtAngleY=gocYaw;
    tgtRateY=0;
    ghiIdleMotor(); return;
  }

  float buR=0, buP=0, buY=0;
  if(sys.pidOn) {
    
#if ENABLE_ALT_HOLD
    static bool altHoldSwitchOn = false;
    if(CH[6] >= ALT_HOLD_CH6_ON_US) altHoldSwitchOn = true;
    else if(CH[6] <= ALT_HOLD_CH6_OFF_US) altHoldSwitchOn = false;

    if(alt.ok && (altHoldSwitchOn || sys.autoLand)) {
      if(!altHoldActive) {
        tgtAlt=constrain(alt.altM, ALT_HOLD_TARGET_MIN_M, ALT_HOLD_TARGET_MAX_M);
        tgtAltVel=0;
        hoverThrottle=mucGa;
        pidAlt.reset();
        altHoldActive=true;
      }
      sys.altHold=true;
      if(sys.autoLand) {
        float landTargetM = alt.groundReady ? (alt.groundM + HA_CANH_GAN_DAT_OFFSET_M) : 0.30f;
        landTargetM = constrain(landTargetM, ALT_HOLD_TARGET_MIN_M, 0.45f);
        tgtAlt = max(landTargetM, tgtAlt - HA_CANH_ALT_GIAM_M_S * dt_flight);
      }
      float altErr = constrain(tgtAlt - alt.altM, -1.0f, 1.0f);
      float holdVzCmd = constrain(altErr * ALT_HOLD_ALT_KP, -ALT_HOLD_VZ_MAX_MPS, ALT_HOLD_VZ_MAX_MPS);
      tgtAltVel = holdVzCmd;
      float outA = pidAlt.compute(tgtAltVel, altVelMps, dt_flight);
      if(!altVelHealthy) outA = constrain(outA, -(float)ALT_SPIKE_BU_LIMIT, (float)ALT_SPIKE_BU_LIMIT);
      pidOutAlt=outA;
      if(fabsf(altErr) < 0.08f && fabsf(outA) < 80.0f) {
        hoverThrottle += 0.004f * outA;
        hoverThrottle = constrain(hoverThrottle, (float)GA_HOAT_DONG_MIN, (float)(ESC_MAX_US - ALT_HOLD_BU_LIMIT));
      }
      mucGa = constrain((int)(hoverThrottle + outA), MUC_GA_NGHI, ESC_MAX_US);
    } else {
      resetAltHoldState(mucGa);
    }
#else
    resetAltHoldState(mucGa);
#endif

    tgtAngleR = stickToAngle(CH[1], tamCH[1]);
    tgtAngleP = stickToAngle(CH[2], tamCH[2]);

#if ENABLE_POS_HOLD
    bool flowNew = alt.flowLastMs && (millis()-alt.flowLastMs<=MTF_TIMEOUT_MS);
    static float target_pos_x = 0, target_pos_y = 0;
    static float vel_i_x = 0, vel_i_y = 0;
    static bool pos_hold_active = false;

    if(sys.altOk && flowNew && alt.flowQuality>FLOW_MIN_QUALITY &&
       abs(CH[1]-tamCH[1])<STICK_DEADBAND && abs(CH[2]-tamCH[2])<STICK_DEADBAND) {
        
        if (!pos_hold_active) {
            target_pos_x = flowPosX;
            target_pos_y = flowPosY;
            vel_i_x = 0; vel_i_y = 0;
            pos_hold_active = true;
        }

        float err_pos_x = constrain(target_pos_x - flowPosX, -100.0f, 100.0f);
        float err_pos_y = constrain(target_pos_y - flowPosY, -100.0f, 100.0f);
        
        float target_vel_x = err_pos_x * pidFlowX.kp;
        float target_vel_y = err_pos_y * pidFlowY.kp;
        
        float vel_err_x = target_vel_x - flowVelX;
        float vel_err_y = target_vel_y - flowVelY;
        
        vel_i_x += vel_err_x * pidFlowX.kd * dt_flight;
        vel_i_y += vel_err_y * pidFlowY.kd * dt_flight;
        
        vel_i_x = constrain(vel_i_x, -5.0f, 5.0f);
        vel_i_y = constrain(vel_i_y, -5.0f, 5.0f);
        
        float cmd_x = constrain(vel_err_x * pidFlowX.ki + vel_i_x, -FLOW_PID_OUT_LIMIT_DEG, FLOW_PID_OUT_LIMIT_DEG);
        float cmd_y = constrain(vel_err_y * pidFlowY.ki + vel_i_y, -FLOW_PID_OUT_LIMIT_DEG, FLOW_PID_OUT_LIMIT_DEG);
        
        
        tgtAngleP += cmd_x;
        tgtAngleR += cmd_y;
        
        tgtAngleR = constrain(tgtAngleR, -15.0f, 15.0f);
        tgtAngleP = constrain(tgtAngleP, -15.0f, 15.0f);
    } else { 
        pos_hold_active = false; 
        target_pos_x = flowPosX;
        target_pos_y = flowPosY;
        vel_i_x = 0; vel_i_y = 0;
        pidFlowX.reset(); pidFlowY.reset(); 
    }
#endif
   
    tgtRateR = constrain(pidAngleR.compute(tgtAngleR, rollOut, dt_flight), -MAX_RATE_DPS, MAX_RATE_DPS);
    tgtRateP = constrain(pidAngleP.compute(tgtAngleP, pitchOut, dt_flight), -MAX_RATE_DPS, MAX_RATE_DPS);
    buR = pidRateR.compute(tgtRateR, gx_filt, dt_flight);
    buP = pidRateP.compute(tgtRateP, gy_filt, dt_flight);

    
    if(compass.ok) {
      if(!yawHoldActive) {
        if(!yawHoldSettleStartMs) yawHoldSettleStartMs = millis();
        tgtAngleY = compass.headingDeg;
        pidAngleY.reset();
        if(millis() - yawHoldSettleStartMs >= YAW_HOLD_SETTLE_MS) {
          yawHoldActive = true;
        }
      }
      if(yawHoldActive) {
        float saiY = angleDiff180(tgtAngleY, compass.headingDeg);
        if(fabsf(saiY)<0.5f) saiY=0;
        tgtRateY = constrain(pidAngleY.compute(saiY, 0, dt_flight), -MAX_RATE_DPS, MAX_RATE_DPS);
      } else {
        tgtRateY = 0;
      }
    } else tgtRateY=0;
    buY = pidYaw.compute(tgtRateY, yawRate, dt_flight);
  } else {
    resetAltHoldState(mucGa);
  }

  pidOutR=buR; pidOutP=buP; pidOutY=buY;
  tronMotor(mucGa, buR, buP, buY);
}

bool khoiTaoWifi() {
  WiFi.persistent(false); WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_17dBm); WiFi.setSleep(false);
  WiFi.setHostname("esp32-drone");
  WiFi.begin(HOME_WIFI_SSID, HOME_WIFI_PASS);
  wifiRetryPrev=millis();
  unsigned long t0=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-t0<WIFI_STA_TIMEOUT) delay(250);
  wifiOk = (WiFi.status()==WL_CONNECTED);
  return wifiOk;
}

void kiemTraWifi() {
  unsigned long now=millis();
  if(now-wifiCheckPrev < WIFI_CHECK_MS) return;
  wifiCheckPrev=now;
  if(WiFi.getMode()==WIFI_OFF) { khoiTaoWifi(); srv.begin(); }
  if(WiFi.status()!=WL_CONNECTED) {
    wifiOk=false;
    if(now-wifiRetryPrev>=WIFI_STA_RETRY_MS) { WiFi.begin(HOME_WIFI_SSID,HOME_WIFI_PASS); wifiRetryPrev=now; }
  } else wifiOk=true;
  wifiSignal = (WiFi.status()==WL_CONNECTED) ?
    (uint8_t)constrain((WiFi.RSSI()+90)*100/45,0,100) : 0;
}

void guiJson(bool ok) {
  srv.sendHeader("Cache-Control","no-store");
  srv.send(200,"application/json", ok?"{\"ok\":true}":"{\"ok\":false}");
}

void trangThaiWeb() {
  SharedData d;
  if(xSemaphoreTake(dataMutex,(TickType_t)5)==pdTRUE) { d=shared; xSemaphoreGive(dataMutex); }
  else { srv.send(503,"application/json","{\"err\":\"busy\"}"); return; }

  float altC = max(0.0f, d.altM);
  unsigned long now=millis();
  unsigned long altAge = d.altLastMs ? (now-d.altLastMs) : 0;
  unsigned long flowAge = d.flowLastMs ? (now-d.flowLastMs) : 0;

  static char buf[8192];
  snprintf(buf,sizeof(buf),
    "{\"daKichHoat\":%s,\"batPid\":%s,\"altHold\":%s,\"icmOk\":%s,\"sbusOk\":%s,\"gpsOk\":%s,\"gpsFix\":%s,\"compassOk\":%s,\"altSensorOk\":%s,"
    "\"matTinHieu\":%s,\"anToan\":%s,\"pinYeu\":%s,\"pinCan\":%s,\"tuDongHaCanh\":%s,"
    "\"mucWifi\":%u,\"mucSbus\":%u,"
    "\"gocRoll\":%.2f,\"gocPitch\":%.2f,\"gocYaw\":%.2f,\"gx\":%.2f,\"gy\":%.2f,\"yawRate\":%.2f,\"doCao\":%.2f,\"dienApPin\":%.2f,\"phanTramPin\":%.1f,"
    "\"gpsLat\":%.7f,\"gpsLon\":%.7f,\"gpsAltM\":%.2f,\"gpsSat\":%u,\"gpsSats\":%u,\"gpsHdop\":%.2f,"
    "\"gpsSpeedKmh\":%.2f,\"gpsCourseDeg\":%.2f,\"gpsHeadingDiffDeg\":0.0,\"gpsBytes\":%lu,\"gpsNmea\":0,\"gpsGga\":0,\"gpsRmc\":0,\"gpsBad\":0,\"gpsRejected\":0,"
    "\"gpsLastSentenceMs\":%lu,\"gpsLastByteMs\":%lu,\"gpsAgeMs\":%lu,"
    "\"gpsState\":\"%s\",\"gpsLastType\":\"UBX\",\"gpsLastLine\":\"PVT\","
    "\"compassState\":\"%s\",\"headingDeg\":%.1f,\"magX\":%d,\"magY\":%d,\"magZ\":%d,"
    "\"altNewM\":%.2f,\"altRawM\":%.2f,\"altFilteredM\":%.2f,\"altGroundM\":%.2f,\"altState\":\"%s\","
    "\"altQuality\":%u,\"flowQuality\":%u,\"altBytes\":%lu,\"altFrames\":%lu,\"altRangeFrames\":%lu,\"altFlowFrames\":%lu,\"altBadFrames\":%lu,"
    "\"altLastMs\":%lu,\"altAgeMs\":%lu,\"flowLastMs\":%lu,\"flowAgeMs\":%lu,"
    "\"gpsHomeSet\":%s,\"gpsHomeLat\":%.7f,\"gpsHomeLon\":%.7f,"
    "\"nhietMpu\":%.1f,\"nhietEsp\":%.1f,"
    "\"pidOut\":[%.1f,%.1f,%.1f],\"pidAltOut\":%.1f,\"pidTarget\":[%.1f,%.1f],\"angleTarget\":[%.1f,%.1f],\"yawTarget\":[%.1f,%.1f],\"altTarget\":%.2f,"
    "\"altVel\":%.2f,\"altVelRaw\":%.2f,\"altTargetVel\":%.2f,\"altVelOk\":%s,\"altVelSpikes\":%u,"
    "\"CH\":[%d,%d,%d,%d,%d,%d],\"esc\":[%d,%d,%d,%d],"
    "\"pRP\":[%.3f,%.4f,%.4f],\"pRateR\":[%.3f,%.4f,%.4f],\"pRateP\":[%.3f,%.4f,%.4f],"
    "\"pAngleR\":[%.3f,%.4f,%.4f],\"pAngleP\":[%.3f,%.4f,%.4f],"
    "\"pYaw\":[%.3f,%.4f,%.4f],\"pYawHold\":[%.3f,%.4f,%.4f],\"pAlt\":[%.3f,%.3f,%.3f],\"pOpt\":[%.3f,%.3f,%.3f],\"optXY\":[%.2f,%.2f],\"optPosXY\":[%.2f,%.2f],"
    "\"imuScore\":%u,\"gpsScore\":%u,\"compassScore\":%u,\"mtfScore\":%u,\"flowScore\":%u,"
    "\"accNorm\":%.3f,\"vibrationScore\":%.1f,\"accTrusted\":%s,"
    "\"gpsTrusted\":%s,\"compassTrusted\":%s,\"mtfTrusted\":%s,\"flowTrusted\":%s,"
    "\"compassHeading\":%.1f,\"mtfAlt\":%.2f,\"mtfQuality\":%u,\"flowX\":%.2f,\"flowY\":%.2f,"
    "\"altitudeSource\":\"%s\",\"yawSource\":\"%s\",\"yawCompareDeg\":%.1f,\"yawCompareOk\":%s,"
    "\"altDistanceMm\":%ld,\"altPayloadLen\":%u,"
    "\"fAng\":%.1f,\"nF\":%.1f,\"nB\":%.1f,\"rOff\":%.2f,\"pOff\":%.2f}",
    d.armed?"true":"false", d.pidOn?"true":"false", d.altHold?"true":"false",
    d.imuOk?"true":"false", d.sbusOk?"true":"false", d.gpsOk?"true":"false", d.gpsFix?"true":"false",
    d.compassOk?"true":"false", d.altOk?"true":"false",
    d.failsafe?"true":"false", d.safety?"true":"false",
    d.lowBat?"true":"false", d.critBat?"true":"false", d.autoLand?"true":"false",
    d.wifiSig, d.sbusSig,
    d.rollOut, d.pitchOut, d.yawOut, d.gx, d.gy, d.yawRate, altC, d.batV, d.batPct,
    d.gpsLat, d.gpsLon, d.gpsAlt, d.gpsSat, d.gpsSat, d.gpsHdop,
    d.gpsSpeed, d.gpsCourse, (unsigned long)d.gpsPollCnt,
    (unsigned long)d.gpsLastMs, (unsigned long)d.gpsLastMs,
    d.gpsLastMs?(unsigned long)(now-d.gpsLastMs):0UL,
    d.gpsState, d.compassState, d.heading, d.magX, d.magY, d.magZ,
    d.altM, d.altRaw, d.altFilt, d.altGround, d.altState,
    d.altQuality, d.flowQuality, (unsigned long)d.altBytes, (unsigned long)d.altFrames,
    (unsigned long)d.altRangeF, (unsigned long)d.altFlowF, (unsigned long)d.altBadF,
    (unsigned long)d.altLastMs, (unsigned long)altAge,
    (unsigned long)d.flowLastMs, (unsigned long)flowAge,
    d.gpsHomeSet?"true":"false", d.gpsHomeLat, d.gpsHomeLon,
    d.mpuT, d.espT, d.pidR, d.pidP, d.pidY, d.pidAlt,
    d.tgtRateR, d.tgtRateP, d.tgtAngleR, d.tgtAngleP, d.tgtRateY, d.tgtAngleY, d.tgtAlt,
    d.altVel, d.altVelRaw, d.tgtAltVel, d.altVelOk?"true":"false", (unsigned int)d.altVelSpikes,
    d.CH[1],d.CH[2],d.CH[3],d.CH[4],d.CH[5],d.CH[6], d.esc[0],d.esc[1],d.esc[2],d.esc[3],
    pidRateR.kp,pidRateR.ki,pidRateR.kd,
    pidRateR.kp,pidRateR.ki,pidRateR.kd,
    pidRateP.kp,pidRateP.ki,pidRateP.kd,
    pidAngleR.kp,pidAngleR.ki,pidAngleR.kd,
    pidAngleP.kp,pidAngleP.ki,pidAngleP.kd,
    pidYaw.kp,pidYaw.ki,pidYaw.kd,
    pidAngleY.kp,pidAngleY.ki,pidAngleY.kd,
    pidAlt.kp,pidAlt.ki,pidAlt.kd,
    pidFlowX.kp,pidFlowX.ki,pidFlowX.kd, d.flowX,d.flowY, d.flowPosX,d.flowPosY,
    d.imuScore, d.gpsScore, d.compassScore, d.mtfScore, d.flowScore,
    d.accNorm, d.vibScore, d.accTrusted?"true":"false",
    d.gpsTrusted?"true":"false", d.compassTrusted?"true":"false",
    d.mtfTrusted?"true":"false", d.flowTrusted?"true":"false",
    d.compassHead, d.altM, d.altQuality, d.flowX, d.flowY,
    d.altSrc, d.yawSrc, d.yawCompareErr, d.yawCompareOk?"true":"false",
    (long)d.altDistMm, (unsigned int)d.altPayloadLen,
    d.fAng, d.nF, d.nB, d.rOff, d.pOff);
  srv.sendHeader("Cache-Control","no-store");
  srv.sendHeader("Access-Control-Allow-Origin", "*");
  srv.setContentLength(CONTENT_LENGTH_UNKNOWN);
  srv.send(200,"application/json","");
  srv.sendContent(buf);
  srv.sendContent("");
}

void caiDatWeb() {
  srv.on("/", [](){ srv.sendHeader("Cache-Control","no-store"); srv.send_P(200,"text/html",WEB_UI_HTML); });
  srv.on("/status", trangThaiWeb);
  srv.on("/arm", [](){ guiJson(kichHoat()); });
  srv.on("/disarm", [](){ ngatKichHoat(); ghiTatMotor(); guiJson(true); });
  srv.on("/reset_safe", [](){ sys.safety=false; guiJson(true); });
  srv.on("/reset_flow_map", [](){ resetFlow(); guiJson(true); });
  srv.on("/pid_toggle", [](){ sys.pidOn=!sys.pidOn; resetAllPID(); guiJson(true); });
  srv.on("/set_center", [](){ for(int i=1;i<=4;i++) tamCH[i]=CH[i]; tamCH[4]=1500; resetAllPID(); guiJson(true); });
  srv.on("/set_gps_home", [](){
    if(gps.ok&&gps.fix&&gpsHasValidPosition()) { gpsHomeLat=gps.lat; gpsHomeLon=gps.lon; gpsHomeSet=true; guiJson(true); }
    else guiJson(false);
  });

  srv.on("/save_pid", [](){
    auto getF=[](const char*k,float&v){ if(srv.hasArg(k)) v=srv.arg(k).toFloat(); };
    float rpKp=pidRateR.kp, rpKi=pidRateR.ki, rpKd=pidRateR.kd;
    getF("rpKp",rpKp); getF("rpKi",rpKi); getF("rpKd",rpKd);
    if(srv.hasArg("rpKp")||srv.hasArg("rpKi")||srv.hasArg("rpKd")) {
      pidRateR.kp=pidRateP.kp=rpKp; pidRateR.ki=pidRateP.ki=rpKi; pidRateR.kd=pidRateP.kd=rpKd;
    }
    getF("rrKp",pidRateR.kp); getF("rrKi",pidRateR.ki); getF("rrKd",pidRateR.kd);
    getF("prKp",pidRateP.kp); getF("prKi",pidRateP.ki); getF("prKd",pidRateP.kd);
    float aKp=pidAngleR.kp;
    if(srv.hasArg("angleKp")) { aKp=srv.arg("angleKp").toFloat(); pidAngleR.kp=pidAngleP.kp=aKp; }
    getF("arKp",pidAngleR.kp); getF("arKi",pidAngleR.ki); getF("arKd",pidAngleR.kd);
    getF("apKp",pidAngleP.kp); getF("apKi",pidAngleP.ki); getF("apKd",pidAngleP.kd);
    getF("yhKp",pidAngleY.kp); getF("yhKi",pidAngleY.ki); getF("yhKd",pidAngleY.kd);
    getF("yKp",pidYaw.kp); getF("yKi",pidYaw.ki); getF("yKd",pidYaw.kd);
    getF("aKp",pidAlt.kp); getF("aKi",pidAlt.ki); getF("aKd",pidAlt.kd);
    getF("altKp",pidAlt.kp); getF("altKi",pidAlt.ki); getF("altKd",pidAlt.kd);
    float optKp=pidFlowX.kp, optKi=pidFlowX.ki, optKd=pidFlowX.kd;
    getF("optKp",optKp); getF("optKi",optKi); getF("optKd",optKd);
    pidFlowX.kp=pidFlowY.kp=optKp; pidFlowX.ki=pidFlowY.ki=optKi; pidFlowX.kd=pidFlowY.kd=optKd;
    pidFlowX.setLimits(FLOW_PID_OUT_LIMIT_DEG, FLOW_PID_INT_LIMIT, FLOW_DTERM_LPF_HZ);
    pidFlowY.setLimits(FLOW_PID_OUT_LIMIT_DEG, FLOW_PID_INT_LIMIT, FLOW_DTERM_LPF_HZ);
    if(!srv.hasArg("auto")) luuCauHinh();
    guiJson(true);
  });

  srv.on("/save_config", [](){ 
    if(srv.hasArg("hSPin")) heSoPin=srv.arg("hSPin").toFloat(); 
    if(srv.hasArg("fAng")) flowAngleDeg=srv.arg("fAng").toFloat();
    if(srv.hasArg("nF")) notchFreq=constrain(srv.arg("nF").toFloat(),20,110);
    if(srv.hasArg("nB")) notchBw=constrain(srv.arg("nB").toFloat(),5,50);
    initFilters(); luuCauHinh(); guiJson(true); 
  });
  srv.on("/save_offset", [](){
    if(srv.hasArg("rOff")) rollOffset=constrain(srv.arg("rOff").toFloat(),-30,30);
    if(srv.hasArg("pOff")) pitchOffset=constrain(srv.arg("pOff").toFloat(),-30,30);
    luuCauHinh(); guiJson(true);
  });
  srv.begin();
}

void syncSharedData() {
  if(xSemaphoreTake(dataMutex,(TickType_t)2)!=pdTRUE) return;
  shared.rollOut=rollOut; shared.pitchOut=pitchOut; shared.yawOut=yawOut; shared.yawRate=yawRate;
  shared.gx=gx_filt; shared.gy=gy_filt;
  shared.pidR=pidOutR; shared.pidP=pidOutP; shared.pidY=pidOutY; shared.pidAlt=pidOutAlt;
  shared.tgtRateR=tgtRateR; shared.tgtRateP=tgtRateP; shared.tgtRateY=tgtRateY;
  shared.tgtAngleR=tgtAngleR; shared.tgtAngleP=tgtAngleP; shared.tgtAngleY=tgtAngleY; shared.tgtAlt=tgtAlt;
  shared.flowX=flowVelX; shared.flowY=flowVelY; shared.flowPosX=flowPosX; shared.flowPosY=flowPosY;
  shared.batV=batV; shared.batPct=batPct; shared.mpuT=mpuTemp; shared.espT=espTemp;
  shared.accNorm=scores.accNorm; shared.vibScore=scores.vibScore; shared.accTrusted=scores.accTrusted;
  shared.yawCompareErr=yawCompareErrDeg; shared.yawCompareOk=yawCompareOk;
  shared.imuScore=scores.imu; shared.gpsScore=scores.gps; shared.compassScore=scores.compass;
  shared.mtfScore=scores.mtf; shared.flowScore=scores.flow;
  shared.gpsTrusted=scores.gpsTrusted; shared.compassTrusted=scores.compassTrusted;
  shared.mtfTrusted=scores.mtfTrusted; shared.flowTrusted=scores.flowTrusted;
  shared.compassHead=compass.headingDeg; shared.heading=compass.headingDeg;
  shared.magX=compass.rawX; shared.magY=compass.rawY; shared.magZ=compass.rawZ;
  shared.gpsLat=gps.lat; shared.gpsLon=gps.lon; shared.gpsAlt=gps.altM;
  shared.gpsSpeed=gps.speedKmh; shared.gpsCourse=gps.courseDeg;
  shared.gpsSat=gps.sats; shared.gpsHdop=gps.hdop;
  shared.gpsPollCnt=gps.pollCount; shared.gpsLastMs=gps.lastMs;
  copyStr(shared.gpsState, sizeof(shared.gpsState), gps.state);
  copyStr(shared.compassState, sizeof(shared.compassState), compass.state);
  shared.altM=alt.altM; shared.altRaw=alt.rawM; shared.altFilt=alt.filteredM; shared.altGround=alt.groundM;
  shared.altVel=altVelMps; shared.altVelRaw=altVelRawMps; shared.tgtAltVel=tgtAltVel;
  shared.altVelOk=altVelHealthy; shared.altVelSpikes=altVelSpikeCount;
  shared.altQuality=alt.quality; shared.flowQuality=alt.flowQuality;
  shared.altBytes=alt.bytes; shared.altFrames=alt.frames; shared.altRangeF=alt.rangeFrames;
  shared.altFlowF=alt.flowFrames; shared.altBadF=alt.badFrames;
  shared.altLastMs=alt.lastMs; shared.flowLastMs=alt.flowLastMs;
  shared.altDistMm=alt.lastDistMm; shared.altPayloadLen=alt.lastPayloadLen;
  copyStr(shared.altState, sizeof(shared.altState), alt.state);
  shared.gpsHomeSet=gpsHomeSet; shared.gpsHomeLat=gpsHomeLat; shared.gpsHomeLon=gpsHomeLon;
  shared.wifiSig=wifiSignal; shared.sbusSig=sbusSignal;
  for(int i=1;i<=6;i++) shared.CH[i]=CH[i];
  for(int i=0;i<4;i++) shared.esc[i]=esc[i];
  shared.armed=sys.armed; shared.pidOn=sys.pidOn; shared.altHold=sys.altHold;
  shared.imuOk=sys.imuOk; shared.sbusOk=sys.sbusOk;
  shared.gpsOk=sys.gpsOk; shared.gpsFix=gps.fix&&gpsHasValidPosition(); shared.compassOk=sys.compassOk; shared.altOk=sys.altOk;
  shared.failsafe=sys.failsafe; shared.safety=sys.safety;
  shared.lowBat=sys.lowBat; shared.critBat=sys.critBat; shared.autoLand=sys.autoLand;
  shared.nF=notchFreq; shared.nB=notchBw; shared.rOff=rollOffset; shared.pOff=pitchOffset;
  shared.fAng=flowAngleDeg;
  xSemaphoreGive(dataMutex);
}

void taskFlight(void* pv) {
  TickType_t xLast = xTaskGetTickCount();
  const TickType_t xFreq = pdMS_TO_TICKS(NHIP_FLIGHT_MS);
  for(;;) {
    unsigned long nowUs=micros();
    if(tFlightPrev) { float d=(nowUs-tFlightPrev)*1e-6f; dt_flight=(d>0.001f&&d<0.05f)?d:(NHIP_FLIGHT_MS/1000.0f); }
    tFlightPrev=nowUs;

    docIMU(); docSBUS(); kiemTraCuChi(); capNhatMotor();

    unsigned long nowMs=millis();
    if(nowMs-tBatPrev>=500) { tBatPrev=nowMs; docPin(); }
#if ENABLE_COMPASS
    docCompass();
#endif
    capNhatAltState();
    syncSharedData();

    TickType_t now=xTaskGetTickCount();
    if(now-xLast>=xFreq) { xLast=now; vTaskDelay(1); }
    else vTaskDelayUntil(&xLast, xFreq);
  }
}

void updateXYFlowVelocityFromMtf() {
  alt.flowQuality = mtf.flowQuality;
  alt.flowLastMs = mtf.lastFlowMs;

  float h = alt.altM;
  if (h < 0.1f) h = 0.1f;

  float raw_x_h = mtf.raw_flow_x * h;
  float raw_y_h = mtf.raw_flow_y * h;

  float aRad = flowAngleDeg * PI / 180.0f;
  float cA = cos(aRad);
  float sA = sin(aRad);
  float vx_cm_s = raw_x_h * cA - raw_y_h * sA;
  float vy_cm_s = raw_x_h * sA + raw_y_h * cA;

  flowVelX = kfFlowX.update(vx_cm_s);
  flowVelY = kfFlowY.update(vy_cm_s);

  static unsigned long lastFlowUpdateMs = 0;
  if (mtf.lastFlowMs != lastFlowUpdateMs) {
    float dt = lastFlowUpdateMs ? ((mtf.lastFlowMs - lastFlowUpdateMs) / 1000.0f) : 0;
    lastFlowUpdateMs = mtf.lastFlowMs;

    if (mtf.flowQuality >= FLOW_MIN_QUALITY && dt > 0 && dt <= FLOW_MAP_MAX_DT_S) {
      flowPosX += flowVelX * dt;
      flowPosY += flowVelY * dt;
    }
  }
}

void taskComm(void* pv) {
  unsigned long tGpsPollPrev = 0;
  for(;;) {
    mtf.update();

    alt.rawM = mtf.altitudeM;
    
    
    static float alt_ema = 0;
    static bool alt_init = false;
    if (!alt_init || alt.rawM < 0.05f) { 
      alt_ema = alt.rawM;
      alt_init = true;
    } else {
      alt_ema += 0.2f * (alt.rawM - alt_ema); 
    }
    alt.filteredM = alt_ema;
    
    alt.altM = alt.filteredM;

    static bool altVelReady = false;
    static float altVelPrevM = 0.0f;
    static unsigned long altVelPrevMs = 0;
    unsigned long altVelNowMs = millis();
    if(!altVelReady) {
      altVelPrevM = alt.altM;
      altVelPrevMs = altVelNowMs;
      altVelMps = 0;
      altVelRawMps = 0;
      altVelHealthy = true;
      altVelReady = true;
    } else {
      float dtAlt = (altVelNowMs - altVelPrevMs) * 0.001f;
      if(dtAlt >= 0.02f && dtAlt <= 0.25f) {
        float dAlt = alt.altM - altVelPrevM;
        float rawVz = dAlt / dtAlt;
        bool spike = fabsf(dAlt) > ALT_SAMPLE_JUMP_MAX_M || fabsf(rawVz) > ALT_VEL_SPIKE_LIMIT_MPS;
        if(spike) {
          altVelSpikeCount++;
          altVelHealthy = false;
          altVelRawMps = constrain(rawVz, -ALT_VEL_RAW_LIMIT_MPS, ALT_VEL_RAW_LIMIT_MPS);
          altVelMps *= 0.75f;
          altVelPrevM = alt.altM;
          altVelPrevMs = altVelNowMs;
        } else {
          altVelHealthy = true;
          altVelRawMps = constrain(rawVz, -ALT_VEL_RAW_LIMIT_MPS, ALT_VEL_RAW_LIMIT_MPS);
          altVelMps += ALT_VEL_LPF_ALPHA * (altVelRawMps - altVelMps);
          altVelMps = constrain(altVelMps, -ALT_VEL_RAW_LIMIT_MPS, ALT_VEL_RAW_LIMIT_MPS);
          altVelPrevM = alt.altM;
          altVelPrevMs = altVelNowMs;
        }
      } else if(dtAlt > 0.25f) {
        altVelPrevM = alt.altM;
        altVelPrevMs = altVelNowMs;
        altVelMps = 0;
        altVelRawMps = 0;
        altVelHealthy = false;
      }
    }

    updateXYFlowVelocityFromMtf();
    unsigned long nowMs = millis();
    if(nowMs - tGpsPollPrev >= 50) {
      tGpsPollPrev = nowMs;
      docGPS();
    }
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void taskWeb(void* pv) {
  for(;;) {
#if ENABLE_WEB_UI
    kiemTraWifi();
    srv.handleClient();
#endif
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  delay(300);

  dataMutex = xSemaphoreCreateMutex();
  taiCauHinh();
  initFilters();

  sys.imuOk = mpu.begin(PIN_SDA, PIN_SCL);
  if(sys.imuOk) mpu.calibrate();
  Wire.setTimeOut(20);

#if ENABLE_COMPASS
  khoiTaoCompass();
#endif

  Serial1.setRxBufferSize(512);
  Serial1.begin(100000, SERIAL_8E2, PIN_SBUS_RX, -1, true);
  while(Serial1.available()) Serial1.read();
  docPin();

#if ENABLE_WEB_UI
  khoiTaoWifi();
  caiDatWeb();
#endif

  khoiTaoGPS();

  MtfSerial.setRxBufferSize(1024);
  mtf.begin(&MtfSerial, 115200, PIN_MTF_RX, PIN_MTF_TX, false);

  ESP32PWM::allocateTimer(0); ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2); ESP32PWM::allocateTimer(3);
  for(int i=0;i<4;i++) {
    dongCo[i].setPeriodHertz(400);
    dongCo[i].attach(PIN_ESC[i],1000,2000);
    dongCo[i].writeMicroseconds(1000);
  }
  delay(2000);
  tBatPrev=millis();

  xTaskCreatePinnedToCore(taskFlight, "Flight", 8192, NULL, 2, NULL, 1);
#if ENABLE_WEB_UI
  xTaskCreatePinnedToCore(taskWeb,    "Web",    8192, NULL, 1, NULL, 0);
#endif
  xTaskCreatePinnedToCore(taskComm,   "Comm",   8192, NULL, 1, NULL, 0);
}

void loop() { vTaskDelete(NULL); }
