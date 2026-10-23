// Define motor pins
#define LEFT_MOTOR_PIN1  11
#define LEFT_MOTOR_PIN2  10
#define RIGHT_MOTOR_PIN1 9
#define RIGHT_MOTOR_PIN2 8

// Define motor speed control pins
#define LEFT_MOTOR_SPEED_PIN 3
#define RIGHT_MOTOR_SPEED_PIN 5

// Define sensor pins
const int LEFTMOST_PIN = A0;
const int LEFT_PIN = A1;
const int MIDDLE_PIN = A2;
const int RIGHT_PIN = A3;
const int RIGHTMOST_PIN = A4;

unsigned long previousMillis = 0; // Store last time the loop ran
const unsigned long interval = 50; // Update every 50ms

void setup() {
  // Initialize motor pins as output
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED_PIN, OUTPUT);

  // Initialize sensor pins as input
  pinMode(LEFTMOST_PIN, INPUT);
  pinMode(LEFT_PIN, INPUT);
  pinMode(MIDDLE_PIN, INPUT);
  pinMode(RIGHT_PIN, INPUT);
  pinMode(RIGHTMOST_PIN, INPUT);

  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Timing Control for stable execution
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Checks if line (0) is detected
    bool FL = digitalRead(LEFTMOST_PIN) == 0; // Far Left
    bool NL = digitalRead(LEFT_PIN) == 0; // Near Left
    bool MID = digitalRead(MIDDLE_PIN) == 0; // Middle
    bool NR = digitalRead(RIGHT_PIN) == 0; // Near Right
    bool FR = digitalRead(RIGHTMOST_PIN) == 0; // Far Right

    Serial.print(NL);
    Serial.print(FL);
    Serial.print(MID);
    Serial.print(NR);
    Serial.print(FR);
    
    // Movement logic
    if (MID || (NL && NR)) { // Move forward when middle sensor or the near left and near right also detects black
      Serial.println("FORWARD");
      moveForward();
    } else if (NL || FL) { // Turn left when near left or far left detects black
      Serial.println("LEFT");
      turnLeft();
    } else if (NR || FR) { // Turn right when ear rightt or far right detects black
      Serial.println("RIGHT");
      turnRight();
    }
  }
}

// Movement functions
void moveForward() {
  digitalWrite(LEFT_MOTOR_PIN1, HIGH);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 140); 
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 140); 
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 80);  
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 100);
}

void turnRight() {
  digitalWrite(LEFT_MOTOR_PIN1, HIGH);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 100);  
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 80);
}