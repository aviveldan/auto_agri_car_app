// A list of libraries that need to be installed:
// NewPing

#include <NewPing.h>

// Global Variables and macros

// Motors
#define MOTOR1_PIN1 2
#define MOTOR1_PIN2 3
#define MOTOR2_PIN1 4
#define MOTOR2_PIN2 5
#define MOTORS_SPEED_PIN 9
#define MIN_SPEED 120
#define MAX_SPEED 190

#define TURN_TIME 500

// Ultrasonic
#define ULTRASONIC_TRIG_PIN A0
#define ULTRASONIC_ECHO_PIN A1
#define MAX_DISTANCE 200

NewPing sonar(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN, MAX_DISTANCE);

int speedvar = MIN_SPEED;
int distance = 100;

void setup() {
  // put your setup code here, to run once:
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  pinMode(MOTORS_SPEED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(50);
  moveForward();
  
  if(distance < 20) {
	  moveBackward();
	  delay(200);
	  turnRight();
  }
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

void moveStop() {
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, LOW);
  } 

void moveForward() {
  analogWrite(MOTORS_SPEED_PIN, MIN_SPEED);
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, HIGH);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, HIGH);
     
  for(speedvar = MIN_SPEED; speedvar < MAX_SPEED; speedvar +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
  {
	distance = readDistance();
	if(distance <= 20) {
		moveStop();
		return;
	}
	analogWrite(MOTORS_SPEED_PIN, speedvar);
	delay(5);
  }
}

void moveBackward() {
  analogWrite(MOTORS_SPEED_PIN, MIN_SPEED);
  digitalWrite(MOTOR1_PIN1, HIGH);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, HIGH);
  digitalWrite(MOTOR2_PIN2, LOW);
     
  for(speedvar = MIN_SPEED; speedvar < MAX_SPEED; speedvar +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
  {
	//distance = readDistance();
	//if(distance <= 20) {
	//	moveStop();
	//	return;
	//}
	analogWrite(MOTORS_SPEED_PIN, speedvar);
	delay(5);
  }
}

void turnLeft() {
  analogWrite(MOTORS_SPEED_PIN, MAX_SPEED);
  
  digitalWrite(MOTOR1_PIN1, HIGH);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, HIGH);
     
  delay(TURN_TIME);
  moveStop();
  delay(100);
}

void turnRight() {
  analogWrite(MOTORS_SPEED_PIN, MAX_SPEED);
  
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, HIGH);
  digitalWrite(MOTOR2_PIN1, HIGH);
  digitalWrite(MOTOR2_PIN2, LOW);
     
  delay(TURN_TIME);
  moveStop();
  delay(100);
}
