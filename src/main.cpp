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
struct MotorConfig 
{
    const int stepPin; // step pin of the motor driver
    const int directionPin; //  direction pin of motor driver
    const int enablePin; // enable pin of motor driver (optional)
    const float stepAngle; // step angle of motor (in degrees)
    const float distPerRev; // distance covered by motor in one revolution (in mm)
    const int rpm; // maximum rated RPM of motor
    const int distance; // total distance required to cover (in mm)
};

constexpr MotorConfig MOTOR_C = 
{
    A6, A7, A2, 1.8, 0.8, 400, 750
};

constexpr MotorConfig MOTOR_H = 
{
    26, 28, 24, 1.8, 0.8, 400, 250
};


// Solenoid Definitions
constexpr int SOLENOID_PIN = A9;
constexpr int SOLENOID_ACTUATION_TIME = 1000; // set the time for the cap to fix on the bottle (in ms)

// Function to calculate the total number of steps required
inline long calculateSteps(const MotorConfig& config) 
{
    return static_cast<long>(config.distance / config.distPerRev * (360.0 / config.stepAngle));
}

// Function to calculate the maximum speed for the motors without step loss (assuming the rated values given are correct)
inline float calculateMaxSpeed(const MotorConfig& config) 
{
    return (config.rpm * (360.0 / config.stepAngle)) / 60.0;
}


// Function to enable motor drivers
void setupMotorPins(int enablePin)
{
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, LOW);
}

// Function to operate the solenoid (activation & deactivation)
void operateSolenoid() 
{
  pinMode(SOLENOID_PIN, OUTPUT);
  digitalWrite(SOLENOID_PIN, HIGH);
  delay(SOLENOID_ACTUATION_TIME + 40); // set the delay for cap to fit on bottle with 40ms of addl. levy
  digitalWrite(SOLENOID_PIN, LOW);
  delay(SOLENOID_ACTUATION_TIME + 40); // set the delay for movement of head motor with 40ms of addl. levy
}

void setup()
{
  //Serial print if debug flag is set to 1
  debugspeed(9600);
  debug("The Bottle Cap Placing Test is Starting Now");
  
  // Enabling the drivers
  setupMotorPins(MOTOR_H.enablePin);
  setupMotorPins(MOTOR_C.enablePin);

  // Using the AccelStepper library for both motors
  AccelStepper stepper1(1, MOTOR_C.stepPin, MOTOR_C.directionPin); // def : (driver_type, step, dir)
  AccelStepper stepper2(1, MOTOR_H.stepPin, MOTOR_H.directionPin);

  // Set maximum speed value for the stepper
  stepper1.setMaxSpeed(calculateMaxSpeed(MOTOR_C));
  stepper2.setMaxSpeed(calculateMaxSpeed(MOTOR_H));

  // Adding the 2 steppers to the steppersControl instance for multi stepper control
  MultiStepper steppersControl; // Create an instance of MultiStepper
  steppersControl.addStepper(stepper1);
  steppersControl.addStepper(stepper2);

  // Calculate the required steps for each motor and save them in the array named positions
  long positions[] = 
  {
      calculateSteps(MOTOR_C),
      calculateSteps(MOTOR_H)
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

  operateSolenoid();

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