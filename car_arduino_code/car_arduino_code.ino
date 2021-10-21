// A list of libraries that need to be installed:
// NewPing, AccelStepper

#include <NewPing.h>
#include <Servo.h>
#include <SoftwareSerial.h>
//#include <AccelStepper.h>

// Global Variables and macros

// Motors
#define FL_MOTORX_STEP 2
#define FL_MOTORX_DIR 5
#define FR_MOTORY_STEP 3
#define FR_MOTORY_DIR 6
#define BL_MOTORZ_STEP 4
#define BL_MOTORZ_DIR 7
#define BR_MOTORA_STEP 12
#define BR_MOTORA_DIR 13

#define NUM_OF_STEPS_IN_METER 980
#define NUM_OF_STEPS_IN_360_DEGREES 900
#define CAR_SPEED 5000
#define TURNING_SPEED 3000

// Ultrasonic
#define ULTRASONIC_TRIG_PIN A0 // Reset/Abort
#define ULTRASONIC_ECHO_PIN A1 // Feed Hold
#define MAX_DISTANCE 200

// Watering stuff
#define PUMP_PIN A2 // Cycle Start/Resume - YELLOW
#define SERVO_PIN A3 // Coolant Enable - ORANGE
#define MOUISTURE_PIN A4 // Reserved - BLUE

#define SERVO_MAX_DEGREE 70
#define SERVO_WATERING_DEGREE 40
#define ENOUGH_MOISTURE_PERCENTAGE 70 // TODO change that to an accurate number
#define WATER_DELAY_TIME 500

// Action Enum
#define FORWARD 0
#define TURN_RIGHT 1
#define TURN_LEFT 2
#define STOP 3
#define WATER_A_PLANT 4

NewPing sonar(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN, MAX_DISTANCE);
Servo servo;
SoftwareSerial ArduinoUno(0,1);

//AccelStepper stepperFR(AccelStepper::DRIVER, FR_MOTORY_STEP, FR_MOTORY_DIR); // Front-Right
//AccelStepper stepperFL(AccelStepper::DRIVER, FL_MOTORX_STEP, FL_MOTORX_DIR); // Front-Left
//AccelStepper stepperBR(AccelStepper::DRIVER, BR_MOTORA_STEP, BR_MOTORA_DIR); // Back-Right
//AccelStepper stepperBL(AccelStepper::DRIVER, BL_MOTORZ_STEP, BL_MOTORZ_DIR); // Back-Left

// Constants

int distance = 100;
float action = -1;
float amount = -1;

void setup() {
  Serial.begin(9600);
  ArduinoUno.begin(9600);
  disablePump();
  
//  stepperFR.setMaxSpeed(100.0);
//  stepperFR.setAcceleration(100.0);
//  stepperFL.setMaxSpeed(100.0);
//  stepperFL.setAcceleration(100.0);
//  stepperBR.setMaxSpeed(100.0);
//  stepperBR.setAcceleration(100.0);
//  stepperBL.setMaxSpeed(100.0);
//  stepperBL.setAcceleration(100.0);
  
  pinMode(FL_MOTORX_STEP, OUTPUT);
  pinMode(FL_MOTORX_DIR, OUTPUT);
  pinMode(FR_MOTORY_STEP, OUTPUT);
  pinMode(FR_MOTORY_DIR, OUTPUT);
  pinMode(BL_MOTORZ_STEP, OUTPUT);
  pinMode(BL_MOTORZ_DIR, OUTPUT);
  pinMode(BR_MOTORA_STEP, OUTPUT);
  pinMode(BR_MOTORA_DIR, OUTPUT);

  pinMode(8, OUTPUT); // Enable Steppers
  digitalWrite(8, LOW);
  pinMode(PUMP_PIN, OUTPUT);
  servo.attach(SERVO_PIN);
  servo.write(0);
}

void loop() 
{
  if(ArduinoUno.available()>0)
  {
    action = ArduinoUno.parseFloat();
    delay(50);
    amount = ArduinoUno.parseFloat();
    delay(50);
    switch(int(action))
    {
      case FORWARD:
        moveForward(amount);
        break;
      case TURN_RIGHT:
        turnRight(amount);
        break;
      case TURN_LEFT:
        turnLeft(amount);
        break;
      case STOP:
        moveStop();
        break;
      case WATER_A_PLANT:
        waterAPlant();
        break;
    }
    delay(100);
  }
  
//  delay(2000);
//  Serial.print("The num of steps: ");
//  Serial.print(degree_test);
//  Serial.print(" and the actual number of steps is: ");
  delay(2000);
  turnRight(90.0);
  delay(2000);
  turnLeft(90.0);
//  degree_test = degree_test + 150;
  
//  delay(5000);
//  moveForward(0.26);
//  delay(50);
//  turnLeft(90);
//  delay(1000);
//  turnRight(90);
//  delay(50);
//  moveForward(0.17);
//  delay(50);
//  turnRight(90);
//  delay(1000);
//  turnLeft(90);
//  delay(50);
//  moveForward(0.41);
//  delay(50);
//  turnLeft(90);
//  delay(1000);
//  turnRight(90);
//  delay(50);
//  moveForward(0.20);
//  delay(50);
//  turnRight(90);
//  delay(1000);
//  turnLeft(90);
//  delay(50);
//  moveBackward(1.04);
//  delay(100000000000000);
}

// Ultrasonic Functions

int readDistance() { 
  delay(70);
  int cm = sonar.ping_cm();
  if(cm==0)
  {
    cm = 250;
  }
  return cm;
}

// Motors Functions

int convertMetersToSteps(float meters)
{
  return meters*NUM_OF_STEPS_IN_METER;
}

void moveStop() 
{
  digitalWrite(FL_MOTORX_STEP, LOW);
  digitalWrite(FR_MOTORY_STEP, LOW);
  digitalWrite(BL_MOTORZ_STEP, LOW);
  digitalWrite(BR_MOTORA_STEP, LOW);
} 

void moveInCurrentDir(int distance_in_steps, bool is_turning)
{
  int car_speed;
  if(is_turning == true)
  {
    car_speed = TURNING_SPEED;
  }
  else
  {
    car_speed = CAR_SPEED;
  }
  for(int i = 0; i<distance_in_steps; i++) 
  {
    digitalWrite(FL_MOTORX_STEP,HIGH);
    digitalWrite(FR_MOTORY_STEP,HIGH);
    digitalWrite(BL_MOTORZ_STEP,HIGH);
    digitalWrite(BR_MOTORA_STEP,HIGH);
    delayMicroseconds(car_speed);
    digitalWrite(FL_MOTORX_STEP,LOW);
    digitalWrite(FR_MOTORY_STEP,LOW);
    digitalWrite(BL_MOTORZ_STEP,LOW);
    digitalWrite(BR_MOTORA_STEP,LOW);
    delayMicroseconds(car_speed);
  }
}

void moveForward(float distance_in_meters) 
{
  digitalWrite(FL_MOTORX_DIR, LOW);
  digitalWrite(FR_MOTORY_DIR, HIGH);
  digitalWrite(BL_MOTORZ_DIR, LOW);
  digitalWrite(BR_MOTORA_DIR, HIGH);
  
  int distance_in_steps = convertMetersToSteps(distance_in_meters);
  moveInCurrentDir(distance_in_steps, false);
  delay(5);
}

void moveBackward(float distance_in_meters) 
{
  digitalWrite(FL_MOTORX_DIR, HIGH);
  digitalWrite(FR_MOTORY_DIR, LOW);
  digitalWrite(BL_MOTORZ_DIR, HIGH);
  digitalWrite(BR_MOTORA_DIR, LOW);

  int distance_in_steps = convertMetersToSteps(distance_in_meters);
  moveInCurrentDir(distance_in_steps, false);
  delay(5);
}

int convertDegreeToSteps(double degree)
{
  double temp = degree / 360.0;
  temp = temp * NUM_OF_STEPS_IN_360_DEGREES;
  return temp;
}

void turnLeft(double degree)
{
  digitalWrite(FL_MOTORX_DIR, HIGH);
  digitalWrite(FR_MOTORY_DIR, HIGH);
  digitalWrite(BL_MOTORZ_DIR, HIGH);
  digitalWrite(BR_MOTORA_DIR, HIGH);

  int num_of_steps = convertDegreeToSteps(degree);
  moveInCurrentDir(num_of_steps, true);
  delay(5);
}

void turnRight(double degree)
{  
  digitalWrite(FL_MOTORX_DIR, LOW);
  digitalWrite(FR_MOTORY_DIR, LOW);
  digitalWrite(BL_MOTORZ_DIR, LOW);
  digitalWrite(BR_MOTORA_DIR, LOW);
     
  int num_of_steps = convertDegreeToSteps(degree);
//  Serial.println(num_of_steps);
  moveInCurrentDir(num_of_steps, true);
  delay(5);
}

// Servo Functions

void moveServoToGround()
{
  servo.write(SERVO_WATERING_DEGREE);
  delay(1000);
}

void moveServoUp()
{
  servo.write(0);
  delay(1000);
}

// Pump Functions

void activatePump() {
  delay(50);
  digitalWrite(PUMP_PIN, LOW);
}

void disablePump() {
  delay(50);
  digitalWrite(PUMP_PIN, HIGH);
}

int moistureToPercent(int sensorValue){
  int percentage = map(sensorValue, 1023, 165, 0, 100);
  delay(50);
  return percentage;
}

int moistureReadInPercent()
{
  int moisture_value = 0;
  for(int i=0; i <= 100; ++i){
    moisture_value += analogRead(MOUISTURE_PIN);
    delay(1);
  }
  moisture_value = moisture_value/100.0;
  return moistureToPercent(moisture_value);
}

void poureWater(int delayTime){
  activatePump();
  delay(delayTime);
  disablePump();
}

// Main Functions

void waterAPlant()
{
  moveServoToGround();
  float moisture_percent = moistureReadInPercent();
  while(moisture_percent < ENOUGH_MOISTURE_PERCENTAGE)
  {
    poureWater(WATER_DELAY_TIME);
    moisture_percent = moistureReadInPercent();
  }
  moveServoUp();
}
