// wisnu.imam.satrio@gmail.com
// @KRTMI2024

#define pulse_rotation 1988  //
#define d_wheel 15           // cm
#define k_wheel (3.14 * 15)
#define spd1 5
#define spd2 7
#define spd3 8
#define spd4 9
#define spd5 26

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

  CountL += (float)dRL / 49.852;
  CountR += (float)dRR / 49.852;

  //42.2 = pulse_rotation/(k_wheel)

  // Serial.print(CountL);
  // Serial.print("\t");
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

//----------FORWARD----------//
void F1() {
  com_agv_motor(spd1, spd1);
}

void F2() {
  com_agv_motor(spd2, spd2);
}
void F3() {
  com_agv_motor(spd3, spd3);
}

void F4() {
  com_agv_motor(spd4, spd4);
}

void F5() {
  com_agv_motor(spd5, spd5);
}

//----------BACKWARD----------//
void Ba1() {
  com_agv_motor(-spd1, -spd1);
}

void B2() {
  com_agv_motor(-spd2, -spd2);
}

void B3() {
  com_agv_motor(-spd3, -spd3);
}

void B4() {
  com_agv_motor(-spd4, -spd4);
}

void B5() {
  com_agv_motor(-spd5, -spd5);
}

void SlideL() {
  com_agv_motor(-11, 11);
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
  delay(13);
}

#include <Arduino_FreeRTOS.h>


// define two tasks for Blink & AnalogRead
void TaskComm(void *pvParameters);
void TaskMotor(void *pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);
  init_motor();
  pinMode(13, OUTPUT);



  while (!Serial) {
    ;
  }

  xTaskCreate(
    TaskComm, "Comm"  // A name just for humans
    ,
    128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,
    NULL, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,
    NULL);

  xTaskCreate(
    TaskMotor, "Motor", 128  // Stack size
    ,
    NULL, 1  // Priority
    ,
    NULL);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}


void loop() {
}

String input;

void TaskComm(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (Serial.available() > 0) {
      input = Serial.readStringUntil('\n');
      CountL = 0, CountR = 0;
    }

    vTaskDelay(1);
  }
}

void TaskMotor(void *pvParameters)  // This is a task.
{
  (void)pvParameters;

  for (;;) {

    if (input == "1") F1();
    else if (input == "2") F2();
    else if (input == "3") F3();
    else if (input == "4") F4();
    else if (input == "5") F5();

    else if (input == "A") Ba1();
    else if (input == "B") B2();
    else if (input == "C") B3();
    else if (input == "D") B4();
    else if (input == "E") B5();

    else if (input == "0") Stop();

    else if (input == "6") {
      SlideL();
      if (CountL >= 95 || CountR >= 95) {
        Stop();
      }
      // one tick delay (15ms) in between reads for stability
    } else Stop();
    vTaskDelay(1);
  }
}
