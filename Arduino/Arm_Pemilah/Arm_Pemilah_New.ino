#include <Dynamixel_Serial.h>
#include <math.h>  // Include the math library for mathematical functions

// Define constants for servo IDs, control pin, baud rate, and angle limits
#define ID1 10u   //mx1A
#define ID2 9u  //mx1B
// #define relay 5

#define SERVO_ControlPin 7
#define SERVO_SET_Baudrate 1000000
#define CW_LIMIT_ANGLE 0
#define CCW_LIMIT_ANGLE_MX 0xFFF


// Calculate the center position of the servo
unsigned int tegak_mx = CCW_LIMIT_ANGLE_MX / 2;

// Define the lengths of the links
float L1 = 37.49;  // cm
float L2 = 38.5;  // cm

// Function to drive the servos based on x and y coordinates
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
    joint1(tegak_mx - Deg(pos1), spd1);
    joint2(tegak_mx + Deg(pos2), spd2);
    delay(5);
  } else if (pos1 > 0 && pos2 < 0) {
    pos2 = (-1) * pos2;
    joint1(tegak_mx + Deg(pos1), spd1);
    joint2(tegak_mx - Deg(pos2), spd2);

    delay(5);
  } else if (pos1 < 0 && pos2 < 0) {
    pos1 = (-1) * pos1;
    pos2 = (-1) * pos2;
    joint1(tegak_mx + Deg(pos1), spd1);
    joint2(tegak_mx + Deg(pos2), spd2);
    delay(5);
  } else if (pos1 > 0 && pos2 > 0) {
    joint1(tegak_mx + Deg(pos1), spd1);
    joint2(tegak_mx + Deg(pos2), spd2);
    delay(5);
  }
}

// Function to convert degrees to servo positions
unsigned int Deg(float deg) {
  float pos_float = deg / 0.088;
  int pos = (int)pos_float;
  float decimal_part = pos_float - pos;

  if (decimal_part > 0.5)
    pos++;

  return (unsigned int)pos;
}

// Function to control the first joint
void joint1(unsigned int pos1, int speed) {
  Dynamixel.servo(ID1, pos1, speed);
  delay(1);
}

// Function to control the second joint
void joint2(unsigned int pos2, int speed) {
  Dynamixel.servo(ID2, pos2, speed);
  delay(1);
}

void sampah1a() {
  driveServo(40, 64, 0x3FF/8, 0x3FF/5);
}

void sampah2a() {
  driveServo(19.5, 64, 0x3FF/8, 0x3FF/5);
}

void sampah3a() {
  driveServo(0, 66, 0x3FF/8, 0x3FF/5);
}

void sampah4a() {
  driveServo(-21.5, 63, 0x3FF/8, 0x3FF/5);
}

void sampah5a() {
 driveServo(-40, 62, 0x3FF/8, 0x3FF/5);
}
void sampah1b() {
  driveServo(40-6.5, 64, 0x3FF/8, 0x3FF/5);
}

void sampah2b() {
  driveServo(19.5-6.5, 64, 0x3FF/8, 0x3FF/5);
}

void sampah3b() {
  driveServo(0-6.5, 66, 0x3FF/8, 0x3FF/5);
}

void sampah4b() {
  driveServo(-21.5-6.5, 63, 0x3FF/8, 0x3FF/5);
}

void sampah5b() {
 driveServo(-40-3.5, 62, 0x3FF/8, 0x3FF/5);
}

// Setup function to initialize Serial communication and Dynamixel servos
void setup() {
  Serial.begin(SERVO_SET_Baudrate);
  Serial.setTimeout(1);
  delay(500);
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
  Dynamixel.begin(SERVO_SET_Baudrate, SERVO_ControlPin);
  Dynamixel.setMode(ID1, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_MX);
  Dynamixel.setMode(ID2, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_MX);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  driveServo(0, 20, 0x3FF/8, 0x3FF/5);
}

/*
sampah1 = ferro //"1"
sampah2 = non-fero //"2"
sampah3 = plastik //"3"
sampah4 = kertas //"4"
sampah5 = daun //"5"
*/

void loop() {
  //  Gas (20, 150);
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    int spaceIndex = input.indexOf(' ');
    if (spaceIndex != -1) {
      float x = input.substring(0, spaceIndex).toFloat();
      float y = input.substring(spaceIndex + 1).toFloat();
      driveServo(x, y, 0x3FF/4, 0x3FF);
      delay(200);
    } else if (input == "1") {
      sampah1a();
    } else if (input == "2") {
      sampah2a();
    } else if (input == "3") {
      sampah3a();
    } else if (input == "4") {
      sampah4a();
    } else if (input == "5") {
      sampah5a();
    } else if (input == "a") {
      sampah1b();
    } else if (input == "b") {
      sampah2b();
    } else if (input == "c") {
      sampah3b();
    } else if (input == "d") {
      sampah4b();
    } else if (input == "e") {
      sampah5b();
    }
  }
}
