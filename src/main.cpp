// include the Arduino Library, AccelStepper Library (which includes MultiStepper.h too)
#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

//To enable DEBUG Mode set Flag to 1 (Debug Mode prints Serial data)
#define DEBUG 1

#if DEBUG == 1
#define debugspeed(x) Serial.begin(x)
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debugspeed(x)
#define debug(x)
#define debugln(x)
#endif

// Microcontroller Pin Definitions
const int MOTOR_C_STEP_PIN = A6;
const int MOTOR_C_DIRECTION_PIN = A7;
const int MOTOR_C_ENABLE_PIN = A2;
const int MOTOR_H_STEP_PIN = 26;
const int MOTOR_H_DIRECTION_PIN = 28;
const int MOTOR_H_ENABLE_PIN = 24;
const int SOLENOID_PIN = A9;

// Stepper Motor Definitions
const int MOTOR_C_DISTANCE = 750;     // total distance required to cover (in mm)
const float MOTOR_C_STEP_ANGLE = 1.8;   // step angle of motor (in degrees)
const float MOTOR_C_DIST_PER_REV = 0.8; // distance covered by motor in one revolution (in mm)
const int MOTOR_C_RPM = 400;          // maximum rated RPM of motor

const int MOTOR_H_DISTANCE = 250;     // total distance required to cover (in mm)
const float MOTOR_H_STEP_ANGLE = 1.8;   // step angle of motor (in degrees)
const float MOTOR_H_DIST_PER_REV = 0.8; // distance covered by motor in one revolution (in mm)
const int MOTOR_H_RPM = 400;          // maximum rated RPM of motor

// Solenoid Definitions
const int SOLENOID_ACTUATION_TIME = 1000; // set the time for the cap to fix on the bottle (in ms)

// Function to calculate the total number of steps required
inline long calculateSteps(float stepAngle, float distancePerRev, float totalDistance) 
{
    return static_cast<long>(totalDistance / distancePerRev * (360.0 / stepAngle));
}

// Function to calculate the maximum speed for the motors without step loss (assuming the rated values given are correct)
inline float calculateMaxSpeed(float rpm, float stepAngle)
{
    return (rpm * (360.0 / stepAngle)) / 60.0;
}

// Function to enable motor drivers
void setupMotorPins(int enablePin)
{
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, LOW);
}

// Using the AccelStepper library for both motors
AccelStepper stepper1(1, MOTOR_C_STEP_PIN, MOTOR_C_DIRECTION_PIN); // (driver_type, step_pin, direction_pin)
AccelStepper stepper2(1, MOTOR_H_STEP_PIN, MOTOR_H_DIRECTION_PIN);
MultiStepper steppersControl; // Create an instance of MultiStepper
long positions[2];         // An array to store the target positions for each stepper motor

void setup()
{
  //Serial print if debug flag is set to 1
  debugspeed(9600);
  debug("The Bottle Cap Placing Test is Starting Now");
  
  // Enabling the drivers
  setupMotorPins(MOTOR_H_ENABLE_PIN);
  setupMotorPins(MOTOR_C_ENABLE_PIN);

  // Set maximum speed value for the stepper
  stepper1.setMaxSpeed(calculateMaxSpeed(MOTOR_C_RPM, MOTOR_C_STEP_ANGLE));
  stepper2.setMaxSpeed(calculateMaxSpeed(MOTOR_H_RPM, MOTOR_H_STEP_ANGLE));

  // Adding the 2 steppers to the steppersControl instance for multi stepper control
  steppersControl.addStepper(stepper1);
  steppersControl.addStepper(stepper2);

  // Calculate the required steps for each motor and save them in the array named positions
  long positions[] = 
  {
      calculateSteps(MOTOR_C_STEP_ANGLE, MOTOR_C_DIST_PER_REV, MOTOR_C_DISTANCE),
      calculateSteps(MOTOR_H_STEP_ANGLE, MOTOR_H_DIST_PER_REV, MOTOR_H_DISTANCE)
  };

  //Print all calculated values for user reference
  debugln("Speed of Each Motor is (Hz): ");
  debugln(stepper1.maxSpeed());
  debugln(stepper2.maxSpeed());
  debugln("Total Steps of Each Motor is (Steps): ");
  debugln(positions[0]);
  debugln(positions[1]);

  // Sets the Stepper Motors to go to set position and blocks use until both steppers are engaged into position
  steppersControl.moveTo(positions);
  steppersControl.runSpeedToPosition();

  // solenoid activation and deactivation
  pinMode(SOLENOID_PIN, OUTPUT);
  digitalWrite(SOLENOID_PIN, HIGH);
  delay(SOLENOID_ACTUATION_TIME + 40); // set the delay for cap to fit on bottle with 40ms of addl. levy
  digitalWrite(SOLENOID_PIN, LOW);
  delay(SOLENOID_ACTUATION_TIME + 40); // set the delay for movement of head motor with 40ms of addl. levy

  // send the head motor with solenoid back to original position
  positions[1] = 0;
  steppersControl.moveTo(positions);
  steppersControl.runSpeedToPosition();

  // let the user know that the program has finished the operation
  delay(500);
  debugln("Bottle Cap Placing Test Completed Successfully");
  debugln("Code by Wireheads | T-Works Byte Bending Championship 2023 - Round 2");
}

void loop () 
{
  
}