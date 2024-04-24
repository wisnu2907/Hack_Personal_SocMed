// rodikwahyuindrawan@gmail.com
// @awgrobotic2023

#define pulse_rotation 1988 //
#define d_wheel 15 // cm
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

#include <ModbusRtu.h>

// data array for modbus network sharing
uint16_t data_comm[6] = { 0, 0, 0, 0, 0, 0 };

Modbus slave(10, Serial, 0); // this is slave @10 and RS-232 or USB-FTDI

//-----------------------------------------

#include <Encoder.h>

Encoder myEncL(3, 4);
Encoder myEncR(2, 8);

long oldPositionL  = -999;
long oldPositionR  = -999;

void read_encoder()
{
  long newPositionL = myEncL.read();
  long newPositionR = myEncR.read();
  if (newPositionL != oldPositionL || newPositionR != oldPositionR) {
    oldPositionL = newPositionL;
    oldPositionR = newPositionR;
    Serial.print(newPositionL); Serial.print('\t');
    Serial.println(newPositionR); // Serial.print('\t');
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
float mLastL = 0,  mLastR = 0;
int pwmL = 0, pwmR = 0;

float CountL = 0, CountR = 0;

void init_motor()
{
  pinMode(DIR_L, OUTPUT);
  pinMode(PWM_L, OUTPUT);

  pinMode(DIR_R, OUTPUT);
  pinMode(PWM_R, OUTPUT);
}

void agv_motor(int ll, int rr)
{
  if (data_comm[0] == 1)
  {
    if (ll < 0)
    {
      digitalWrite(DIR_L, LOW);
      analogWrite(PWM_L, -ll);
    }
    else if (ll > 0)
    {
      digitalWrite(DIR_L, HIGH);
      analogWrite(PWM_L, ll);
    }
    else
    {
      analogWrite(PWM_L, 0);
    }

    if (rr < 0)
    {
      digitalWrite(DIR_R, HIGH);
      analogWrite(PWM_R, -rr);
    }
    else if (rr > 0)
    {
      digitalWrite(DIR_R, LOW);
      analogWrite(PWM_R, rr);
    }
    else
    {
      analogWrite(PWM_R, 0);
    }
  }
  else
  {
    analogWrite(PWM_L, 0);
    analogWrite(PWM_R, 0);
    mErrL = 0; mErrR = 0;
    mdErrL = 0; mdErrR = 0;
    SpeedL = 0; SpeedR = 0;
    mLastL = 0;  mLastR = 0;
  }

}


void com_agv_motor(int dSL, int dSR)
{
  int dRL = 0, dRR = 0;
  delay(13);
  dRL = abs(myEncL.read());
  dRR = abs(myEncR.read());

  CountL += (float)dRL / 42.2;
  CountR += (float)dRR / 42.2;

  data_comm[4] = CountL;
  data_comm[5] = CountR;

  myEncL.write(0);
  myEncR.write(0);

  mErrL =  abs(dSL) - dRL;
  mErrR =  abs(dSR) - dRR;

  mdErrL = mErrL - mLastL;
  mLastL = mErrL;


  mdErrR = mErrR - mLastR;
  mLastR = mErrR;

  SpeedL += (float)(mErrL * 0.3) +  (float)(mdErrL * 0.4 );
  SpeedR += (float)(mErrR * 0.3) +  (float)(mdErrR * 0.4 );

  if (SpeedL >= 255) SpeedL = 255;
  if (SpeedR >= 255) SpeedR = 255;

  if (dSL < 0) pwmL = -SpeedL;
  else if (dSL > 0) pwmL = SpeedL;
  else        pwmL = 0;
  if (dSR < 0) pwmR = -SpeedR;
  else if (dSR > 0) pwmR = SpeedR;
  else        pwmR = 0;

  agv_motor(pwmL, pwmR);
}


#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskComm( void *pvParameters );
void TaskMotor( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  slave.start();
  init_motor();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskComm
    ,  "Comm"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskMotor
    ,  "Motor"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskComm(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {

    slave.poll( data_comm, 6 );

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskMotor(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;)
  {
    if (data_comm[3] == 1) {
      CountL = 0;
      CountR = 0;
      data_comm[3] = 0;
      data_comm[4] = 0;
      data_comm[5] = 0;
    }

    com_agv_motor(data_comm[1], data_comm[2]);

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
