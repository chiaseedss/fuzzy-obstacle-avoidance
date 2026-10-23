#include <Fuzzy.h>
#include <NewPing.h>

// Sensor Pins
#define S1_TRIG A5 // Left
#define S1_ECHO A4
#define S2_TRIG A3 // Front
#define S2_ECHO A2
#define S3_TRIG A1 // Right
#define S3_ECHO A0

// Motor Driver Pins
#define IN1 5
#define IN2 6
#define IN3 7
#define IN4 8
#define ENA 3
#define ENB 10

#define MAX_DISTANCE 200
#define MOTOR_SPEED 130

NewPing sonarLeft(S1_TRIG, S1_ECHO, MAX_DISTANCE);
NewPing sonarFront(S2_TRIG, S2_ECHO, MAX_DISTANCE);
NewPing sonarRight(S3_TRIG, S3_ECHO, MAX_DISTANCE);

Fuzzy* fuzzy = new Fuzzy();

void setupFuzzy() {
  // Inputs
  FuzzyInput* frontDist = new FuzzyInput(1);
  FuzzySet* frontClose = new FuzzySet(0, 0, 15, 30);
  FuzzySet* frontFar = new FuzzySet(20, 50, 200, 200);
  frontDist->addFuzzySet(frontClose);
  frontDist->addFuzzySet(frontFar);
  fuzzy->addFuzzyInput(frontDist);

  FuzzyInput* leftDist = new FuzzyInput(2);
  FuzzySet* leftClose = new FuzzySet(0, 0, 15, 30);
  FuzzySet* leftFar = new FuzzySet(20, 50, 200, 200);
  leftDist->addFuzzySet(leftClose);
  leftDist->addFuzzySet(leftFar);
  fuzzy->addFuzzyInput(leftDist);

  FuzzyInput* rightDist = new FuzzyInput(3);
  FuzzySet* rightClose = new FuzzySet(0, 0, 15, 30);
  FuzzySet* rightFar = new FuzzySet(20, 50, 200, 200);
  rightDist->addFuzzySet(rightClose);
  rightDist->addFuzzySet(rightFar);
  fuzzy->addFuzzyInput(rightDist);

  // Output
  FuzzyOutput* direction = new FuzzyOutput(1);
  FuzzySet* dirStop = new FuzzySet(0, 0, 10, 10);
  FuzzySet* dirForward = new FuzzySet(10, 20, 30, 30);
  FuzzySet* dirLeft = new FuzzySet(30, 40, 50, 50);
  FuzzySet* dirRight = new FuzzySet(50, 60, 70, 70);
  FuzzySet* dirReverse = new FuzzySet(70, 80, 90, 90);
  direction->addFuzzySet(dirStop);
  direction->addFuzzySet(dirForward);
  direction->addFuzzySet(dirLeft);
  direction->addFuzzySet(dirRight);
  direction->addFuzzySet(dirReverse);
  fuzzy->addFuzzyOutput(direction);

  // Rules
  // Rule 1: IF Front is Far THEN Go Forward
  FuzzyRuleAntecedent* rule1Antecedent = new FuzzyRuleAntecedent();
  rule1Antecedent->joinSingle(frontFar);
  FuzzyRuleConsequent* rule1Consequent = new FuzzyRuleConsequent();
  rule1Consequent->addOutput(dirForward);
  FuzzyRule* fuzzyRule1 = new FuzzyRule(1, rule1Antecedent, rule1Consequent);
  fuzzy->addFuzzyRule(fuzzyRule1);

  // Rule 2: IF Front is Close AND Left is Far THEN Turn Left
  FuzzyRuleAntecedent* rule2Antecedent = new FuzzyRuleAntecedent();
  rule2Antecedent->joinWithAND(frontClose, leftFar);
  FuzzyRuleConsequent* rule2Consequent = new FuzzyRuleConsequent();
  rule2Consequent->addOutput(dirLeft);
  FuzzyRule* fuzzyRule2 = new FuzzyRule(2, rule2Antecedent, rule2Consequent);
  fuzzy->addFuzzyRule(fuzzyRule2);

  // Rule 3: IF Front is Close AND Right is Far THEN Turn Right
  FuzzyRuleAntecedent* rule3Antecedent = new FuzzyRuleAntecedent();
  rule3Antecedent->joinWithAND(frontClose, rightFar);
  FuzzyRuleConsequent* rule3Consequent = new FuzzyRuleConsequent();
  rule3Consequent->addOutput(dirRight);
  FuzzyRule* fuzzyRule3 = new FuzzyRule(3, rule3Antecedent, rule3Consequent);
  fuzzy->addFuzzyRule(fuzzyRule3);

  // Rule 4: IF Front is Close AND Left is Close AND Right is Close THEN Reverse
  FuzzyRuleAntecedent* rule4Antecedent1 = new FuzzyRuleAntecedent();
  rule4Antecedent1->joinWithAND(frontClose, leftClose);
  FuzzyRuleAntecedent* rule4Antecedent = new FuzzyRuleAntecedent();
  rule4Antecedent->joinWithAND(rule4Antecedent1, rightClose);
  FuzzyRuleConsequent* rule4Consequent = new FuzzyRuleConsequent();
  rule4Consequent->addOutput(dirReverse);
  FuzzyRule* fuzzyRule4 = new FuzzyRule(4, rule4Antecedent, rule4Consequent);
  fuzzy->addFuzzyRule(fuzzyRule4);
}

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  setupFuzzy();
}

void loop() {
  int distFront = sonarFront.ping_cm();
  int distLeft = sonarLeft.ping_cm();
  int distRight = sonarRight.ping_cm();

  if (distFront == 0) distFront = 200;
  if (distLeft == 0) distLeft = 200;
  if (distRight == 0) distRight = 200;

  Serial.print("Front: "); Serial.print(distFront);
  Serial.print(" | Left: "); Serial.print(distLeft);
  Serial.print(" | Right: "); Serial.println(distRight);

  fuzzy->setInput(1, distFront);
  fuzzy->setInput(2, distLeft);
  fuzzy->setInput(3, distRight);

  fuzzy->fuzzify();
  float output = fuzzy->defuzzify(1);
  
  if (output < 10) {
    moveStop();
  } else if (output < 30) {
    moveForward();
  } else if (output < 50) {
    moveLeft();
  } else if (output < 70) {
    moveRight();
  } else {
    moveReverse();
  }

  delay(300);
}

void moveForward() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveReverse() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void moveLeft() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveRight() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void moveStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
