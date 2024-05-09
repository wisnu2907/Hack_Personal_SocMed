//#include <mcd.h>

/*
   KRTMI UNAIR 2021
*/
//--------------------------------------------

#define port_buzzer 13

void buzzer(int num, int ton, int toff) {
  for (int i = 0; i < num; i++) {
    digitalWrite(port_buzzer, HIGH);
    delay(ton);
    digitalWrite(port_buzzer, LOW);
    delay(toff);
  }
}
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
#define DX_5 5
#define DX_6 6
#define DX_7 7
#define DX_8 8
#define DX_9 9

void dx_init() {
  dxl_init_wheel(DX_A, WHEEL, 0, 0);
  delay(100);
  dxl_init_wheel(DX_B, WHEEL, 0, 0);
  delay(100);
  dxl_init_wheel(DX_C, WHEEL, 0, 0);
  delay(100);
  dxl_init_wheel(DX_D, WHEEL, 0, 0);
  delay(100);
  dxl_init(DX_5, SERVO, 1, 1024);
  delay(100);
  dxl_init(DX_6, SERVO, 1, 1024);
  delay(100);
  dxl_init(DX_7, SERVO, 1, 1024);
  delay(100);
  dxl_init(DX_8, SERVO, 1, 1024);
  delay(100);
  dxl_init(DX_9, SERVO, 1, 1024);
  delay(100);
}

// REMOTE SETUP
//------------------------------------------------//
#define PIN_AIL 2
#define PIN_ELE 4
#define PIN_THR 8
#define PIN_RUD 9
#define PIN_SA 10
#define PIN_SD A3

unsigned long AIL_PULSE_WIDTH;
unsigned long ELE_PULSE_WIDTH;
unsigned long THR_PULSE_WIDTH;
unsigned long RUD_PULSE_WIDTH;
unsigned long SA_PULSE_WIDTH;
unsigned long SD_PULSE_WIDTH;

float thr_low = 970, thr_high = 2000;
float ail_low = 970, ail_high = 1981;
float ele_low = 970, ele_high = 1998;
float rud_low = 979, rud_high = 1998;
float sa_low = 979, sa_high = 1998;
float sd_low = 979, sd_high = 1998;

float D_THR;
float D_AIL;
float D_ELE;
float D_RUD;
float D_SA;
float D_SD;
float D_SUDUT;

void rc_read() {
  // Read  RC pulses
  THR_PULSE_WIDTH = pulseIn(PIN_THR, HIGH);  //, PULSEIN_TIMEOUT);
  AIL_PULSE_WIDTH = pulseIn(PIN_AIL, HIGH);
  ELE_PULSE_WIDTH = pulseIn(PIN_ELE, HIGH);
  RUD_PULSE_WIDTH = pulseIn(PIN_RUD, HIGH);
  SA_PULSE_WIDTH = pulseIn(PIN_SA, HIGH);
  SD_PULSE_WIDTH = pulseIn(PIN_SD, HIGH);

  //if (THR_PULSE_WIDTH < thr_low ) D_THR = 0;
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

  D_SA = ((float)(SA_PULSE_WIDTH - sa_low) / (float)(sa_high - sa_low) * 200) - 100;
  if (D_SA > 50) D_SA = 1;
  else if (D_SA < -50) D_SA = -1;
  else D_SA = 0;

  D_SD = ((float)(SD_PULSE_WIDTH - sd_low) / (float)(sd_high - sd_low) * 200) - 100;
  if (D_SD > 0) D_SD = 1;
  else D_SD = -1;

  D_SUDUT = 57.2958 * atan2(D_AIL, D_ELE);
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
  /*
    Serial.print(", ");
    Serial.print(wa); Serial.print(" ");
    Serial.print(-wb); Serial.print(" ");
    Serial.print(-wc); Serial.print(" ");
    Serial.println(wd);
  */
  dxlwheel(DX_A, wa);   // wa
  dxlwheel(DX_C, wc);   // wc
  dxlwheel(DX_B, -wb);  // wb
  dxlwheel(DX_D, -wd);  // wd
}
int flag = 0;
int flag_down = 0;
int flag_angkat = 0;
void setup() {
  Serial.begin(9600);
  delay(2000);
  pinMode(12, INPUT);
  buzzer(2, 50, 50);

  dx_init();
  //dxlservo(DX_9, 512 - 30 , 50);
  //dxlservo(DX_5, 512 + 320, 50);
  dxlservo(DX_9, 512 - 15, 20);
  dxlservo(DX_5, 512 + 300, 100);

  dxlwheel(DX_A, 0);  // wa
  dxlwheel(DX_B, 0);  // wb
  dxlwheel(DX_C, 0);  // wc
  dxlwheel(DX_D, 0);  // wd
  buzzer(2, 50, 50);

  //dxlservo(DX_6, 512, 50);
  dxlservo(DX_6, 512 + 305, 40);
  dxlservo(DX_7, 512, 50);
  dxlservo(DX_8, 512, 50);
  // standby
  //  dxlservo(DX_5, 512 - 20, 50);
  //  dxlservo(DX_6, 512, 50);
  //  dxlservo(DX_7, 512 - 170, 50);
  //  dxlservo(DX_8, 512 + 300, 500);
  //
  //  flag = 0;
}
int fmode = 1;
void loop() {
  delay(20);
  rc_read();
  drive_mecanum(D_THR, D_SUDUT, D_RUD * 2.5);
  //  drive_mecanum(D_THR / 1.75, D_SUDUT, D_RUD * 2.5);
  if (D_SD == 1) {
    if (D_SA == 1) {
      // reset standby
      if (flag == 1 || fmode == 1)  // meletakkan object
      {
        dxlservo(DX_7, 512 - 100, 400);
        dxlservo(DX_8, 512 + 40, 400);
        for (int i = 0; i < 10; i++) {
          rc_read();
          drive_mecanum(D_THR, D_SUDUT, D_RUD * 2.5);
          delay(10);
        }
        dxlservo(DX_5, 512 + 210, 50);
        dxlservo(DX_9, 512 - 5, 50);  // 50
        //  dxlservo(DX_9, 512 - 100 , 200);


        // dxlservo(DX_7, 512 + 40 , 400);
        dxlservo(DX_8, 512 - 40, 400);
        for (int i = 0; i < 10; i++) {
          rc_read();
          drive_mecanum(D_THR, D_SUDUT, D_RUD * 2.5);
          delay(10);
        }
        dxlservo(DX_6, 512 + 305, 400);
        //dxlservo(DX_7, 512 , 500);
        dxlservo(DX_8, 512 + 40, 500);
      }
      dxlservo(DX_7, 512 - 35, 100);
      dxlservo(DX_8, 512, 500);

      //dxlservo(DX_9, 512 - 30 , 20);
      //dxlservo(DX_5, 512 + 320, 100);

      dxlservo(DX_9, 512 - 15, 20);
      dxlservo(DX_5, 512 + 300, 100);
      flag = 0;
      flag_angkat = 0;


    } else if (D_SA == -1) {
      //putar
      dxlservo(DX_6, 512, 200);
      //dxlservo(DX_5, 512 + 300, 30);
      // dxlservo(DX_9, 512 + 30 , 50);
      // dxlservo(DX_5, 512 + 250, 50);

      dxlservo(DX_9, 512 + 44, 50);
      dxlservo(DX_5, 512 + 230, 50);
      flag = 1;
      delay(20);
      // buzzer(2, 50, 50);
    } else if (D_SA == 0 && flag == 0) {
      buzzer(1, 50, 50);
      dxlservo(DX_7, 512 + 40, 550);
      if (flag_down == 1) {
        // standby
        dxlservo(DX_9, 512 - 30, 50);
        dxlservo(DX_5, 512 + 320, 50);
        dxlservo(DX_6, 512 + 310, 50);  //
        dxlservo(DX_7, 512, 500);
        dxlservo(DX_8, 512, 500);
        delay(20);
        flag_down = 0;
      }
      // jepit
      /*      if (flag_angkat == 0)
            {
              dxlservo(DX_9, 512 + 15 , 40);
              dxlservo(DX_5, 512 + 240, 60);
              delay(200);
            }
      */
      dxlservo(DX_7, 512 + 60, 650);
      dxlservo(DX_8, 512 - 60, 400);
      if (flag_angkat == 0) {
        delay(600);
        flag_angkat = 1;
      }
      // angkat

      dxlservo(DX_9, 512 - 50, 50);
      for (int i = 0; i < 5; i++) {
        rc_read();
        drive_mecanum(D_THR, D_SUDUT, D_RUD * 2.5);
        delay(10);
      }
      dxlservo(DX_5, 512 + 260, 50);

      // dxlservo(DX_5, 512 + 260, 50);
      // dxlservo(DX_9, 512 - 50 , 50);
    }
    fmode = 0;
  }

  else {
    if (D_SA == 1) {

      if (fmode == 0) {
        dxlservo(DX_7, 512 + 100, 400);
        dxlservo(DX_8, 512 - 100, 400);
        for (int i = 0; i < 10; i++) {
          rc_read();
          drive_mecanum(D_THR, D_SUDUT, D_RUD * 2.5);
          delay(10);
        }
        dxlservo(DX_6, 512, 300);
        for (int i = 0; i < 20; i++) {
          rc_read();
          drive_mecanum(D_THR, D_SUDUT, D_RUD * 2.5);
          delay(10);
        }
      }

      dxlservo(DX_6, 512, 300);
      dxlservo(DX_7, 512 - 40, 50);
      dxlservo(DX_8, 512 + 40, 500);

      flag = 0;
      delay(20);

    } else if (D_SA == -1) {
      // jepit
      dxlservo(DX_7, 512 + 50, 450);
      dxlservo(DX_8, 512 - 50, 650);
      if (flag == 0) delay(600);
      // Angkat
      // dxlservo(DX_9, 512 - 45 , 190);
      // dxlservo(DX_5, 512 + 300, 145);

      dxlservo(DX_9, 512 + 44, 190);
      dxlservo(DX_5, 512 + 230, 170);
      flag = 1;


    } else if (D_SA == 0 && flag == 0) {
      // standby
      dxlservo(DX_9, 512 + 300, 200);
      dxlservo(DX_5, 512, 200);
      dxlservo(DX_6, 512, 300);
      dxlservo(DX_7, 512, 100);
      dxlservo(DX_8, 512, 500);
      delay(20);
      flag_down = 1;
    }
    fmode = 1;
  }
}

/*
  delay(1500);
  buzzer(1, 80, 100);
  dxlwheel(DX_A, 100); // wa
  dxlwheel(DX_B, 100); // -wb
  dxlwheel(DX_C, -100); // wc
  dxlwheel(DX_D, -100); // -wd

  delay(1500);
  buzzer(2, 80, 100);
  dxlwheel(DX_A, -100); // wa
  dxlwheel(DX_B, 100); // -wb
  dxlwheel(DX_C, 100); // wc
  dxlwheel(DX_D, -100); // -wd
  delay(1500);
  buzzer(1, 80, 100);
  dxlwheel(DX_A, -100); // wa
  dxlwheel(DX_B, -100); // -wb
  dxlwheel(DX_C, 100); // wc
  dxlwheel(DX_D, 100); // -wd
  delay(1500);
  buzzer(2, 80, 100);
  dxlwheel(DX_A, 100); // wa
  dxlwheel(DX_B, -100); // -wb
  dxlwheel(DX_C, -100); // wc
  dxlwheel(DX_D, 100); // -wd*/

/*
      Serial.print("D_THR =  ");
      Serial.print(D_THR);

      Serial.print(" , D_RUD =  ");
      Serial.print(D_RUD);

       Serial.print(" , D_AIL =  ");
       Serial.print(D_AIL);

       Serial.print(" , D_ELE=  ");
       Serial.print(D_ELE);

       Serial.print(" , D_SUDUT =  ");
       Serial.print(D_SUDUT);

       Serial.print(" , D_SA =  ");
       Serial.println(D_SA);

       Serial.print(" ELE pulse = ");
       Serial.print(ELE_PULSE_WIDTH);
       Serial.print(" , D_ELE =  ");
       Serial.print(D_ELE);

       Serial.print(" ; RUD pulse = ");
       Serial.print(RUD_PULSE_WIDTH);
       Serial.print(" , D_RUD =  ");
       Serial.print(D_RUD);

       Serial.print(" ; SA pulse = ");
       Serial.print(SA_PULSE_WIDTH);
       Serial.print(" , D_SA =  ");
       Serial.println(D_SA);


*/
