#include <Dynamixel_Serial.h>
#include <math.h>  // Include the math library for mathematical functions

// Define constants for servo IDs, control pin, baud rate, and angle limits
#define ID1 9u //mx1A
#define ID2 10u //mx1B
#define ID3 7u //mx2
// #define ID4 7u

#define SERVO_ControlPin 7
#define SERVO_SET_Baudrate 1000000
#define CW_LIMIT_ANGLE 0
#define CCW_LIMIT_ANGLE_MX 0xFFF
#define CCW_LIMIT_ANGLE_AX 0x3FF


// Calculate the center position of the servo
unsigned int tegak_mx = CCW_LIMIT_ANGLE_MX / 2;
unsigned int tegak_ax = CCW_LIMIT_ANGLE_AX / 2;

// Define the lengths of the links
float L1 = 31;    // 10.4 cm
float L2 = 30.5;  // 10.4 cm

// Function to drive the servos based on x and y coordinates
void driveServo(float x, float y) {
  // Calculate the angles for the servo positions using inverse kinematics
  float theta2 = acos((pow(x, 2) + pow(y, 2) - pow(L1, 2) - pow(L2, 2)) / (2 * L1 * L2));
  float theta1 = atan2(x, y) - atan2((L2 * sin(theta2)), (L1 + L2 * cos(theta2)));

  // Convert angles to servo positions in degrees
  float pos1 = theta1 * 57.2;
  float pos2 = theta2 * 57.2;

  // Also, set the speed for servo movement
  if (pos1 < 0 && pos2 > 0) {
    pos1 = (-1) * pos1;
    joint1(tegak_mx - Deg(pos1), 0x3FF);
    joint2(tegak_mx + Deg(pos1), 0x3FF);
    joint3(tegak_mx + Deg(pos2), 0x3FF);
    delay(5);
  } else if (pos1 > 0 && pos2 < 0) {
    pos2 = (-1) * pos2;
    joint1(tegak_mx + Deg(pos1), 0x3FF);
    joint2(tegak_mx - Deg(pos1), 0x3FF);
    joint3(tegak_mx - Deg(pos2), 0x3FF);
    delay(5);
  } else if (pos1 < 0 && pos2 < 0) {
    pos1 = (-1) * pos1;
    pos2 = (-1) * pos2;
    joint1(tegak_mx + Deg(pos1), 0x3FF);
    joint2(tegak_mx - Deg(pos1), 0x3FF);
    joint3(tegak_mx + Deg(pos2), 0x3FF);
    delay(5);
  } else if (pos1 > 0 && pos2 > 0) {
    joint1(tegak_mx + Deg(pos1), 0x3FF);
    joint2(tegak_mx - Deg(pos1), 0x3FF);
    joint3(tegak_mx + Deg(pos2), 0x3FF);
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
void joint3(unsigned int pos3, int speed) {
  Dynamixel.servo(ID3, pos3, speed);
  delay(5);
}

// Setup function to initialize Serial communication and Dynamixel servos
void setup() {
  Serial.begin(SERVO_SET_Baudrate);
  Serial.setTimeout(1);
  delay(500);
  pinMode(12, INPUT);
  Dynamixel.begin(SERVO_SET_Baudrate, SERVO_ControlPin);
  Dynamixel.setMode(ID1, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_MX);
  Dynamixel.setMode(ID2, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_MX);
  Dynamixel.setMode(ID3, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_AX);
  Dynamixel.setMode(ID4, SERVO, CW_LIMIT_ANGLE, CCW_LIMIT_ANGLE_AX);
  joint1(tegak_mx - Deg(80), 512);
  joint2(tegak_mx + Deg(80), 512);
  joint3(tegak_mx + Deg(180), 512);
  // Dynamixel.servo(ID4, tegak_ax, 0x3FF);
}


void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    int spaceIndex = input.indexOf(' ');
    if (spaceIndex != -1) {
      float x = input.substring(0, spaceIndex).toFloat();
      float y = input.substring(spaceIndex + 1).toFloat();
      driveServo(x, y);
    } else if (input == "Naik") {
      Dynamixel.servo(ID3, tegak_ax, 0x3FF);
    } else if (input == "Turun") {
      Dynamixel.servo(ID3, tegak_ax + 500, 0x3FF);
    }
  }
}