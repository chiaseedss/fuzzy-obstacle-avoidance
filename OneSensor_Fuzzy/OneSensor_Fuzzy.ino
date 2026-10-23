#include <Servo.h>
#include <NewPing.h>

//Sensor Pins
#define trig_pin A2
#define echo_pin A1
#define maximum_distance 500

//Motor Pins
const int in1 = 5;
const int in2 = 6;
const int in3 = 7;
const int in4 = 8;

//PWM Pins for Speed Control 
const int enA = A3; 
const int enB = A4; 

//Global Variables
boolean goesForward = false;
int distance = 0;
int motorSpeed = 130;  

NewPing sonar(trig_pin, echo_pin, maximum_distance);
Servo servo_motor;

void setup() {
  Serial.begin(9600);
  servo_motor.attach(3);
  
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  servo_motor.write(150); 
  delay(2000);

  for (int i = 0; i < 4; i++) {
    distance = readPing();
    delay(100);
  }

  Serial.print("Distance: ");
  Serial.println(distance);
}

void loop() {
  int distanceR = 0;
  int distanceL = 0;
  delay(1);

  Serial.print("Current Distance: ");
  Serial.println(distance);

  if (distance <= 50) {
    moveStop();
    delay(500);
    reverseRun();
    delay(500);
    moveStop();
    delay(200);

    distanceR = lookRight();
    Serial.print("Right distance: ");
    Serial.println(distanceR);
    delay(500);

    distanceL = lookLeft();
    Serial.print("Left distance: ");
    Serial.println(distanceL);
    delay(200);

    String turnDirection = fuzzyTurnDecision(distanceR, distanceL);
    Serial.print("Fuzzy turn decision: ");
    Serial.println(turnDirection);

    if (turnDirection == "RIGHT") {
      moveRight();
    } else {
      moveLeft();
    }
    delay(500);
    moveStop();
  } else {
    Serial.println("Path clear, moving forward");
    moveForward();
  }

  distance = readPing();
  delay(100);
}

// Fuzzy Logic Decision
String fuzzyTurnDecision(int distR, int distL) {
  float nearR = 0.0, farR = 0.0;
  float nearL = 0.0, farL = 0.0;

  // Fuzzify right distance
  if (distR <= 20) nearR = 1;
  else if (distR > 20 && distR <= 50) {
    nearR = (50 - distR) / 30.0;
    farR = (distR - 20) / 30.0;
  } else farR = 1;

  // Fuzzify left distance
  if (distL <= 20) nearL = 1;
  else if (distL > 20 && distL <= 50) {
    nearL = (50 - distL) / 30.0;
    farL = (distL - 20) / 30.0;
  } else farL = 1;

  // Fuzzy rules:
  float ruleTurnRight = min(farR, nearL);
  float ruleTurnLeft = min(nearR, farL);
  float ruleForward = min(farR, farL);
  float ruleReverse = min(nearR, nearL);

  // Find dominant rule
  if (ruleForward >= ruleTurnLeft && ruleForward >= ruleTurnRight && ruleForward >= ruleReverse)
    return "FORWARD";
  else if (ruleTurnRight >= ruleTurnLeft && ruleTurnRight >= ruleReverse)
    return "RIGHT";
  else if (ruleTurnLeft >= ruleReverse)
    return "LEFT";
  else
    return "REVERSE";
}

// Sensor Functions
int lookRight() {
  servo_motor.write(100);
  delay(500);
  int dist = readPing();
  delay(100);
  servo_motor.write(150);  
  return dist;
}

int lookLeft() {
  servo_motor.write(200);  
  delay(500);
  int dist = readPing();
  delay(100);
  servo_motor.write(150);  
  return dist;
}

int readPing() {
  delay(70);
  int cm = sonar.ping_cm();
  if (cm == 0) cm = 500; 
  return cm;
}

//Movement Functions
void moveForward() {
  Serial.println("moveForward() called");
  goesForward = true;
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void reverseRun() {
  Serial.println("reverseRun() called");
  goesForward = false;
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void moveStop() {
  Serial.println("moveStop() called");
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void moveLeft() {
  Serial.println("moveLeft() called");
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void moveRight() {
  Serial.println("moveRight() called");
  analogWrite(enA, motorSpeed);
  analogWrite(enB, motorSpeed);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}