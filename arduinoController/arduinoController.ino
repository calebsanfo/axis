// MultiStepper.pde
// -*- mode: C++ -*-
// Use MultiStepper class to manage multiple steppers and make them all move to 
// the same position at the same time for linear 2d (or 3d) motion.

#include <AccelStepper.h>
#include <MultiStepper.h>

#define LIMIT_X 9
#define LIMIT_Y 10
#define LIMIT_Z 11

#define STEPS_PER_MM 10

#define XY_SPEED 1000
#define XY_ACCEL 1000
#define Z_SPEED  400
#define Z_ACCEL  200
#define XY_SPEED_SLOW 200
#define XY_ACCEL_SLOW 200
#define Z_SPEED_SLOW  200
#define Z_ACCEL_SLOW  200


AccelStepper stepperX(1, 3, 2);
AccelStepper stepperY(1, 5, 4);
AccelStepper stepperZ(1, 7, 6);

// Up to 10 steppers can be handled as a group by MultiStepper
MultiStepper steppers;

long current_positions[3] = {0,0,0};

String readString;

void setup() {
  Serial.begin(9600);

  // Set the limit pins
  pinMode(LIMIT_X, INPUT_PULLUP);
  pinMode(LIMIT_Y, INPUT_PULLUP);
  pinMode(LIMIT_Z, INPUT_PULLUP);

  stepperX.setMaxSpeed(XY_SPEED);
  stepperX.setAcceleration(XY_ACCEL);
  stepperY.setMaxSpeed(XY_SPEED);
  stepperY.setAcceleration(XY_ACCEL);
  stepperZ.setMaxSpeed(Z_SPEED);
  stepperZ.setAcceleration(Z_ACCEL);

  // Then give them to MultiStepper to manage
  steppers.addStepper(stepperX);
  steppers.addStepper(stepperY);
  steppers.addStepper(stepperZ);
}

/**
 * loop parses commands from serial and calls 
 * appropriate functions.
 * 
 * Possible commands are:
 * moveXY <x> <y>
 * moveZ <z>
 * get x 
 * get y 
 * get z 
 * findhome
 * sethome
 * analogread
 * 
 */
void loop() {
  while (!Serial.available()) {} // wait for data to arrive

  while (Serial.available()) {
    if (Serial.available()) {
        char c = Serial.read();
        readString += c;
        delay(2);
    }
  }

  char *readIn = (char *)readString.c_str();
  char *command = strtok(readIn, " ");
  char *param1 = strtok(NULL, " ");
  char *param2 = strtok(NULL, " ");

  if (strcmp(command, "moveXY") == 0) {
    setXY(atof(param1), atof(param2));
    Serial.println(1);
  } else if (strcmp(command, "moveZ") == 0) {
    setZ(atof(param1));
    Serial.println(1);
  } else if(strcmp(command, "get") == 0) {
         if (strcmp(param1, "x") == 0)    Serial.println(current_positions[0] / STEPS_PER_MM);
    else if (strcmp(param1, "y") == 0)    Serial.println(current_positions[1] / STEPS_PER_MM);
    else if (strcmp(param1, "z") == 0)    Serial.println(current_positions[2] / STEPS_PER_MM);
  } else if (strcmp(command, "findhome") == 0) {
    findHome();
    Serial.println(1);
  } else if (strcmp(command, "sethome") == 0) {
    setHome();
    Serial.println(1);
  } else if (strcmp(command, "analogread") == 0) {
    Serial.println(analogRead(A0));
  }

  Serial.flush();
  readString = "";
}

void setXY(float x, float y){
  current_positions[0] = x;
  current_positions[1] = y;
  steppers.moveTo(current_positions);
  steppers.runSpeedToPosition();
}

void setZ(float z){
  current_positions[2] = z;
  steppers.moveTo(current_positions);
  steppers.runSpeedToPosition();
}

/**
 * Move the steppers until the limit switches are reached
 * 
 * Modified code found here:
 * https://www.brainy-bits.com/setting-stepper-motors-home-position-using-accelstepper/
 */
void findHome() {
  // Set the current position to the home position
  setHome();
  // Slow down the steppers to get better accuracy
  stepperX.setMaxSpeed(XY_SPEED_SLOW);
  stepperX.setAcceleration(XY_ACCEL_SLOW);
  stepperY.setMaxSpeed(XY_SPEED_SLOW);
  stepperY.setAcceleration(XY_ACCEL_SLOW);
  stepperZ.setMaxSpeed(Z_SPEED_SLOW);
  stepperZ.setAcceleration(Z_ACCEL_SLOW);

  // Move until all axis are pressing the limit switch
  while (digitalRead(LIMIT_X) || digitalRead(LIMIT_Y) || digitalRead(LIMIT_Z)) { 
    if (digitalRead(LIMIT_X)) { current_positions[0]--; } 
    if (digitalRead(LIMIT_Y)) { current_positions[1]--; } 
    if (digitalRead(LIMIT_Z)) { current_positions[2]--; }
    steppers.moveTo(current_positions);
    steppers.run();
    delay(5);
  }

  setHome();

  // Move until all axis are no longer pressing the limit switch
  while (!digitalRead(LIMIT_X) || !digitalRead(LIMIT_Y) || !digitalRead(LIMIT_Z)) { 
    if (!digitalRead(LIMIT_X)) { current_positions[0]++; } 
    if (!digitalRead(LIMIT_Y)) { current_positions[1]++; } 
    if (!digitalRead(LIMIT_Z)) { current_positions[2]++; }
    steppers.moveTo(current_positions);
    steppers.run();
    delay(5);
  }
  
  setHome();
  // Set the speed and acceleration to their previous values
  stepperX.setMaxSpeed(XY_SPEED_SLOW);
  stepperX.setAcceleration(XY_ACCEL_SLOW);
  stepperY.setMaxSpeed(XY_SPEED_SLOW);
  stepperY.setAcceleration(XY_ACCEL_SLOW);
  stepperZ.setMaxSpeed(Z_SPEED_SLOW);
  stepperZ.setAcceleration(Z_ACCEL_SLOW);
}

/**
 * Set the position of all motors to zero
 */
void setHome() {
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperZ.setCurrentPosition(0);
  current_positions[0] = 0;
  current_positions[1] = 0;
  current_positions[2] = 0;
}
