// PIN DRIVER MOTOR
const unsigned int LP1 = 2;
const unsigned int LA1 = 3;
const unsigned int LB1 = 4;

const unsigned int LP2 = 5;
const unsigned int LA2 = 6;
const unsigned int LB2 = 7;

const unsigned int RP1 = 8;
const unsigned int RA1 = 9;
const unsigned int RB1 = 10;

const unsigned int RP2 = 11;
const unsigned int RA2 = 12;
const unsigned int RB2 = 13;

const unsigned int RL1 = 22;
const unsigned int RL2 = 23;
const unsigned int RL3 = 24;
const unsigned int RL4 = 25;


void init_motor()
{
  pinMode(LP1, OUTPUT);
  pinMode(LA1, OUTPUT);
  pinMode(LB1, OUTPUT);

  pinMode(LP2, OUTPUT);
  pinMode(LA2, OUTPUT);
  pinMode(LB2, OUTPUT);

  pinMode(RP1, OUTPUT);
  pinMode(RA1, OUTPUT);
  pinMode(RB1, OUTPUT);

  pinMode(RP2, OUTPUT);
  pinMode(RA2, OUTPUT);
  pinMode(RB2, OUTPUT);


}

void motor(int M1, int M2, int M3, int M4 )
{
  if (M1 < 0)
  {
    digitalWrite(LA1, LOW);
    digitalWrite(LB1, HIGH);
    analogWrite(LP1, -M1);
  }
  else if (M1 > 0)
  {
    digitalWrite(LA1, HIGH);
    digitalWrite(LB1, LOW);
    analogWrite(LP1, M1);
  }
  else
  {
    digitalWrite(LA1, LOW);
    digitalWrite(LB1, LOW);
    analogWrite(LP1, 0);
  }
  if (M2 < 0)
  {
    digitalWrite(LA2, LOW);
    digitalWrite(LB2, HIGH);
    analogWrite(LP2, -M2);
  }
  else if (M2 > 0)
  {
    digitalWrite(LA2, HIGH);
    digitalWrite(LB2, LOW);
    analogWrite(LP2, M2);
  }
  else
  {
    digitalWrite(LA2, LOW);
    digitalWrite(LB2, LOW);
    analogWrite(LP2, 0);
  }

  if (M3 < 0)
  {
    digitalWrite(RA1, LOW);
    digitalWrite(RB1, HIGH);
    analogWrite(RP1, -M3);
  }
  else if (M3 > 0)
  {
    digitalWrite(RA1, HIGH);
    digitalWrite(RB1, LOW);
    analogWrite(RP1, M3);
  }
  else
  {
    digitalWrite(RA1, LOW);
    digitalWrite(RB1, LOW);
    analogWrite(RP1, 0);
  }

  if (M4 < 0)
  {
    digitalWrite(RA2, LOW);
    digitalWrite(RB2, HIGH);
    analogWrite(RP2, -M4);
  }
  else if (M4 > 0)
  {
    digitalWrite(RA2, HIGH);
    digitalWrite(RB2, LOW);
    analogWrite(RP2, M4);
  }
  else
  {
    digitalWrite(RA2, LOW);
    digitalWrite(RB2, LOW);
    analogWrite(RP2, 0);
  }

}

void init_relay()
{
  pinMode(RL1, OUTPUT);
  pinMode(RL2, OUTPUT);
  pinMode(RL3, OUTPUT);
  pinMode(RL4, OUTPUT);

}

void relay(int pn, bool dt)
{
  digitalWrite(pn, dt);
}

// REMOTE SETUP
//------------------------------------------------//
#define PIN_AIL 26
#define PIN_ELE 27
#define PIN_THR 28
#define PIN_RUD 29
#define PIN_SA 30
#define PIN_SB 31
#define PIN_SC 32
#define PIN_SD 33

unsigned long AIL_PULSE_WIDTH;
unsigned long ELE_PULSE_WIDTH;
unsigned long THR_PULSE_WIDTH;
unsigned long RUD_PULSE_WIDTH;
unsigned long SA_PULSE_WIDTH;
unsigned long SB_PULSE_WIDTH;
unsigned long SC_PULSE_WIDTH;
unsigned long SD_PULSE_WIDTH;

float thr_low = 970, thr_high = 2000;
float ail_low = 970, ail_high = 1981;
float ele_low = 970, ele_high = 1998;
float rud_low = 979, rud_high = 1998;
float sa_low = 979, sa_high = 1998;
float sb_low = 979, sb_high = 1998;
float sc_low = 979, sc_high = 1998;
float sd_low = 979, sd_high = 1998;

float D_THR;
float D_AIL;
float D_ELE;
float D_RUD;
float D_SA;
float D_SB;
float D_SC;
float D_SD;
float D_SUDUT;

void rc_read()
{
  // Read  RC pulses
  THR_PULSE_WIDTH = pulseIn(PIN_THR, HIGH);//, PULSEIN_TIMEOUT);
  AIL_PULSE_WIDTH = pulseIn(PIN_AIL, HIGH);
  ELE_PULSE_WIDTH = pulseIn(PIN_ELE, HIGH);
  RUD_PULSE_WIDTH = pulseIn(PIN_RUD, HIGH);
  SA_PULSE_WIDTH = pulseIn(PIN_SA, HIGH);
  SB_PULSE_WIDTH = pulseIn(PIN_SB, HIGH);
  SC_PULSE_WIDTH = pulseIn(PIN_SC, HIGH);
  SD_PULSE_WIDTH = pulseIn(PIN_SD, HIGH);


  //if (THR_PULSE_WIDTH < thr_low ) D_THR = 0;
  D_THR =  ((float) (THR_PULSE_WIDTH - thr_low) / (float)(thr_high - thr_low ) * 300);
  if (D_THR < 5 )          D_THR = 0;
  else if (D_THR > 300 )  D_THR = 300;

  D_AIL =  ((float) (AIL_PULSE_WIDTH - ail_low) / (float)(ail_high - ail_low ) * 200) - 100;
  if (D_AIL < 5 && D_AIL > -5 )  D_AIL = 0;
  else if (D_AIL < -100 )       D_AIL = -100;
  else if (D_AIL > 100 )        D_AIL =  100;

  D_ELE =  ((float) (ELE_PULSE_WIDTH - ele_low) / (float)(ele_high - ele_low ) * 200) - 100;
  if (D_ELE < 5 && D_ELE > -5 )  D_ELE = 0;
  else if (D_ELE < -100 )       D_ELE = -100;
  else if (D_ELE >  100 )       D_ELE =  100;

  D_RUD =  ((float) (RUD_PULSE_WIDTH - rud_low) / (float)(rud_high - rud_low ) * 100) - 50;
  if (D_RUD < 5 && D_RUD > -5 )  D_RUD = 0;
  else if (D_RUD < -50 )       D_RUD = -50;
  else if (D_RUD >  50 )       D_RUD =  50;

  D_SA =  ((float) (SA_PULSE_WIDTH - sa_low) / (float)(sa_high - sa_low ) * 200) - 100;
  if (D_SA > 50)        D_SA = 1;
  else if (D_SA < -50)  D_SA = -1;
  else                  D_SA = 0;

  D_SB =  ((float) (SB_PULSE_WIDTH - sb_low) / (float)(sb_high - sb_low ) * 200) - 100;
  if (D_SB > 50)        D_SB = 1;
  else if (D_SB < -50)  D_SB = -1;
  else                  D_SB = 0;

  D_SC =  ((float) (SC_PULSE_WIDTH - sc_low) / (float)(sc_high - sc_low ) * 200) - 100;
  if (D_SC > 50)        D_SC = 1;
  else if (D_SC < -50)  D_SC = -1;
  else                  D_SC = 0;

  D_SD =  ((float) (SD_PULSE_WIDTH - sd_low) / (float)(sd_high - sd_low ) * 200) - 100;
  if (D_SD > 50)        D_SD = 1;
  else if (D_SD < -50)  D_SD = -1;
  else                  D_SD = 0;

  D_SUDUT = 57.2958 * atan2 (D_AIL, D_ELE);
}

// MECANUM SETUP
//------------------------------------------------//
// SetWheel Degrees
int DEGA = 45;
int DEGB = 135;
int DEGC = 225;
int DEGD = 315;
float toRADS = 0.0174533;
float MAX_SPEED = 300;

void drive_mecanum(float Vel, float Dir, float Turn)
{

  if (Dir < 0)          Dir += 360;
  else if (Dir >= 360)  Dir -= 360;

  float wa = (cos( (DEGA + Dir) * toRADS ) * Vel) + Turn;
  float wb = (cos( (DEGB + Dir) * toRADS ) * Vel) + Turn;
  float wc = (cos( (DEGC + Dir) * toRADS ) * Vel) + Turn;
  float wd = (cos( (DEGD + Dir) * toRADS ) * Vel) + Turn;

  if (wa > MAX_SPEED) wa = MAX_SPEED; else if (wa < -MAX_SPEED) wa = -MAX_SPEED;
  if (wb > MAX_SPEED) wb = MAX_SPEED; else if (wb < -MAX_SPEED) wb = -MAX_SPEED;
  if (wc > MAX_SPEED) wc = MAX_SPEED; else if (wc < -MAX_SPEED) wc = -MAX_SPEED;
  if (wd > MAX_SPEED) wd = MAX_SPEED; else if (wd < -MAX_SPEED) wd = -MAX_SPEED;

  Serial.print(", ");
  Serial.print(wa); Serial.print(" ");
  Serial.print(wb); Serial.print(" ");
  Serial.print(wc); Serial.print(" ");
  Serial.println(wd);
  motor(wa, wb, wc, wd);
  /*
      dxlwheel(DX_A, wa); // wa
      dxlwheel(DX_B, -wb); // wb
      dxlwheel(DX_C, wc); // wc
      dxlwheel(DX_D, -wd); // wd
  */
}


//#include "Arduino.h"
#include "AX12A.h"

#define DirectionPin   (18u)
#define BaudRate      (1000000ul)
#define ID        (1u)

int initial_pos = 512;
int max = initial_pos + 400;
int min = initial_pos - 400;

int pos = initial_pos;
int delta = 5;

void init_dxl()
{
  ax12a.begin(BaudRate, DirectionPin, &Serial1);
  ax12a.setEndless(ID, OFF);
}

void test_dxl()
{
  pos = pos + delta;

  if (pos > max)
  {
    pos = max;
    delta *= -1;
  }

  if (pos < min)
  {
    pos = min;
    delta *= -1;
  }

  ax12a.move(ID, pos);
  delay(2);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  init_motor();
  init_relay();
  init_dxl();
  delay(2000);
}
int i = 0;
void loop() {
  // test_dxl();
  rc_read();
    Serial.print(" , D_RUD =  ");
    Serial.print(D_RUD);
  drive_mecanum(D_THR , -D_SUDUT, D_RUD);

  /*
    Serial.print("D_THR =  ");
    Serial.print(D_THR);

    Serial.print(" , D_RUD =  ");
    Serial.print(D_RUD);

    Serial.print(" , D_AIL =  ");
    Serial.print(D_AIL);

    Serial.print(" , D_ELE=  ");
    Serial.print(D_ELE);
  */
  /*
    Serial.print(" , D_SUDUT =  ");
    Serial.print(D_SUDUT);

    Serial.print(" , D_SA =  ");
    Serial.print(D_SA);

    Serial.print(" , D_SB =  ");
    Serial.print(D_SB);

    Serial.print(" , D_SC =  ");
    Serial.print(D_SC);

    Serial.print(" , D_SD =  ");
    Serial.println(D_SD);
  */
  /*
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
  /*// put your main code here, to run repeatedly:
    while (i < 255)
    {
    i++;
    motor(i, i, i, i);
      delay(30);
    }
    i = -255;
    motor(0, 0, 0, 0);

    relay(RL1, HIGH); delay(1000);
    relay(RL1, LOW); delay(1000);

    relay(RL2, HIGH); delay(1000);
    relay(RL2, LOW); delay(1000);

    relay(RL3, HIGH); delay(1000);
    relay(RL3, LOW); delay(1000);

    relay(RL4, HIGH); delay(1000);
    relay(RL4, LOW); delay(1000);
  */
}
