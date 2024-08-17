#define bt1 32
#define bt2 34
#define vacuum 23
#define buzzer 13

const int numArrays = 5;
const int arraySize = 3;
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
float L2 = 37.4;  // cm

//setup mpu
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Variabel untuk kalibrasi
float gyroZ_calibration = 0;
const int calibration_samples = 1000;

// Variabel untuk filter rata-rata bergerak
const int filter_window_size = 10;  // Ukuran jendela filter
float gyroZ_values[filter_window_size];
int filter_index = 0;
bool filter_full = false;

// Variabel untuk menghitung sudut
float angleZ = 0;
unsigned long last_time;

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

void driveServoSudutDalam(float x, float y, int spd1, int spd2) {
  // Calculate the angles for the servo positions using inverse kinematics
  // float pos1 = 0;
  // float pos2 = 0;

  float theta2 = acos((pow(x, 2) + pow(y, 2) - pow(L1, 2) - pow(L2, 2)) / (2 * L1 * L2));
  float theta1 = atan2(x, y) - atan2((L2 * sin(theta2)), (L1 + L2 * cos(theta2)));

  // if (x > 0) {
  //   // Pendekatan kiri
  //   pos1 = theta1 * 57.2;
  //   pos2 = theta2 * 57.2;
  // } else {
  //   // Pendekatan kanan
  //   pos1 = 360 - theta1 * 57.2;
  //   pos2 = 360 - theta2 * 57.2;
  // }
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

void driveServoSudutLuar(float x, float y, int spd1, int spd2) {
  // Calculate the angles for the servo positions using inverse kinematics
  // float pos1 = 0;
  // float pos2 = 0;

  float theta2 = acos((pow(x, 2) + pow(y, 2) - pow(L1, 2) - pow(L2, 2)) / (2 * L1 * L2));
  float theta1 = atan2(x, y) - atan2((L2 * sin(theta2)), (L1 + L2 * cos(theta2)));

  // if (x > 0) {
  //   // Pendekatan kiri
  //   pos1 = theta1 * 57.2;
  //   pos2 = theta2 * 57.2;
  // } else {
  //   // Pendekatan kanan
  //   pos1 = 360 - theta1 * 57.2;
  //   pos2 = 360 - theta2 * 57.2;
  // }
  // Convert angles to servo positions in degrees
  float pos1 = 360 - theta1 * 57.2;
  float pos2 = 360 - theta2 * 57.2;

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
#define d_wheel 15.2 * PI    // cm
const float dwheel = 0.15;
const float wheelBase = 46.2;

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

float theta = 0.0;  // Robot orientation
float thetaErr = 0.0;
float lastThetaErr = 0.0;
float sumThetaErr = 0.0;
float thetaCorrection = 0.0;
float delta_theta = 0;
float dThetaErr = 0;

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

  // Serial.print(pwmL);
  // Serial.print("\t");
  // Serial.println(pwmR);
  // Serial.println(mErrL);
  agv_motor(pwmL, pwmR);
  // Serial.println(0);
}

void com_agv_motor_odometry(int dSL, int dSR) {
  int dRL = 0, dRR = 0;
  delay(13);
  dRL = abs(myEncL.read());
  dRR = abs(myEncR.read());

  CountL += (float)dRL / 11.14;
  CountR += (float)dRR / 11.14;

  myEncL.write(0);
  myEncR.write(0);

  // if (dRL != oldPositionL || dRR != oldPositionR) {
  //   oldPositionL = dRL;
  //   oldPositionR = dRR;
  // Serial.print(dRL);
  // Serial.print('\t');
  // Serial.print(dRR);
  // Serial.print('\t');

  delta_theta = (dRR - dRL) * (d_wheel / wheelBase) / pulse_rotation;
  // Serial.println(delta_theta);
  theta += delta_theta;

  // Calculate PID for theta
  thetaErr = 0.0 - theta;
  sumThetaErr += thetaErr;
  dThetaErr = thetaErr - lastThetaErr;
  lastThetaErr = thetaErr;

  thetaCorrection = (float)(0.01 * thetaErr) + (float)(0.00 * dThetaErr) + (float)(0 * sumThetaErr);
  // thetaCorrection = (float)(2 * thetaErr) + (float)(4 * dThetaErr) + (float)(0 * sumThetaErr);

  // Print theta (orientation)
  // Serial.println(theta);
  // }


  SpeedL = dSL - thetaCorrection;
  SpeedR = dSR + thetaCorrection;

  if (SpeedL >= 255) SpeedL = 255;
  if (SpeedR >= 255) SpeedR = 255;

  if (dSL < 0) pwmL = -SpeedL;
  else if (dSL > 0) pwmL = SpeedL;
  else pwmL = 0;
  if (dSR < 0) pwmR = -SpeedR;
  else if (dSR > 0) pwmR = SpeedR;
  else pwmR = 0;

  // Serial.print(SpeedL);
  // Serial.print("\t");
  // Serial.println(SpeedR);

  agv_motor(pwmL, pwmR);
}


void com_agv_motor_MPU(int dSL, int dSR) {
  int dRL = 0, dRR = 0;
  delay(13);
  dRL = abs(myEncL.read());
  dRR = abs(myEncR.read());

  CountL += (float)dRL / 11.14;
  CountR += (float)dRR / 11.14;

  // Serial.println(CountL);

  //42.2 = pulse_rotation/(k_wheel)


  myEncL.write(0);
  myEncR.write(0);

  thetaErr = 0 - angleZ;
  sumThetaErr += thetaErr;
  dThetaErr = thetaErr - lastThetaErr;
  lastThetaErr = thetaErr;

  thetaCorrection = (float)(1.2 * thetaErr) + (float)(0.00 * dThetaErr) + (float)(0 * sumThetaErr);
  // thetaCorrection = (float)(2 * thetaErr) + (float)(4 * dThetaErr) + (float)(0 * sumThetaErr);

  // Print theta (orientation)
  // Serial.println(theta);
  // }


  SpeedL = dSL - thetaCorrection;
  SpeedR = dSR + thetaCorrection;

  if (SpeedL >= 255) SpeedL = 255;
  if (SpeedR >= 255) SpeedR = 255;

  if (dSL < 0) pwmL = -SpeedL;
  else if (dSL > 0) pwmL = SpeedL;
  else pwmL = 0;
  if (dSR < 0) pwmR = -SpeedR;
  else if (dSR > 0) pwmR = SpeedR;
  else pwmR = 0;

  // Serial.print(SpeedL);
  // Serial.print("\t");
  // Serial.println(SpeedR);

  agv_motor(pwmL, pwmR);
}

void com_agv_motor_MPU2(int dSL, int dSR) {
  int dRL = 0, dRR = 0;
  delay(13);
  dRL = abs(myEncL.read());
  dRR = abs(myEncR.read());

  CountL += (float)dRL / 11.14;
  CountR += (float)dRR / 11.14;

  // Serial.println(CountL);

  //42.2 = pulse_rotation/(k_wheel)


  myEncL.write(0);
  myEncR.write(0);

  // thetaErr = -90 - angleZ;
  thetaErr = -88.5 - angleZ;

  sumThetaErr += thetaErr;
  dThetaErr = thetaErr - lastThetaErr;
  lastThetaErr = thetaErr;

  thetaCorrection = (float)(1.5 * thetaErr) + (float)(0.00 * dThetaErr) + (float)(0 * sumThetaErr);
  // thetaCorrection = (float)(2 * thetaErr) + (float)(4 * dThetaErr) + (float)(0 * sumThetaErr);

  // Print theta (orientation)
  // Serial.println(theta);
  // }


  SpeedL = dSL - thetaCorrection;
  SpeedR = dSR + thetaCorrection;


  // SpeedL = dSL + thetaCorrection;
  // SpeedR = dSR - thetaCorrection;

  if (SpeedL >= 255) SpeedL = 255;
  if (SpeedR >= 255) SpeedR = 255;

  if (dSL < 0) pwmL = -SpeedL;
  else if (dSL > 0) pwmL = SpeedL;
  else pwmL = 0;
  if (dSR < 0) pwmR = -SpeedR;
  else if (dSR > 0) pwmR = SpeedR;
  else pwmR = 0;

  // Serial.print(SpeedL);
  // Serial.print("\t");
  // Serial.println(SpeedR);

  agv_motor(pwmL, pwmR);
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
void TaskArmFollow(void *pvParameters);
void TaskMPU(void *pvParameters);

TaskHandle_t TaskHandle_TaskMajuAwal;
TaskHandle_t TaskHandle_TaskLeft;
TaskHandle_t TaskHandle_TaskMajuKedua;
TaskHandle_t TaskHandle_TaskArmPemilah;
TaskHandle_t TaskHandle_TaskComm;
TaskHandle_t TaskHandle_TaskArmFollow;
TaskHandle_t TaskHandle_TaskMPU;


void setup() {
  Serial.begin(1000000);
  Serial2.begin(9600);
  Wire.begin();
  mpu.initialize();
  Serial.setTimeout(10000);
  Serial2.setTimeout(10000);
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

  servo2(tegak_mx - Deg_mx(175), 0x3FF / 10);
  servo1(tegak_mx - Deg_mx(85), 0x3FF / 10);
  servo3(tegak_ax, 1023 / 2);
  servo4(tegak_ax, 1023 / 2);



  // Pastikan sensor terhubung
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful");
  } else {
    Serial.println("MPU6050 connection failed");
    while (1)
      ;
  }

  // Kalibrasi gyro Z
  Serial.println("Calibrating gyro Z...");
  for (int i = 0; i < calibration_samples; i++) {
    mpu.getRotationZ();
    gyroZ_calibration += mpu.getRotationZ();
    delay(3);  // Delay untuk mendapatkan pembacaan stabil
  }
  gyroZ_calibration /= calibration_samples;
  Serial.println("Calibration complete");

  // Inisialisasi filter rata-rata bergerak
  for (int i = 0; i < filter_window_size; i++) {
    gyroZ_values[i] = 0;
  }

  // Inisialisasi waktu
  last_time = millis();

  while (!Serial) {
    ;
  }

  xTaskCreate(TaskComm, "Comm", 128, NULL, 4, NULL);
  xTaskCreate(TaskMajuAwal, "Motor", 128, NULL, 3, &TaskHandle_TaskMajuAwal);
  // xTaskCreate(TaskLeft, "Motor", 128, NULL, 3, &TaskHandle_TaskLeft);
  // xTaskCreate(TaskMajuKedua, "Motor", 128, NULL, 2, &TaskHandle_TaskMajuKedua);
  xTaskCreate(TaskArmPemilah, "Motor", 128, NULL, 1, &TaskHandle_TaskArmPemilah);
  // xTaskCreate(TaskArmToSampah, "Motor", 128, NULL, 1, NULL);
  // xTaskCreate(TaskArmFollow, "Motor", 128, NULL, 1, &TaskHandle_TaskArmFollow);
  xTaskCreate(TaskMPU, "MPU", 128, NULL, 4, &TaskHandle_TaskMPU);
}

String input;
int spdL, spdR;
int start;
int spaceIndex;
int tempatSampah;
int previousTempatSampah = 0;  // Variabel untuk menyimpan tempatSampah sebelumnya
float x;
float y;

void hisap() {
  digitalWrite(vacuum, HIGH);
}

void lepas() {
  digitalWrite(vacuum, LOW);
}

void sampah5() {
  // driveServo(39 + 3, 63 + 2, 0x3FF / 8, 0x3FF / 8);
  // servo1(tegak_mx + Deg_mx(33), 0x3FF / 8);
  // delay(1);
  // servo2(tegak_mx - Deg_mx(0), 0x3FF / 8);
  // delay(1);
  driveServoSudutDalam(36, 66, 0x3FF / 8, 0x3FF / 8);
  delay(2000);
  // posDefault();
  // delay
  servo3(tegak_ax, 1000);
  delay(1);
  servo4(tegak_ax + Deg_ax(90), 1000);
  delay(1);
  delay(1000);
  // Release the trash
  lepas();

  // Move the arm back to default position
  delay(1000);
  posDefault(1000, 1000);
  delay(500);
}

void sampah2() {
  driveServoSudutDalam(-22, 66, 0x3FF / 8, 0x3FF / 8);
  delay(1400);
  // posDefault();
  // delay
  posServoUjungSampah(1000);
  delay(600);
  // Release the trash
  lepas();

  // Move the arm back to default position
  delay(1000);
  posDefault(1000, 1000);
  delay(500);
}

void sampah3() {
  driveServoSudutDalam(0, 66, 0x3FF / 8, 0x3FF / 8);
  delay(1400);
  // posDefault();
  // delay
  posServoUjungSampah(1000);
  delay(600);
  // Release the trash
  lepas();

  // Move the arm back to default position
  delay(700);
  posDefault(1000, 1000);
  delay(400);
}

void sampah4() {
  driveServoSudutDalam(21, 66, 0x3FF / 8, 0x3FF / 8);
  delay(1400);
  // posDefault();
  // delay
  posServoUjungSampah(1000);
  delay(600);
  // Release the trash
  lepas();

  // Move the arm back to default position
  delay(700);
  posDefault(1000, 1000);
  delay(400);
}

void sampah1() {
  // driveServo(-39 - 4, 63, 0x3FF / 8, 0x3FF / 8);
  servo1(tegak_mx - Deg_mx(40), 0x3FF / 8);
  delay(1);
  servo2(tegak_mx + Deg_mx(0), 0x3FF / 8);
  delay(1);
  // driveServoSudutDalam(-36, 66, 0x3FF / 8, 0x3FF / 8);

  delay(2000);
  // posDefault();
  // delay
  servo3(tegak_ax, 1000);
  delay(1);
  servo4(tegak_ax + Deg_ax(90), 1000);
  delay(1);
  delay(600);
  // Release the trash
  lepas();

  // Move the arm back to default position
  delay(700);
  posDefault(1000, 1000);
  delay(400);
}

void posServoNgisep() {
  servo4(tegak_ax + Deg_ax(89), 700);
  delay(400);
  servo3(tegak_ax - Deg_ax(70), 500);
  servo4(tegak_ax + Deg_ax(15), 501);
  delay(10);
}

void posServoNgisep(int spd1, int spd2) {
  servo4(tegak_ax + Deg_ax(89), spd1);
  delay(600);
  servo3(tegak_ax - 300, spd2);
  servo4(tegak_ax, spd2);
  delay(10);
}

void posServoUjungSampah(int spd1) {
  servo3(tegak_ax - 300, spd1);
  delay(1);
  servo4(tegak_ax, spd1);
  delay(1);
}

void posDefault() {
  servo3(tegak_ax, 500);
  // delay(200);
  servo4(tegak_ax + Deg_ax(89), 500);
  delay(300);
  servo4(tegak_ax, 500);
}

void posDefault(int spd1, int spd2) {

  servo3(tegak_ax, spd1);
  delay(1);
  servo4(tegak_ax, spd2);
  delay(1);
}

void loop() {
}

// Setting Dynamic Array
//--------------------------------------//
// float* detectedObjects = NULL;
// int numObjects = 0;

// Function to dynamically allocate memory for the detected objects
// void allocateMemory(int num) {
//   if (detectedObjects != NULL) {
//     free(detectedObjects);
//   }
//   numObjects = num;
//   detectedObjects = (float*)malloc(numObjects * arraySize * sizeof(float));
//   if (detectedObjects == NULL) {
//     Serial.println("Memory allocation failed!");
//     while (true); // Halt the program if memory allocation fails
//   }
// }

// // Function to parse the incoming data and fill the dynamic array
// void parseData(String data) {
//   int dataSize = data.length() / (arraySize * sizeof(float));
//   allocateMemory(dataSize);
//   char* dataArray = (char*)data.c_str();
//   for (int i = 0; i < dataSize; i++) {
//     for (int j = 0; j < arraySize; j++) {
//       memcpy(&detectedObjects[i * arraySize + j], dataArray + (i * arraySize + j) * sizeof(float), sizeof(float));
//     }
//   }
// }

// void parseData(String data) {
//   int arrayIndex = 0;
//   String valueStr = "";

//   for (int i = 0; i < data.length(); i++) {
//     char c = data.charAt(i);
//     if (c == '*' || c == '\n') {
// if (arrayIndex % arraySize == 0) {
//   tempatSampah = valueStr.toFloat();
// } else if (arrayIndex % arraySize == 1) {
//   x = valueStr.toFloat();
// } else if (arrayIndex % arraySize == 2) {
//   y = valueStr.toFloat();
// }

//       if (c == '\n') {
//         // Reset index for new row of data
//         arrayIndex = -1;  // It will become 0 after increment below
//       }

//       valueStr = "";
//       arrayIndex++;
//     } else if ((c >= '0' && c <= '9') || c == '.') {
//       valueStr += c;
//     }
//   }
// }



void parseData(String data) {
  int arrayIndex = 0;
  String valueStr = "";
  // Serial.println("2");

  for (int i = 0; i < data.length(); i++) {
    char c = data.charAt(i);
    // Serial.println("3");

    if (c == '*') {
      arrays[arrayIndex / arraySize][arrayIndex % arraySize] = valueStr.toFloat();
      // Serial.println("4");

      valueStr = "";
      // Serial.println("5");

      arrayIndex++;
      // Serial.println("6");

    } else if ((c >= '0' && c <= '9') || c == '.' || c == '-') {
      valueStr += c;
      // Serial.println("7");
    }
  }
  // Add the last value
  arrays[arrayIndex / arraySize][arrayIndex % arraySize] = valueStr.toFloat();
  tempatSampah = arrays[0][0];
  x = arrays[0][1];
  y = arrays[0][2];
  // Serial.println("11");

  // Serial.print("tempatSampah: ");
  // Serial.println(tempatSampah);
  // Serial.print("x: ");
  // Serial.println(x, 2);
  // Serial.print("y: ");
  // Serial.println(y, 2);
  // Serial.println("=====================");
}

//TASK
//--------------------------------------//

/*
INI UDAH DIPECAH MENJADI VARIABEL TEMPAT SAMPAH, X, Y
COMMENT SALAH SATU
*/


// void TaskComm(void *pvParameters) {
//   (void)pvParameters;

//   for (;;) {
//     delay(20);
//     if (Serial2.available()) {
//       String data = Serial2.readStringUntil('\n');  // Read the incoming data until newline character
//       parseData(data);

//       // Print the values to verify
// Serial.print("tempatSampah: ");
// Serial.println(tempatSampah);
// Serial.print("x: ");
// Serial.println(x, 2);
// Serial.print("y: ");
// Serial.println(y, 2);
// Serial.println("=====================");
//     }
//     vTaskDelay(1);
//   }
// }


/*
INI BUAT NGECEK DATA YANG DITERIMA DARI PYTHON
COMMENT SALAH SATU
*/


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
      // for (int i = 0; i < numArrays; i++) {
      //   for (int j = 0; j < arraySize; j++) {
      //     Serial.print(arrays[i][j], 2);
      //     Serial.print(" ");
      //   }
      //   Serial.println();
      // }
      // Serial.println("=====================");
    }

    vTaskDelay(1);
  }
}

void TaskMPU(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    unsigned long current_time = millis();
    float dt = (current_time - last_time) / 1000.0;  // Interval waktu dalam detik
    last_time = current_time;

    int16_t gyroZ_raw = mpu.getRotationZ();
    float gyroZ = (gyroZ_raw - gyroZ_calibration) / 131.0;  // 131.0 adalah sensitivitas gyro default dari MPU6050

    // Masukkan nilai gyroZ ke dalam filter
    gyroZ_values[filter_index] = gyroZ;
    filter_index = (filter_index + 1) % filter_window_size;

    if (filter_index == 0) {
      filter_full = true;
    }

    // Hitung nilai rata-rata dari filter
    float gyroZ_avg = 0;
    int count = filter_full ? filter_window_size : filter_index;
    for (int i = 0; i < count; i++) {
      gyroZ_avg += gyroZ_values[i];
    }
    gyroZ_avg /= count;

    // Hitung sudut berdasarkan gyroZ yang sudah difilter
    angleZ += gyroZ_avg * dt;

    // Tampilkan nilai sudut
    // Serial.print("Angle Z: ");
    Serial.println(angleZ);

    vTaskDelay(1);
  }
}


void TaskMajuAwal(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    //Untuk Maju
    // Serial.print("tombolStart = ");
    // Serial.println(digitalRead(bt1));
    // Serial.println("TaskMajuAwal");
    // vTaskSuspend(TaskHandle_TaskComm);
    // servo2(tegak_mx - Deg_mx(175), 0x3FF / 10);
    // servo1(tegak_mx - Deg_mx(85), 0x3FF / 10);
    // servo3(tegak_ax, 1023 / 2);
    // servo4(tegak_ax, 1023 / 2);
    // vTaskSuspend(TaskHandle_TaskLeft);  //Suspend Task2/3
    // vTaskSuspend(TaskHandle_TaskMajuKedua);
    vTaskSuspend(TaskHandle_TaskArmPemilah);


    if (digitalRead(bt1) == LOW) {
      for (int i = 0; i < 2; i++) {
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
        delay(500);
      }
      count = 1;
    }

    if (count == 1) {

      // com_agv_motor(2, 2);
      com_agv_motor_MPU(25, 25);

      // com_agv_motor_odometry(14, 14);
      // Serial.println(mErrL);
      // Serial.print("CountL:");
      // Serial.print(CountL);
      Serial.print("  CountR:");
      Serial.println(CountR);
      // if (CountR >= 165678.64 - 1000) {
      // if (CountR >= 109980 + 5000) {
      // if (CountR >= 68.4) {
      if (CountR >= 95.4) {

        // if (CountL >= 200) {

        Stop();
        delay(800);
        CountL = 0;
        CountR = 0;
        delay(100);
        vTaskResume(TaskHandle_TaskArmPemilah);
        vTaskDelete(TaskHandle_TaskMajuAwal);
        delay(100);
      }
    }
  }
  // else if (tombolStart == LOW) {
  //   Stop();
  // }
  // vTaskDelay(1);
}

// void TaskLeft(void *pvParameters) {
//   (void)pvParameters;

//   for (;;) {
//     //Untuk Maju //Suspend Task2/3
//     // Serial.println("TaskLeft");
//     // vTaskSuspend(TaskHandle_TaskComm);
//     // servo2(tegak_mx - Deg_mx(175), 0x3FF / 10);
//     // servo1(tegak_mx - Deg_mx(85), 0x3FF / 10);
//     // servo3(tegak_ax, 1023 / 2);
//     // servo4(tegak_ax, 1023 / 2);
//     vTaskSuspend(TaskHandle_TaskMajuKedua);
//     vTaskSuspend(TaskHandle_TaskArmPemilah);

//     com_agv_motor(4, -4);
//     // agv_motor(30, -30);
//     // Serial.println(mErrL);

//     // Serial.println(CountL);
//     if (angleZ <= -89 && angleZ >= -91) {
//       // if (CountL >= 35.4) {
//       // com_agv_motor_MPU(20, -20);
//       // delay(2000);
//       Stop();
//       delay(800);
//       CountL = 0;
//       CountR = 0;
//       delay(100);
//       vTaskResume(TaskHandle_TaskMajuKedua);
//       vTaskDelete(TaskHandle_TaskLeft);
//       delay(100);
//     }
//   }
// }

// void TaskMajuKedua(void *pvParameters) {
//   (void)pvParameters;

//   for (;;) {
//     //Untuk Maju
//     // Serial.println("TaskMajuKedua");
//     // vTaskSuspend(TaskHandle_TaskComm);
//     // servo2(tegak_mx - Deg_mx(175), 0x3FF / 10);
//     // servo1(tegak_mx - Deg_mx(85), 0x3FF / 10);
//     // servo3(tegak_ax, 1023 / 2);
//     // servo4(tegak_ax, 1023 / 2);
//     vTaskSuspend(TaskHandle_TaskArmPemilah);

//     com_agv_motor_MPU2(25, 25);
//     // Serial.println(mErrL);

//     // Serial.println(CountL);
//     // if (CountR >= 135) {
//     if (CountL >= 127) {

//       // if (CountL >= 100) {
//       servo2(tegak_mx - Deg_mx(175), 0x3FF / 10);
//       servo1(tegak_mx - Deg_mx(85), 0x3FF / 10);
//       servo3(tegak_ax, 1023 / 2);
//       servo4(tegak_ax, 1023 / 2);
//       Stop();
//       delay(500);
//       vTaskResume(TaskHandle_TaskArmPemilah);
//       // vTaskResume(TaskHandle_TaskComm);
//       vTaskDelete(TaskHandle_TaskMajuKedua);
//       vTaskSuspend(TaskHandle_TaskMPU);
//       delay(100);
//     }
//   }
// }

//bool terhisap = false;

void TaskArmPemilah(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    // vTaskSuspend(TaskHandle_TaskArmFollow);
    vTaskSuspend(TaskHandle_TaskMPU);

    if (x != 0 && y != 0) {
      // int x;
      // Serial.println("12dd");
      // Serial.println("12dd");
      int address = tempatSampah;
      if (x >= 0) {
        // Pendekatan kiri
        // x = -x;
        driveServoSudutDalam(x - 6, y + 1, 0x3FF / 14, 0x3FF / 14);
        // Serial.println("13ee");

        // Serial.println("13ee");

        // driveServoSudutDalam(x , y + 1, 0x3FF / 5, 0x3FF / 5);

        delay(1200);
      } else {
        // Pendekatan kanan
        // x = x;
        driveServoSudutLuar(-(x - 9), y + 1, 0x3FF / 5, 0x3FF / 5);
        // Serial.println("14ee");


        // driveServoSudutLuar(-(x + 2), y + 1, 0x3FF / 14, 0x3FF / 14);
        delay(3000);
        // driveServoSudutLuar(-(x + 10), y, 0, 0);
        // delay(10);
      }
      // delay(100);
      // posDefault();
      // delay(780);

      //pakai ini kalo mau tes arena biru
      // driveServo(x - 7, y, 0x3FF / 6.5, 0x3FF / 6.5);

      //pakai ini kalo mau tes arena merah
      // driveServo(x - 5, y, 0x3FF / 9, 0x3FF / 9);

      // INI NGIDE
      // if (x > 0) {
      //   driveServo(x, y, 0x3FF / 10, 0x3FF / 10);
      // } else {
      //   driveServoSudutLuar(-x, y, 0x3FF / 10, 0x3FF / 10);
      // }

      // driveServo(x, y, 0x3FF / 9, 0x3FF / 9);
      // delay(700);
      hisap();  // Start the vacuum to pick up the trash
      // Serial.println("14zz");

      posServoNgisep();
      // Serial.println("15aa");

      delay(900);
      posDefault();
      // Serial.println("16qq");

      delay(500);
      // delay(500);

      // if (tempatSampah == 0) {
      //   tempatSampah = previousTempatSampah;
      // } else {
      //   previousTempatSampah = tempatSampah;
      // }
      // posDefault();
      // Move the arm to the correct trash bin
      switch (address) {
        // case 0:
        //   tempatSampah = previousTempatSampah;
        //   // No break statement to allow fall-through to the next case
        //   break;
        case 1:
          // previousTempatSampah = 1;
          sampah3();
          // Serial.println("17ww");

          break;
        case 2:
          // previousTempatSampah = 2;
          sampah5();
          // Serial.println("18rr");

          break;
        case 3:
          // previousTempatSampah = 3;
          sampah2();
          // Serial.println("19tt");

          break;
        case 4:
          // previousTempatSampah = 4;
          sampah4();
          // Serial.println("20pp");

          break;
        case 5:
          // previousTempatSampah = 5;
          sampah1();
          // Serial.println("21ss");

          break;
        default:
          break;
      }
      // Serial.println("22aa");
      servo2(tegak_mx - Deg_mx(175), 0x3FF / 12);
      // Serial.println("23pp");
      servo1(tegak_mx - Deg_mx(85), 0x3FF / 12);
      // Serial.println("24mm");

      // servo2(tegak_mx, 0x3FF / 11);
      // servo1(tegak_mx, 0x3FF / 11);
      servo3(tegak_ax, 1023 / 8);
      // Serial.println("25oo");
      servo4(tegak_ax, 1023 / 8);

      delay(500);

      // Reset the variables to ensure the arm doesn't move again until new data is processed
      // tempatSampah = 0;
      x = 0;
      y = 0;
      // vTaskResume(TaskHandle_TaskArmFollow);
      //terhisap = false;
    }
    // else if (tempatSampah == 0 && x == 0 && y == 0) {
    //   servo2(tegak_mx - Deg_mx(179.5), 0x3FF / 6);
    //   servo1(tegak_mx - Deg_mx(90), 0x3FF / 10);
    //   servo3(tegak_ax, 1023 / 2);
    //   servo4(tegak_ax, 1023 / 2);
    // }

    vTaskDelay(1);  // Adjust the delay as necessary for your task scheduling
  }
}

// void TaskArmToSampah(void *pvParameters) {
//   (void)pvParameters;

//   for (;;) {
//     sampah1();
//     // Serial.println("1");
//     delay(5000);
//     sampah2();
//     // Serial.println("2");
//     delay(3000);
//     sampah3();
//     // Serial.println("3");
//     delay(3000);
//     sampah4();
//     // Serial.println("4");
//     delay(3000);
//     sampah5();
//     // Serial.println("5");
//     delay(5000);
//     vTaskDelay(1);
//   }
// }

// void TaskArmFollow(void *pvParameters) {
//   (void)pvParameters;

//   for (;;) {
//     if (tempatSampah != 0 && x != 0 && y != 0 /&& terhisap != true/) {
//       // Move the arm to the coordinates (x, y)
//       vTaskSuspend(TaskHandle_TaskArmPemilah);
//       driveServoSudutLuar(x + 15, y, 0x3FF / 6, 0x3FF / 6);
//       delay(300);
//       driveServoSudutLuar(x + 5, y, 0x3FF / 6, 0x3FF / 6);
//       delay(300);
//       driveServoSudutLuar(x + 20, y, 0x3FF / 6, 0x3FF / 6);
//       delay(300);
//       vTaskResume(TaskHandle_TaskArmPemilah);
//       vTaskDelay(1);  // Adjust the delay as necessary for your task scheduling
//     }
//   }
// }
