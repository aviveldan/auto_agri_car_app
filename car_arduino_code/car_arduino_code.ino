// A list of libraries that need to be installed:
// NewPing, AccelStepper

//#include <NewPing.h>
#include <Servo.h>

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

#define NUM_OF_STEPS_IN_METER 1100
#define NUM_OF_STEPS_IN_360_DEGREES 825
#define CAR_SPEED 5000
#define TURNING_SPEED 3000

// Ultrasonic
//#define ULTRASONIC_TRIG_PIN A0 // Reset/Abort
//#define ULTRASONIC_ECHO_PIN A1 // Feed Hold
//#define MAX_DISTANCE 200

// Watering stuff
#define PUMP_PIN A2 // Cycle Start/Resume - YELLOW
#define SERVO_PIN A0 // Coolant Enable - ORANGE
#define MOUISTURE_PIN A4 // SDA - BLUE

#define SERVO_MAX_DEGREE 70
#define SERVO_WATERING_DEGREE 120
#define ENOUGH_MOISTURE_PERCENTAGE 70
#define BIG_WATER_DELAY_TIME 400
#define MEDIUM_WATER_DELAY_TIME 250
#define SMALL_WATER_DELAY_TIME 150
#define AMOUNT_OF_REFILL 9500 // From 11000

// Action Enum
#define FORWARD 0
#define TURN_RIGHT 1
#define TURN_LEFT 2
#define STOP 3
#define WATER_A_PLANT 4
#define REFILL 5

//NewPing sonar(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN, MAX_DISTANCE);
Servo servo;

// Constants

int distance = 100;
float action = -1;
float amount = -1;
double amount_of_water = 1;

void setup() {
  delay(50);
  Serial.begin(9600);
  digitalWrite(PUMP_PIN, HIGH); // disablePump();
  
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
  if(Serial.available()>0)
  {
    action = Serial.parseFloat();
    delay(50);
    amount = Serial.parseFloat();
    delay(50);
    
    switch(int(action))
    {
      case FORWARD:
//        Serial.print("Forward: ");
//        Serial.println(amount);
        moveForward(amount);
        break;
      case TURN_RIGHT:
//        Serial.print("Right: ");
//        Serial.println(amount);
        turnRight(amount);
        break;
      case TURN_LEFT:
//        Serial.print("Left: ");
//        Serial.println(amount);
        turnLeft(amount);
        break;
      case STOP:
//        Serial.print("Stop: ");
//        Serial.println(amount);
        moveStop();
        break;
      case WATER_A_PLANT:
//        Serial.print("Water: ");
//        Serial.println(amount);
        waterAPlant();
        break;
      case REFILL:
        amount_of_water = AMOUNT_OF_REFILL;
        delay(50000);
        break;
    }
    if(amount_of_water <= 0)
    { 
      delay(5000);
      sendNeedRefill();
      delay(3000);
    }
    else
    {
      Serial.print(0);
      delay(10);
      Serial.println("\n");
    }
  }
  delay(50);

//  // First Line
//  delay(3000);
//  moveForward(50);
//  delay(50);
////  turnRight(90);
////  delay(50);
//  waterAPlant();
//  delay(50);
////  turnLeft(90);
////  delay(50);
////  moveForward(25);
////  delay(50);
//  turnLeft(90);
//  delay(50);
//  // Second Line
//  moveForward(50);
//  delay(50);
////  turnRight(90);
////  delay(50);
//  waterAPlant();
//  delay(50);
////  turnLeft(90);
////  delay(50);
////  moveForward(25);
////  delay(50);
//  turnLeft(90);
//  delay(50);
//  // Third Line
//  moveForward(50);
//  delay(50);
////  turnRight(90);
////  delay(50);
//  waterAPlant();
//  delay(50);
////  turnLeft(90);
////  delay(50);
////  moveForward(25);
////  delay(50);
//  turnLeft(90);
//  delay(50);
//  // Fourth Line
//  moveForward(50);
//  delay(50);
////  turnRight(90);
////  delay(50);
//  waterAPlant();
//  delay(50);
////  turnLeft(90);
////  delay(50);
////  moveForward(25);
////  delay(50);
//  turnLeft(90);
//  delay(50);

//  delay(2000);
//  moveForward(25);
//  waterAPlant();
//  moveForward(25);
//  delay(10000000);
}

// Ultrasonic Functions

//int readDistance() { 
//  delay(70);
//  int cm = sonar.ping_cm();
//  if(cm==0)
//  {
//    cm = 250;
//  }
//  return cm;
//}

// Motors Functions

int convertCMetersToSteps(double cmeters)
{
  double temp = (cmeters/100)*NUM_OF_STEPS_IN_METER;
  return temp;
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

void moveForward(float distance_in_cmeters) 
{
  digitalWrite(FL_MOTORX_DIR, LOW);
  digitalWrite(FR_MOTORY_DIR, HIGH);
  digitalWrite(BL_MOTORZ_DIR, LOW);
  digitalWrite(BR_MOTORA_DIR, HIGH);
  
  int distance_in_steps = convertCMetersToSteps(distance_in_cmeters);
  moveInCurrentDir(distance_in_steps, false);
  delay(5);
}

void moveBackward(float distance_in_cmeters) 
{
  digitalWrite(FL_MOTORX_DIR, HIGH);
  digitalWrite(FR_MOTORY_DIR, LOW);
  digitalWrite(BL_MOTORZ_DIR, HIGH);
  digitalWrite(BR_MOTORA_DIR, LOW);

  int distance_in_steps = convertCMetersToSteps(distance_in_cmeters);
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
  delay(500);
}

void moveServoUp()
{
  servo.write(0);
  delay(500);
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

int moistureToPercent(int sensorValue)
{
  int percentage = map(sensorValue, 1023, 165, 0, 100);
  delay(50);
  return percentage;
}

int moistureReadInPercent()
{
//  int moisture_value = 0;
//  for(int i=0; i <= 100; ++i){
//    moisture_value += analogRead(MOUISTURE_PIN);
//    delay(1);
//  }
//  moisture_value = moisture_value/100.0;
  int res = moistureToPercent(analogRead(MOUISTURE_PIN));
  return res;
}

void poureWater(int delayTime){
  activatePump();
  delay(delayTime);
  disablePump();
}

void sendNeedRefill()
{
  Serial.print(1);
  delay(10);
  Serial.println("\n");
}

// Main Functions

void waterAPlant()
{
  moveServoToGround();
  float moisture_percent = moistureReadInPercent();
  if(moisture_percent < (ENOUGH_MOISTURE_PERCENTAGE/3))
  {
    activatePump();
    delay(BIG_WATER_DELAY_TIME);
    disablePump();
    amount_of_water = amount_of_water - BIG_WATER_DELAY_TIME;
  }
  else
  {
    if(moisture_percent < (ENOUGH_MOISTURE_PERCENTAGE/2))
    {
      activatePump();
      delay(MEDIUM_WATER_DELAY_TIME);
      disablePump();
      amount_of_water = amount_of_water - MEDIUM_WATER_DELAY_TIME;
    }
    else
    {
      if(moisture_percent < ENOUGH_MOISTURE_PERCENTAGE)
      {
        activatePump();
        delay(SMALL_WATER_DELAY_TIME);
        disablePump();
        amount_of_water = amount_of_water - SMALL_WATER_DELAY_TIME;
      }
    }
  }
  delay(300);
  moveServoUp();
}
