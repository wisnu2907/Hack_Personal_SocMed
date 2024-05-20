//#include <mcd.h>



/*
   KRTMI UNAIR 2022
*/

#define BK_10 2048 - 200
#define BK_9 2048 - 100
#define BK_5 512
#define BK_6 512
#define BK_7 512 + 60  // jepit
#define BK_8 512 - 60  // jepit

#define ST_10 2048 - 420 + 50
#define ST_9 2048 + 120 + 50
#define ST_5 512 + 140
#define ST_6 512 + 310
#define ST_7 512
#define ST_8 512
//--------------------------------------------

#define PIN_IR A0
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
// #include "Dynamixel_Serial.h"
// #define dxl_id 0x01
// #define dxl_controlpin 7
// #define dxl_baudrate 1000000
// void dxl_init(int did, int mode, int cwlimit, int ccwlimit) {
//   Dynamixel.begin(dxl_baudrate, dxl_controlpin);
//   Dynamixel.setMode(did, mode, cwlimit, ccwlimit);
// }
// void dxl_init_wheel(int did, int mode, int cwlimit, int ccwlimit) {
//   Dynamixel.begin(dxl_baudrate, dxl_controlpin);
//   Dynamixel.setMode(did, WHEEL, 0, 0);
// }
// void dxlservo(int did, int dpos, int dspeed) {
//   Dynamixel.servo(did, dpos, dspeed);
//   delay(1);
// }
// void dxlsetid(int did) {
//   Dynamixel.begin(dxl_baudrate, dxl_controlpin);
//   Dynamixel.setID(BROADCAST_ID, did);
// }
// void dxlwheel(int did, int dspeed) {
//   if (dspeed < 0) Dynamixel.wheel(did, LEFT, -dspeed);
//   else if (dspeed > 0) Dynamixel.wheel(did, RIGHT, dspeed);
//   else Dynamixel.wheel(did, RIGHT, 0);
//   delay(1);
// }
// #define DX_A 1
// #define DX_B 3
// #define DX_C 14
// #define DX_D 2
// #define DX_5 5
// #define DX_6 6
// #define DX_7 7
// #define DX_8 8
// #define DX_9 9
// #define DX_10 10

// void dx_init() {
//   dxl_init_wheel(DX_A, WHEEL, 0, 0);
//   delay(100);
//   dxl_init_wheel(DX_B, WHEEL, 0, 0);
//   delay(100);
//   dxl_init_wheel(DX_C, WHEEL, 0, 0);
//   delay(100);
//   dxl_init_wheel(DX_D, WHEEL, 0, 0);
//   delay(100);
//   dxl_init(DX_5, SERVO, 1, 1024);
//   delay(100);
//   dxl_init(DX_6, SERVO, 1, 1024);
//   delay(100);
//   dxl_init(DX_7, SERVO, 1, 1024);
//   delay(100);
//   dxl_init(DX_8, SERVO, 1, 1024);
//   delay(100);
//   dxl_init(DX_9, SERVO, 1, 4095);
//   delay(100);
//   dxl_init(DX_10, SERVO, 1, 4095);
//   delay(100);
// }

// // REMOTE SETUP
// //------------------------------------------------//
// #define PIN_AIL 2
// #define PIN_ELE 4
// #define PIN_THR 8
// #define PIN_RUD 9
// #define PIN_SA A1
// #define PIN_SB A2
// #define PIN_SC A3
// #define PIN_SD 10

// unsigned long AIL_PULSE_WIDTH;
// unsigned long ELE_PULSE_WIDTH;
// unsigned long THR_PULSE_WIDTH;
// unsigned long RUD_PULSE_WIDTH;
// unsigned long SA_PULSE_WIDTH;
// unsigned long SB_PULSE_WIDTH;
// unsigned long SC_PULSE_WIDTH;
// unsigned long SD_PULSE_WIDTH;

// float thr_low = 980, thr_high = 1980;
// float ail_low = 980, ail_high = 1980;
// float ele_low = 980, ele_high = 1980;
// float rud_low = 980, rud_high = 1980;
// float sa_low = 980, sa_high = 1998;
// float sb_low = 980, sb_high = 1998;
// float sc_low = 980, sc_high = 1998;
// float sd_low = 980, sd_high = 1998;

// float D_THR;
// float D_AIL;
// float D_ELE;
// float D_RUD;
// float D_SA;
// float D_SB;
// float D_SC;
// float D_SD;
// float D_SUDUT;

// void rc_read()
// {
//   // Read  RC pulses
//   THR_PULSE_WIDTH = pulseIn(PIN_THR, HIGH);//, PULSEIN_TIMEOUT);
//   AIL_PULSE_WIDTH = pulseIn(PIN_AIL, HIGH);
//   ELE_PULSE_WIDTH = pulseIn(PIN_ELE, HIGH);
//   RUD_PULSE_WIDTH = pulseIn(PIN_RUD, HIGH);
//   SA_PULSE_WIDTH = pulseIn(PIN_SA, HIGH);
//   SB_PULSE_WIDTH = pulseIn(PIN_SB, HIGH);
//   SC_PULSE_WIDTH = pulseIn(PIN_SC, HIGH);
//   SD_PULSE_WIDTH = pulseIn(PIN_SD, HIGH);

//   D_THR =  ((float) (THR_PULSE_WIDTH - thr_low) / (float)(thr_high - thr_low ) * 300);
//   if (D_THR < 5 )          D_THR = 0;
//   else if (D_THR > 300 )  D_THR = 300;

//   D_AIL =  ((float) (AIL_PULSE_WIDTH - ail_low) / (float)(ail_high - ail_low ) * 200) - 100;
//   if (D_AIL < 5 && D_AIL > -5 )  D_AIL = 0;
//   else if (D_AIL < -100 )       D_AIL = -100;
//   else if (D_AIL > 100 )        D_AIL =  100;

//   D_ELE =  ((float) (ELE_PULSE_WIDTH - ele_low) / (float)(ele_high - ele_low ) * 200) - 100;
//   if (D_ELE < 5 && D_ELE > -5 )  D_ELE = 0;
//   else if (D_ELE < -100 )       D_ELE = -100;
//   else if (D_ELE >  100 )       D_ELE =  100;

//   D_RUD =  ((float) (RUD_PULSE_WIDTH - rud_low) / (float)(rud_high - rud_low ) * 100) - 50;
//   if (D_RUD < 5 && D_RUD > -5 )  D_RUD = 0;
//   else if (D_RUD < -50 )       D_RUD = -50;
//   else if (D_RUD >  50 )       D_RUD =  50;

//   D_SA =  ((float) (SA_PULSE_WIDTH - sa_low) / (float)(sa_high - sa_low ) * 200) - 100;
//   if (D_SA > 50)        D_SA = -1;
//   else if (D_SA < -50)  D_SA = 1;
//   else                  D_SA = 0;

//   D_SB =  ((float) (SB_PULSE_WIDTH - sb_low) / (float)(sb_high - sb_low ) * 200) - 100;
//   if (D_SB > 50)        D_SB = -1;
//   else if (D_SB < -50)  D_SB = 1;
//   else                  D_SB = 0;

//   D_SC =  ((float) (SC_PULSE_WIDTH - sc_low) / (float)(sc_high - sc_low ) * 200) - 100;
//   if (D_SC > 0)        D_SC = 1;
//   else                 D_SC = 0;

//   D_SD =  ((float) (SD_PULSE_WIDTH - sd_low) / (float)(sd_high - sd_low ) * 200) - 100;
//   if (D_SD > 0)        D_SD = 1;
//   else                 D_SD = 0;



//   D_SUDUT = 57.2958 * atan2 (D_AIL, D_ELE);
/*
    Serial.print(THR_PULSE_WIDTH);Serial.print(",");
    Serial.print(AIL_PULSE_WIDTH);Serial.print(",");
    Serial.print(ELE_PULSE_WIDTH);Serial.print(",");
    Serial.print(RUD_PULSE_WIDTH);Serial.print(",");
    Serial.print(SA_PULSE_WIDTH);Serial.print(",");
    Serial.print(SB_PULSE_WIDTH);Serial.print(",");
    Serial.print(SC_PULSE_WIDTH);Serial.print(",");
    Serial.print(SD_PULSE_WIDTH);Serial.println(",");*/
/*
    Serial.print(D_THR);Serial.print(",");
    Serial.print(D_AIL);Serial.print(",");
    Serial.print(D_ELE);Serial.print(",");
    Serial.print(D_RUD);Serial.print(",");
    Serial.print(D_SA);Serial.print(",");
    Serial.print(D_SB);Serial.print(",");
    Serial.print(D_SC);Serial.print(",");
    Serial.print(D_SD);Serial.println(",");
  */


// MECANUM SETUP
//------------------------------------------------//
// SetWheel Degrees
int DEGA = 45;
int DEGB = 135;
int DEGC = 225;
int DEGD = 315;
float toRADS = 0.0174533;
float MAX_SPEED = 400;
int Vel = 11;
int Turn = 0;
void drive_mecanum(float Dir) {

  if (Dir < 0) Dir += 360;
  else if (Dir >= 360) Dir -= 360;

  float wa = (cos((DEGA + Dir) * toRADS) * Vel) - Turn;
  float wc = (cos((DEGC + Dir) * toRADS) * Vel) - Turn;
  float wb = (cos((DEGB + Dir) * toRADS) * Vel) + Turn;
  float wd = (cos((DEGD + Dir) * toRADS) * Vel) + Turn;

  // if (wa > MAX_SPEED) wa = MAX_SPEED;
  // else if (wa < -MAX_SPEED) wa = -MAX_SPEED;
  // if (wc > MAX_SPEED) wc = MAX_SPEED;
  // else if (wc < -MAX_SPEED) wc = -MAX_SPEED;
  // if (wb > MAX_SPEED) wb = MAX_SPEED;
  // else if (wb < -MAX_SPEED) wb = -MAX_SPEED;
  // if (wd > MAX_SPEED) wd = MAX_SPEED;
  // else if (wd < -MAX_SPEED) wd = -MAX_SPEED;

  Serial.print(", ");
  Serial.print(wa);
  Serial.print(" ");
  Serial.print(wc);
  Serial.print(" ");
  Serial.print(wb);
  Serial.print(" ");
  Serial.println(wd);

  // dxlwheel(DX_A, wa); // wa
  // dxlwheel(DX_C, wc); // wc
  // dxlwheel(DX_B, -wb); // wb
  // dxlwheel(DX_D, -wd); // wd
}

// void d_delay(int dt)
// {
//   for (int i = 0; i < dt; i++)
//   {
//     rc_read();
//     drive_mecanum(D_THR , D_SUDUT, D_RUD * 2.5);
//     delay(10);
//   }
// }
int flag = 0;
int flag_down = 0;
int flag_angkat = 0;
void setup() {
  Serial.begin(9600);
  delay(2000);
  pinMode(12, INPUT);
  pinMode(PIN_IR, INPUT);

  // dx_init();
  // dxlwheel(DX_A, 0);  // wa
  // dxlwheel(DX_B, 0);  // wb
  // dxlwheel(DX_C, 0);  // wc
  // dxlwheel(DX_D, 0);  // wd

  // dxlservo(DX_6, ST_6, 20);
  // dxlservo(DX_10, BK_10, 20);
  // dxlservo(DX_9, BK_9, 20);
  // dxlservo(DX_5, BK_5 - 10, 100);
  // dxlservo(DX_7, 512 + 280, 50);
  // dxlservo(DX_8, 512 - 280, 50);
  buzzer(2, 50, 50);
  //   for (int i = 0; i < 10; i++)
  //     while (1)
  //     {
  //       rc_read();
  //       if (D_SB == 1 && D_SA == 1) break;
  //     }
  //   buzzer(3, 50, 50);
}
int fmode = 1;
int emode = 0;

int ANGKAT = 0;
int JEPIT = 0;
int JEPIT_OTO = 0;
int PUTAR = 0;
int M_ATAS = 1;
int M_TENGAH = 1;
int ANGKAT_BAWAH = 0;
int JEPIT_BAWAH = 0;
int JEPIT_SAMPING = 0;
int M_BAWAH = 0;

void loop() {

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    float x = input.toFloat();
    drive_mecanum(x);
  }
}

// void jepit_oto()
// {
//   // jepit
//   dxlservo(DX_7, ST_7 + 60, 300);
//   dxlservo(DX_8, ST_8 - 60, 300);
//   d_delay(5);
//   dxlservo(DX_10, ST_10 + 150, 150);
//   dxlservo(DX_9, ST_9 + 280, 200);
//   if (!ANGKAT) {
//     dxlservo(DX_5, ST_5 + 35, 80);
//     d_delay(5);
//     buzzer(2, 20, 20);
//     ANGKAT = 1;
//   }
//   dxlservo(DX_5, ST_5 - 20 , 80);
//   dxlservo(DX_10, ST_10 + 150 + 180, 200);
//   dxlservo(DX_9, ST_9 + 280 + 180, 250);
//   JEPIT_OTO = 1;
//   JEPIT = 1;
// }

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
