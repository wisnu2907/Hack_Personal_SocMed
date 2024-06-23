#define bt1 32
#define bt2 34
#define vacuum 23
#define buzzer 13

const int numArrays = 5;
const int arraySize = 4;
float arrays[numArrays][arraySize];

//DXL SETUP
//--------------------------------------//
#include <Dynamixel_Serial.h>
#include <math.h>

// Define constants for servo IDs, control pin, baud rate, and angle limits
#define ID1 10u  //mx1A
#define ID2 9u   //mx1B
#define ID3 5u
#define ID4 6u

#define SERVO_ControlPin 7
#define SERVO_SET_Baudrate 1000000
#define CW_LIMIT_ANGLE 0
#define CCW_LIMIT_ANGLE_MX 0xFFF
#define CCW_LIMIT_ANGLE_AX 0x3FF

// Calculate the center position of the servo
unsigned int tegak_mx = CCW_LIMIT_ANGLE_MX / 2;
unsigned int tegak_ax = CCW_LIMIT_ANGLE_AX / 2;

// Define the lengths of the links
float L1 = 38.1;  // cm
float L2 = 44.6;  // cm

unsigned int Deg_mx(float degm) {
  float pos_floatm = degm / 0.08789;
  int posm = (int)pos_floatm;
  float decimal_partm = pos_floatm - posm;

  if (decimal_partm > 0.5)
    posm++;

  return (unsigned int)posm;
}

unsigned int Deg_ax(float dega) {
  float pos_floata = dega / 0.29;
  int posa = (int)pos_floata;
  float decimal_parta = pos_floata - posa;

  if (decimal_parta > 0.5)
    posa++;

  return (unsigned int)posa;
}

void driveServo(float x, float y, int spd1, int spd2) {
  // Calculate the angles for the servo positions using inverse kinematics

  float theta2 = acos((pow(x, 2) + pow(y, 2) - pow(L1, 2) - pow(L2, 2)) / (2 * L1 * L2));
  float theta1 = atan2(x, y) - atan2((L2 * sin(theta2)), (L1 + L2 * cos(theta2)));

  // Convert angles to servo positions in degrees
  float pos1 = theta1 * 57.2;
  float pos2 = theta2 * 57.2;

  // Also, set the speed for servo movement
  if (pos1 < 0 && pos2 > 0) {
    pos1 = (-1) * pos1;
    servo1(tegak_mx - Deg_mx(pos1), spd1);
    servo2(tegak_mx - Deg_mx(pos2), spd2);
  } else if (pos1 > 0 && pos2 < 0) {
    pos2 = (-1) * pos2;
    servo1(tegak_mx + Deg_mx(pos1), spd1);
    servo2(tegak_mx + Deg_mx(pos2), spd2);
  } else if (pos1 < 0 && pos2 < 0) {
    pos1 = (-1) * pos1;
    pos2 = (-1) * pos2;
    servo1(tegak_mx + Deg_mx(pos1), spd1);
    servo2(tegak_mx + Deg_mx(pos2), spd2);
  } else if (pos1 > 0 && pos2 > 0) {
    servo1(tegak_mx + Deg_mx(pos1), spd1);
    servo2(tegak_mx - Deg_mx(pos2), spd2);
  }
}

// Function to control the first joint
void servo1(unsigned int pos1, int speed) {
  Dynamixel.servo(ID1, pos1, speed);
  delay(1);
}

// Function to control the second joint
void servo2(unsigned int pos2, int speed) {
  Dynamixel.servo(ID2, pos2, speed);
  delay(1);
}

void servo3(unsigned int pos3, int speed) {
  Dynamixel.servo(ID3, pos3, speed);
  delay(1);
}

// Function to control the second joint
void servo4(unsigned int pos4, int speed) {
  Dynamixel.servo(ID4, pos4, speed);
  delay(1);
}


//MOTOR SETUP
//--------------------------------------//

float pulse_rotation = 532;  //
#define d_wheel 15           // cm
float k_wheel = 3.14 * 15.2;
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
#include <Encoder.h>

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
int count = 0;

Encoder myEncL(2, 8);
Encoder myEncR(3, 4);

long oldPositionL = -999;
long oldPositionR = -999;

// float kp = 1.11;
float kp = 0.3;
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

  // Serial.println(mErrL);
  agv_motor(pwmL, pwmR);
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

//RTOS
//--------------------------------------//
#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskComm(void *pvParameters);
void TaskMajuAwal(void *pvParameters);
void TaskLeft(void *pvParameters);
void TaskMajuKedua(void *pvParameters);
void TaskArmPemilah(void *pvParameters);

TaskHandle_t TaskHandle_TaskMajuAwal;
TaskHandle_t TaskHandle_TaskLeft;
TaskHandle_t TaskHandle_TaskMajuKedua;
TaskHandle_t TaskHandle_TaskArmPemilah;
TaskHandle_t TaskHandle_TaskComm;


void setup() {
  Serial.begin(1000000);
  Serial2.begin(9600);
  // Serial.setTimeout(1);
  // Serial2.setTimeout(5);
  init_motor();
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
  pinMode(bt1, INPUT_PULLUP);
  pinMode(vacuum, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Dynamixel.begin(SERVO_SET_Baudrate, SERVO_ControlPin);
  Dynamixel.setMode(ID1, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_MX);
  Dynamixel.setMode(ID2, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_MX);

  Dynamixel.setMode(ID3, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_AX);
  Dynamixel.setMode(ID4, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_AX);

  // driveServo(5, 7, 0x3FF / 8, 0x3FF / 5);
  // driveServo(1, 10, 0x3FF / 8, 0x3FF / 8);
  // // delay(500);
  servo1(tegak_mx + Deg_mx(90), 0x3FF / 8);
  servo2(tegak_mx + Deg_mx(180), 0x3FF / 8);
  servo3(tegak_ax, 500 / 2);
  servo4(tegak_ax, 501 / 2);

  while (!Serial) {
    ;
  }

  xTaskCreate(TaskComm, "Comm", 128, NULL, 4, NULL);
  xTaskCreate(TaskMajuAwal, "Motor", 128, NULL, 3, &TaskHandle_TaskMajuAwal);
  xTaskCreate(TaskLeft, "Motor", 128, NULL, 2, &TaskHandle_TaskLeft);
  xTaskCreate(TaskMajuKedua, "Motor", 128, NULL, 1, &TaskHandle_TaskMajuKedua);
  xTaskCreate(TaskArmPemilah, "Motor", 128, NULL, 0, &TaskHandle_TaskArmPemilah);
}

String input;
int spdL, spdR;
int start;
int spaceIndex;
float tempatSampah;
float x;
float y;

void hisap() {
  digitalWrite(vacuum, HIGH);
}

void lepas() {
  digitalWrite(vacuum, LOW);
}

void sampah1() {
  driveServo(-43, 63, 0x3FF / 8, 0x3FF / 5);
  delay(1);
}

void sampah2() {
  driveServo(-22, 63, 0x3FF / 8, 0x3FF / 5);
  delay(1);
}

void sampah3() {
  driveServo(-1, 63, 0x3FF / 8, 0x3FF / 5);
  delay(1);
}

void sampah4() {
  driveServo(21, 63, 0x3FF / 8, 0x3FF / 5);
  delay(1);
}

void sampah5() {
  driveServo(45, 63, 0x3FF / 8, 0x3FF / 5);
  delay(1);
}

void ngisep() {
  servo3(tegak_ax - 300, 500);
  servo4(tegak_ax, 501);
  delay(1);
}

void posisiDefault() {
  servo3(tegak_ax, 500);
  servo4(tegak_ax + 301, 501);
  delay(1);
}

void loop() {
}

void parseData(String data) {
  int arrayIndex = 0;
  String valueStr = "";

  for (int i = 0; i < data.length(); i++) {
    char c = data.charAt(i);
    if (c == ' ' || c == '\n') {
      if (arrayIndex % arraySize == 0) {
        tempatSampah = valueStr.toFloat();
      } else if (arrayIndex % arraySize == 2) {
        x = valueStr.toFloat();
      } else if (arrayIndex % arraySize == 3) {
        y = valueStr.toFloat();
      }

      if (c == '\n') {
        // Reset index for new row of data
        arrayIndex = -1;  // It will become 0 after increment below
      }

      valueStr = "";
      arrayIndex++;
    } else if ((c >= '0' && c <= '9') || c == '.') {
      valueStr += c;
    }
  }
}

/*
void parseData(String data) {
  int arrayIndex = 0;
  String valueStr = "";

  for (int i = 0; i < data.length(); i++) {
    char c = data.charAt(i);
    if (c == '*') {
      arrays[arrayIndex / arraySize][arrayIndex % arraySize] = valueStr.toFloat();
      valueStr = "";
      arrayIndex++;
    } else if ((c >= '0' && c <= '9') || c == '.') {
      valueStr += c;
    }
  }
  // Add the last value
  arrays[arrayIndex / arraySize][arrayIndex % arraySize] = valueStr.toFloat();
}
*/

//TASK
//--------------------------------------//

/*
INI UDAH DIPECAH MENJADI VARIABEL TEMPAT SAMPAH, X, Y
COMMENT SALAH SATU
*/

void TaskComm(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    delay(20);
    if (Serial2.available()) {
      String data = Serial2.readStringUntil('\n');  // Read the incoming data until newline character
      parseData(data);

      // Print the values to verify
      Serial.print("tempatSampah: ");
      Serial.println(tempatSampah, 2);
      Serial.print("x: ");
      Serial.println(x, 2);
      Serial.print("y: ");
      Serial.println(y, 2);
      Serial.println("=====================");
    }
    vTaskDelay(1);
  }
}

/*
INI BUAT NGECEK DATA YANG DITERIMA DARI PYTHON
COMMENT SALAH SATU
*/

/*
void TaskComm(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    // Serial.println(CountL);
    // Serial.println(CountL);
    // vTaskSuspend(TaskHandle_TaskArmPemilah);
    // Serial.println("TaskComm");

    delay(20);
    if (Serial2.available()) {
      String data = Serial2.readStringUntil('\n');  // Read the incoming data until newline character
      parseData(data);

      // Print the arrays to verify
      for (int i = 0; i < numArrays; i++) {
        for (int j = 0; j < arraySize; j++) {
          Serial.print(arrays[i][j], 2);
          Serial.print(" ");
        }
        Serial.println();
      }
      Serial.println("=====================");
    }

    // if (Serial.available() > 0) {
    //   input = Serial.readStringUntil('\n');
    //   // Serial.print("Received: ");
    //   // Serial.println(input);
    //   spaceIndex = input.indexOf(' ');
    //   if (spaceIndex != -1) {
    //     x = input.substring(0, spaceIndex).toFloat();
    //     y = input.substring(spaceIndex + 1).toFloat();
    //   }
    // }
    vTaskDelay(1);
  }
}
*/

void TaskMajuAwal(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    //Untuk Maju
    // Serial.print("tombolStart = ");
    // Serial.println(digitalRead(bt1));
    // Serial.println("TaskMajuAwal");
    // vTaskSuspend(TaskHandle_TaskComm);
    vTaskSuspend(TaskHandle_TaskLeft);  //Suspend Task2/3
    vTaskSuspend(TaskHandle_TaskMajuKedua);
    // vTaskSuspend(TaskHandle_TaskArmPemilah);


    if (digitalRead(bt1) == LOW) {
      delay(2000);
      for (int i = 0; i < 3; i++) {
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
        delay(500);
      }
      count = 1;
    }

    if (count == 1) {

      com_agv_motor(11, 11);
      // Serial.println(mErrL);
      // Serial.println(CountL);
      if (CountL >= 420) {
        // if (CountL >= 200) {

        Stop();
        delay(1200);
        CountL = 0;
        CountR = 0;
        delay(200);
        vTaskResume(TaskHandle_TaskLeft);
        vTaskDelete(TaskHandle_TaskMajuAwal);
      }
    }
  }
  // else if (tombolStart == LOW) {
  //   Stop();
  // }
  // vTaskDelay(1);
}

void TaskLeft(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    //Untuk Maju //Suspend Task2/3
    // Serial.println("TaskLeft");
    // vTaskSuspend(TaskHandle_TaskComm);
    vTaskSuspend(TaskHandle_TaskMajuKedua);
    // vTaskSuspend(TaskHandle_TaskArmPemilah);

    com_agv_motor(5, -5);
    // Serial.println(mErrL);

    // Serial.println(CountL);
    if (CountL >= 44.3) {
      // if (CountL >= 35.4) {

      Stop();
      delay(1000);
      CountL = 0;
      CountR = 0;
      delay(200);
      vTaskResume(TaskHandle_TaskMajuKedua);
      vTaskDelete(TaskHandle_TaskLeft);
    }
  }
}

void TaskMajuKedua(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    //Untuk Maju
    // Serial.println("TaskMajuKedua");
    // vTaskSuspend(TaskHandle_TaskComm);
    // vTaskSuspend(TaskHandle_TaskArmPemilah);

    com_agv_motor(11, 11);
    // Serial.println(mErrL);

    // Serial.println(CountL);
    if (CountL >= 122) {
      // if (CountL >= 100) {
      Stop();
      delay(500);
      // vTaskResume(TaskHandle_TaskArmPemilah);
      // vTaskResume(TaskHandle_TaskComm);
      vTaskDelete(TaskHandle_TaskMajuKedua);
      delay(100);
    }
  }
}

void TaskArmPemilah(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    // Ensure the TaskComm has processed the data before proceeding
    if (tempatSampah != 0 && x != 0 && y != 0) {
      // Move the arm to the coordinates (x, y)
      driveServo(x, y, 0x3FF / 8, 0x3FF / 8);

      // Move servo2 down to pick up the trash
      servo3(tegak_ax - 300, 500);
      servo4(tegak_ax, 501);
      delay(500); // Delay to ensure the servo has time to move
      hisap();    // Start the vacuum to pick up the trash
      delay(1000); // Give some time to ensure trash is picked up

      // Move the arm to the correct trash bin
      switch ((int)tempatSampah) {
        case 1:
          sampah1();
          break;
        case 2:
          sampah2();
          break;
        case 3:
          sampah3();
          break;
        case 4:
          sampah4();
          break;
        case 5:
          sampah5();
          break;
        default:
          break;
      }

      // Release the trash
      lepas();

      // Move the arm back to default position
      posisiDefault();

      // Reset the variables to ensure the arm doesn't move again until new data is processed
      tempatSampah = 0;
      x = 0;
      y = 0;
    }

    vTaskDelay(100); // Adjust the delay as necessary for your task scheduling
  }
}
