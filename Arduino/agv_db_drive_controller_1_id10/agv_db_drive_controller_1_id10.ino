// rodikwahyuindrawan@gmail.com
// @awgrobotic2023

#define pulse_rotation 1988  //
#define d_wheel 15           // cm
#define k_wheel (3.14 * 15)

/*
   range speed -50 0 50
    0 = enable
    1 = speed L
    2 = speed R
    3 = reset counter
    4 = counter L
    5 = counter R
*/
//-----------------------------------------

//-----------------------------------------

#include <Encoder.h>

#define ledbuiltin 13

Encoder myEncL(3, 4);
Encoder myEncR(2, 8);

long oldPositionL = -999;
long oldPositionR = -999;

void read_encoder() {
  long newPositionL = myEncL.read();
  long newPositionR = myEncR.read();
  if (newPositionL != oldPositionL || newPositionR != oldPositionR) {
    oldPositionL = newPositionL;
    oldPositionR = newPositionR;
    Serial.print(newPositionL);
    Serial.print('\t');
    Serial.println(newPositionR);  // Serial.print('\t');
  }
}

// PIN DRIVER MOTOR
const unsigned int DIR_L = 6;
const unsigned int PWM_L = 5;

const unsigned int DIR_R = 9;
const unsigned int PWM_R = 10;

float mErrL = 0, mErrR = 0;
float mdErrL = 0, mdErrR = 0;
float SpeedL = 0, SpeedR = 0;
float mLastL = 0, mLastR = 0;
int pwmL = 0, pwmR = 0;

float CountL = 0, CountR = 0;

void init_motor() {
  pinMode(DIR_L, OUTPUT);
  pinMode(PWM_L, OUTPUT);

  pinMode(DIR_R, OUTPUT);
  pinMode(PWM_R, OUTPUT);
}

void agv_motor(int ll, int rr) {
  digitalWrite(13, HIGH);
  if (ll < 0) {
    digitalWrite(DIR_L, LOW);
    analogWrite(PWM_L, -ll);
  } else if (ll > 0) {
    digitalWrite(DIR_L, HIGH);
    analogWrite(PWM_L, ll);
  } else {
    analogWrite(PWM_L, 0);
  }

  if (rr < 0) {
    digitalWrite(DIR_R, HIGH);
    analogWrite(PWM_R, -rr);
  } else if (rr > 0) {
    digitalWrite(DIR_R, LOW);
    analogWrite(PWM_R, rr);
  } else {
    analogWrite(PWM_R, 0);
  }
}


void com_agv_motor(int dSL, int dSR) {
  int dRL = 0, dRR = 0;
  delay(13);
  dRL = abs(myEncL.read());
  dRR = abs(myEncR.read());

  CountL += (float)dRL / 42.2;
  CountR += (float)dRR / 42.2;

  // Serial.print(CountL);
  // Serial.println(CountR);

  myEncL.write(0);
  myEncR.write(0);

  mErrL = abs(dSL) - dRL;
  mErrR = abs(dSR) - dRR;

  mdErrL = mErrL - mLastL;
  mLastL = mErrL;


  mdErrR = mErrR - mLastR;
  mLastR = mErrR;

  SpeedL += (float)(mErrL * 0.3) + (float)(mdErrL * 0.4);
  SpeedR += (float)(mErrR * 0.3) + (float)(mdErrR * 0.4);

  if (SpeedL >= 255) SpeedL = 255;
  if (SpeedR >= 255) SpeedR = 255;

  if (dSL < 0) pwmL = -SpeedL;
  else if (dSL > 0) pwmL = SpeedL;
  else pwmL = 0;
  if (dSR < 0) pwmR = -SpeedR;
  else if (dSR > 0) pwmR = SpeedR;
  else pwmR = 0;

  agv_motor(pwmL, pwmR);
}

void Forward() {
  mErrL = 0;
  mErrR = 0;
  mdErrL = 0;
  mdErrR = 0;
  SpeedL = 0;
  SpeedR = 0;
  mLastL = 0;
  mLastR = 0;
  CountL = 0, CountR = 0;
  delay(50);
  com_agv_motor(6, 6);
}

void Backward() {
  mErrL = 0;
  mErrR = 0;
  mdErrL = 0;
  mdErrR = 0;
  SpeedL = 0;
  SpeedR = 0;
  mLastL = 0;
  mLastR = 0;
  CountL = 0, CountR = 0;
  delay(50);
  com_agv_motor(-6, -6);
}

void Stop() {
  com_agv_motor(0, 0);
  analogWrite(PWM_L, 0);
  analogWrite(PWM_R, 0);
  mErrL = 0;
  mErrR = 0;
  mdErrL = 0;
  mdErrR = 0;
  SpeedL = 0;
  SpeedR = 0;
  mLastL = 0;
  mLastR = 0;
  CountL = 0, CountR = 0;
  delay(50);
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1);
  init_motor();
  pinMode(13, OUTPUT);
}

void loop() {
  //  Gas (20, 150);
  if (Serial.available() > 0) {
    mErrL = 0;
    mErrR = 0;
    mdErrL = 0;
    mdErrR = 0;
    SpeedL = 0;
    SpeedR = 0;
    mLastL = 0;
    mLastR = 0;
    CountL = 0, CountR = 0;
    String input = Serial.readStringUntil('\n');
    if (input == "1") {
      Forward();
    } else if (input == "2") {
      Backward();
    } else if (input == "0") {
      Stop();
    }
  }
}