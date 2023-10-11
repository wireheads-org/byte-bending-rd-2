#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

//PIN assignments for the components
const int LED_PIN = 13;
const int MOTOR_X_STEP_PIN = 26;
const int MOTOR_X_DIRECTION_PIN = 28;
const int MOTOR_X_ENABLE_PIN = 24;
const int MOTOR_Y_STEP_PIN = A6;
const int MOTOR_Y_DIRECTION_PIN = A7;
const int MOTOR_Y_ENABLE_PIN = A2;

// Define the stepper motor and the pins that is connected to
AccelStepper stepper1(1, MOTOR_X_STEP_PIN, MOTOR_X_DIRECTION_PIN); // (Typeof driver: with 2 pins, STEP, DIR)
AccelStepper stepper2(1, MOTOR_Y_STEP_PIN, MOTOR_Y_DIRECTION_PIN);

MultiStepper steppersControl;  // Create instance of MultiStepper

long gotoposition[2]; // An array to store the target positions for each stepper motor

void setup() {

  //enabling the motors & their drivers
  pinMode(MOTOR_X_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_X_ENABLE_PIN, LOW);
  pinMode(MOTOR_Y_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_Y_ENABLE_PIN, LOW);

  stepper1.setMaxSpeed(1000); // Set maximum speed value for the stepper
  stepper2.setMaxSpeed(1000);

  // Adding the 2 steppers to the steppersControl instance for multi stepper control
  steppersControl.addStepper(stepper1);
  steppersControl.addStepper(stepper2);
}

void loop() {
  // Store the target positions in the "gotoposition" array
  gotoposition[0] = 6250;
  gotoposition[1] = 18750;

  steppersControl.moveTo(gotoposition); // Calculates the required speed for all motors
  steppersControl.runSpeedToPosition(); // Blocks until all steppers are in position

  delay(1000);

  gotoposition[0] = 0;
  gotoposition[1] = 0;

  steppersControl.moveTo(gotoposition);
  steppersControl.runSpeedToPosition();

  delay(1000);
}