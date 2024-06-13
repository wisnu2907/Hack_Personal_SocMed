// wisnu.imam.satrio@gmail.com
// @KRTMI2024

#define pulse_rotation 532  //
#define d_wheel 15          // cm
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

Encoder myEncL(2, 8);
Encoder myEncR(3, 4);

long oldPositionL = -999;
long oldPositionR = -999;

float kp = 1.1;
float kd = 0.4;

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
const unsigned int PWM_CW_L = 10;
const unsigned int PWM_CCW_L = 9;

const unsigned int PWM_CW_R = 5;
const unsigned int PWM_CCW_R = 6;

float mErrL = 0, mErrR = 0;
float mdErrL = 0, mdErrR = 0;
float SpeedL = 0, SpeedR = 0;
float mLastL = 0, mLastR = 0;
int pwmL = 0, pwmR = 0;

float CountL = 0, CountR = 0;

void init_motor() {
  pinMode(PWM_CW_L, OUTPUT);
  pinMode(PWM_CCW_L, OUTPUT);

  pinMode(PWM_CW_R, OUTPUT);
  pinMode(PWM_CCW_R, OUTPUT);
}

void agv_motor(int ll, int rr) {
  if (ll < 0) {
    analogWrite(PWM_CW_L, -ll);
  } else if (ll > 0) {
    analogWrite(PWM_CCW_L, ll);
  } else {
    analogWrite(PWM_CCW_L, 0);
    analogWrite(PWM_CW_L, 0);
  }

  if (rr > 0) {
    analogWrite(PWM_CW_R, rr);
  } else if (rr < 0) {
    analogWrite(PWM_CCW_R, -rr);
  } else {
    analogWrite(PWM_CCW_R, 0);
    analogWrite(PWM_CW_R, 0);
  }
}


void com_agv_motor(int dSL, int dSR) {
  int dRL = 0, dRR = 0;
  delay(13);
  dRL = abs(myEncL.read());
  dRR = abs(myEncR.read());

  CountL += (float)dRL / 11.14;
  CountR += (float)dRR / 11.14;

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

  SpeedL += (float)(mErrL * kp) + (float)(mdErrL * kd);
  SpeedR += (float)(mErrR * kp) + (float)(mdErrR * kd);

  if (SpeedL >= 255) SpeedL = 255;
  if (SpeedR >= 255) SpeedR = 255;

  if (dSL < 0) pwmL = -SpeedL;
  else if (dSL > 0) pwmL = SpeedL;
  else pwmL = 0;
  if (dSR < 0) pwmR = -SpeedR;
  else if (dSR > 0) pwmR = SpeedR;
  else pwmR = 0;

  agv_motor(pwmL, pwmR);
  Serial.println(mErrL);
  // Serial.println(0);
}


void Stop() {
  com_agv_motor(0, 0);
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
TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2;
TaskHandle_t TaskHandle_3;


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);
  init_motor();
  while (!Serial) {
    ;
  }

  xTaskCreate(
    TaskComm, "Comm"  // A name just for humans
    ,
    128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,
    NULL, 0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,
    NULL);

  xTaskCreate(
    TaskMaju1, "Motor", 128  // Stack size
    ,
    NULL, 4  // Priority
    ,
    &TaskHandle_1);

  xTaskCreate(
    TaskLeft, "Comm"  // A name just for humans
    ,
    128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,
    NULL, 3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,
    &TaskHandle_2);

  xTaskCreate(
    TaskMaju2, "Motor", 128  // Stack size
    ,
    NULL, 2  // Priority
    ,
    &TaskHandle_3);
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}


void loop() {
}

String input;
int spdL, spdR;

void TaskComm(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    Serial.println(CountL);

    vTaskDelay(1);
  }
}

void TaskMaju1(void *pvParameters)  // This is a task.
{
  (void)pvParameters;

  for (;;) {
    //Untuk Maju
    vTaskSuspend(TaskHandle_2);  //Suspend Task2/3
    vTaskSuspend(TaskHandle_3);
    com_agv_motor(3, 3);
    Serial.println(CountL);
    if (CountL >= 300) {
      Stop();
      delay(1000);
      CountL = 0;
      CountR = 0;
      vTaskResume(TaskHandle_2);
      vTaskDelete(TaskHandle_1);
    }
  }
}

void TaskLeft(void *pvParameters)  // This is a task.
{
  (void)pvParameters;

  for (;;) {
    //Untuk Maju //Suspend Task2/3
    vTaskSuspend(TaskHandle_3);
    com_agv_motor(3, -3);
    Serial.println(CountL);
    if (CountL >= 36.95) {
      Stop();
      delay(1000);
      CountL = 0;
      CountR = 0;
      vTaskResume(TaskHandle_3);
      vTaskDelete(TaskHandle_2);
    }
  }
}

void TaskMaju2(void *pvParameters)  // This is a task.
{
  (void)pvParameters;

  for (;;) {
    //Untuk Maju
    com_agv_motor(3, 3);
    Serial.println(CountL);
    if (CountL >= 100) {
      Stop();
      delay(1);
      vTaskDelete(TaskHandle_3);
    }
  }
}