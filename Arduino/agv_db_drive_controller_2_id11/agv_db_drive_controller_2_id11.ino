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

void majuKenceng() {
  com_agv_motor(24, 24);
}

void majuKencengToMid() {
  com_agv_motor(27, 27); 
}

void majuPelan() {
  com_agv_motor(9, 9);
} 

void majuPelanToMid() { 
  com_agv_motor(24, 24);
}

void majuNgiri() {
  com_agv_motor(4, -4);
}

void majuNganan() {
  com_agv_motor(-5, 5);
} 

void mundurKenceng() {
  com_agv_motor(-24, -24);
}

void mundurKencengToMid() {
  com_agv_motor(-27, -27);
}
 
void mundurPelan() {
  com_agv_motor(-9 , -9 );
}

void mundurPelanToMid() {
  com_agv_motor(-24, -24);
}

void mundurNgiriToMid() {
  com_agv_motor(0, 0);
}

void mundurNgananToMid() {
  com_agv_motor(0, 0);
}

void mundurNgiri() {
  com_agv_motor(0, 0);
}

void mundurNganan() {
  com_agv_motor(0, 0);
}

void SlideL() {
  com_agv_motor(-40, 40);
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
int spdL, spdR;

void TaskComm(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (Serial.available() > 0) {
      input = Serial.readStringUntil('\n');
      // int spaceIndex = input.indexOf(' ');
      // // CountL = 0, CountR = 0;
      // if (spaceIndex != -1) {
      //  spdL = input.substring(0, spaceIndex).toInt();
      //  spdR = input.substring(spaceIndex + 1).toInt();
      // }
    }

    vTaskDelay(1);
  }
}

void TaskMotor(void *pvParameters)  // This is a task.
{
  (void)pvParameters;

  for (;;) {
    //Untuk Maju
    if (input == "a") majuKenceng();
    else if (input == "e") majuKencengToMid();
    else if (input == "b") majuPelan();
    else if (input == "f") majuPelanToMid();
    else if (input == "c") majuNgiri();
    else if (input == "d") majuNganan();

    //Untuk Mundur
    else if (input == "1") mundurKenceng();
    else if (input == "5") mundurKencengToMid();
    else if (input == "2") mundurPelan();
    else if (input == "6") mundurPelanToMid();
    else if (input == "3") mundurNgiri();
    else if (input == "7") mundurNgiriToMid();
    else if (input == "4") mundurNganan();
    else if (input == "8") mundurNgananToMid();

    //Untuk Stop
    else if (input == "0") Stop();

    //Untuk Start
    else if (input == "L") {
      SlideL();
      if (CountL >= 95 || CountR >= 95) {
        Stop();
      }
    } else Stop();

    vTaskDelay(1);
  }
}
