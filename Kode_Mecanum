
//--------------------------------------------

// DXL SETUP
//------------------------------------------------//
#include "Dynamixel_Serial.h"
#define dxl_id 0x01
#define dxl_controlpin 7
#define dxl_baudrate 1000000

void dxl_init(int did, int mode, int cwlimit, int ccwlimit) {
  Dynamixel.begin(dxl_baudrate, dxl_controlpin);
  Dynamixel.setMode(did, mode, cwlimit, ccwlimit);
}
void dxl_init_wheel(int did, int mode, int cwlimit, int ccwlimit) {
  Dynamixel.begin(dxl_baudrate, dxl_controlpin);
  Dynamixel.setMode(did, WHEEL, 0, 0);
}
void dxlservo(int did, int dpos, int dspeed) {
  Dynamixel.servo(did, dpos, dspeed);
  delay(1);
}
void dxlsetid(int did) {
  Dynamixel.begin(dxl_baudrate, dxl_controlpin);
  Dynamixel.setID(BROADCAST_ID, did);
}
void dxlwheel(int did, int dspeed) {
  if (dspeed < 0) Dynamixel.wheel(did, LEFT, -dspeed);
  else if (dspeed > 0) Dynamixel.wheel(did, RIGHT, dspeed);
  else Dynamixel.wheel(did, RIGHT, 0);
  delay(1);
}
#define DX_A 1
#define DX_B 3
#define DX_C 14
#define DX_D 2

void dx_init() {
  dxl_init_wheel(DX_A, WHEEL, 0, 0);
  delay(100);
  dxl_init_wheel(DX_B, WHEEL, 0, 0);
  delay(100);
  dxl_init_wheel(DX_C, WHEEL, 0, 0);
  delay(100);
  dxl_init_wheel(DX_D, WHEEL, 0, 0);
  delay(100);
}

// REMOTE SETUP
//------------------------------------------------//
#define PIN_AIL 2  // CH 1
#define PIN_ELE 4  // CH 2
#define PIN_THR 8  // CH 3
#define PIN_RUD 9  // CH 4

unsigned long AIL_PULSE_WIDTH;
unsigned long ELE_PULSE_WIDTH;
unsigned long THR_PULSE_WIDTH;
unsigned long RUD_PULSE_WIDTH;

float thr_low = 980, thr_high = 1980;
float ail_low = 980, ail_high = 1980;
float ele_low = 980, ele_high = 1980;
float rud_low = 980, rud_high = 1980;

float D_THR;
float D_AIL;
float D_ELE;
float D_RUD;
float D_SUDUT;

void rc_read() {
  // Read  RC pulses
  THR_PULSE_WIDTH = pulseIn(PIN_THR, HIGH);  //, PULSEIN_TIMEOUT);
  AIL_PULSE_WIDTH = pulseIn(PIN_AIL, HIGH);
  ELE_PULSE_WIDTH = pulseIn(PIN_ELE, HIGH);
  RUD_PULSE_WIDTH = pulseIn(PIN_RUD, HIGH);

  D_THR = ((float)(THR_PULSE_WIDTH - thr_low) / (float)(thr_high - thr_low) * 300);
  if (D_THR < 5) D_THR = 0;
  else if (D_THR > 300) D_THR = 300;

  D_AIL = ((float)(AIL_PULSE_WIDTH - ail_low) / (float)(ail_high - ail_low) * 200) - 100;
  if (D_AIL < 5 && D_AIL > -5) D_AIL = 0;
  else if (D_AIL < -100) D_AIL = -100;
  else if (D_AIL > 100) D_AIL = 100;

  D_ELE = ((float)(ELE_PULSE_WIDTH - ele_low) / (float)(ele_high - ele_low) * 200) - 100;
  if (D_ELE < 5 && D_ELE > -5) D_ELE = 0;
  else if (D_ELE < -100) D_ELE = -100;
  else if (D_ELE > 100) D_ELE = 100;

  D_RUD = ((float)(RUD_PULSE_WIDTH - rud_low) / (float)(rud_high - rud_low) * 100) - 50;
  if (D_RUD < 5 && D_RUD > -5) D_RUD = 0;
  else if (D_RUD < -50) D_RUD = -50;
  else if (D_RUD > 50) D_RUD = 50;

  D_SUDUT = 57.2958 * atan2(D_AIL, D_ELE);

  // Serial.print(D_THR);
  // Serial.print(",");
  // Serial.print(D_AIL);
  // Serial.print(",");
  // Serial.print(D_ELE);
  // Serial.print(",");
  // Serial.print(D_RUD);
  // Serial.print(",");
  Serial.println(D_SUDUT);
}

// MECANUM SETUP
//------------------------------------------------//
// SetWheel Degrees
int DEGA = 45;
int DEGB = 135;
int DEGC = 225;
int DEGD = 315;
float toRADS = 0.0174533;
float MAX_SPEED = 400;

void drive_mecanum(float Vel, float Dir, float Turn) {

  if (Dir < 0) Dir += 360;
  else if (Dir >= 360) Dir -= 360;

  float wa = (cos((DEGA + Dir) * toRADS) * Vel) - Turn;
  float wc = (cos((DEGC + Dir) * toRADS) * Vel) - Turn;
  float wb = (cos((DEGB + Dir) * toRADS) * Vel) + Turn;
  float wd = (cos((DEGD + Dir) * toRADS) * Vel) + Turn;

  if (wa > MAX_SPEED) wa = MAX_SPEED;
  else if (wa < -MAX_SPEED) wa = -MAX_SPEED;
  if (wc > MAX_SPEED) wc = MAX_SPEED;
  else if (wc < -MAX_SPEED) wc = -MAX_SPEED;
  if (wb > MAX_SPEED) wb = MAX_SPEED;
  else if (wb < -MAX_SPEED) wb = -MAX_SPEED;
  if (wd > MAX_SPEED) wd = MAX_SPEED;
  else if (wd < -MAX_SPEED) wd = -MAX_SPEED;

  // Serial.print(", ");
  // Serial.print(wa);
  // Serial.print(" ");
  // Serial.print(-wb);
  // Serial.print(" ");
  // Serial.print(wc);
  // Serial.print(" ");
  // Serial.println(-wd);

  dxlwheel(DX_A, wa);   // wa
  dxlwheel(DX_C, wc);   // wc
  dxlwheel(DX_B, -wb);  // wb
  dxlwheel(DX_D, -wd);  // wd
}

void setup() {
  Serial.begin(9600);
  delay(2000);
  pinMode(12, INPUT);

  dx_init();
  dxlwheel(DX_A, 0);  // wa
  dxlwheel(DX_B, 0);  // wb
  dxlwheel(DX_C, 0);  // wc-
  dxlwheel(DX_D, 0);  // wd
}
int fmode = 1;
int emode = 0;

void loop() {

  delay(20);
  rc_read();
  drive_mecanum(D_THR, D_SUDUT, D_RUD * 2.5);
}
