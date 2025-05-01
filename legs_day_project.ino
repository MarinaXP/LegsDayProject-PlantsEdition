#include <Servo.h>

// Servo instances for each leg joint
Servo LFR_A1, LFR_A2; // Front right leg
Servo LFL_A1, LFL_A2; // // Front left leg
Servo LBR_A1, LBR_A2; // Back right leg
Servo LBL_A1, LBL_A2; // Back left leg

// Pin assignments for servos
const int pinLFR_A1 = 2;
const int pinLFR_A2 = 3;
const int pinLFL_A1 = 4;
const int pinLFL_A2 = 5;
const int pinLBR_A1 = 6;
const int pinLBR_A2 = 7;
const int pinLBL_A1 = 8;
const int pinLBL_A2 = 9;

// Sensor pins
const int photoresistorPin = A0;    // Light sensor
const int trigPin = 11;             // Ultrasonic trigger
const int echoPin = 12;             // Ultrasonic echo

// Default servo position
const int posInitiale = 90;

// Delay between servo movements
const int waitTime = 300; // in milliseconds

// Sensor data
int lightMeasurement[3];        // To store light measurements in the 3 directions
int optimalDirection = 0;   // Direction with the most light (0=front, 1=right, 2=left)
long distance = 0;           // Distance measured by the ultrasonic sensor in cm
unsigned long haltTime = 0; // Timestamp for idle period

void setup() {
  // Initialization of serial communication
  Serial.begin(9600);
  Serial.println("Initializing the quadruped walking plant robot...");
  

  LFR_A1.attach(pinLFR_A1);
  LFR_A2.attach(pinLFR_A2);
  LFL_A1.attach(pinLFL_A1);
  LFL_A2.attach(pinLFL_A2);
  LBR_A1.attach(pinLBR_A1);
  LBR_A2.attach(pinLBR_A2);
  LBL_A1.attach(pinLBL_A1);
  LBL_A2.attach(pinLBL_A2);
  
  // Configure ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Move all servos to initial position (90°)
  reset_all_servos();
  
  // Give it time to bloom
  delay(2000);
  Serial.println("Program starting... Let’s photosynthesize!");
}

void loop() {
  // First, check if there is an obstacle
  distance = mesureDistance();
  
  if (distance < 5) {
    // Obstacle avoidance : if an obstacle is detected less than 5 cm away
    Serial.println("Obstacle detected at " + String(distance) + " cm");
    
    // Turn right until the path is clear (> 20 cm)
    while (distance <= 20) {
      turnRight();
      distance = mesureDistance();
      Serial.println("Rotation to avoid obstacle. Distance: " + String(distance) + " cm");
    }
    
    // Move forward
    for (int i = 0; i < 10; i++) {
      moveForward();
      distance = mesureDistance();
      // Stop if a new obstacle is detected
      if (distance < 5) break;
    }
  }
  else {
    // Normal light-seeking routine
    
    // Light measurement in the initial position
    int frontLight = analogRead(photoresistorPin);
    lightMeasurement[0] = frontLight;
    Serial.println("Light ahead: " + String(frontLight));
    
    // Turn right and measure
    for (int i = 0; i < 3; i++) turnRight();
    delay(500); // Wait to stabilize
    lightMeasurement[1] = analogRead(photoresistorPin);
    Serial.println("Light to the right: " + String(lightMeasurement[1]));
    
    // Turn left to return to the initial position then turn left again
    for (int i = 0; i < 6; i++) turnLeft();
    delay(500); // Wait to stabilize
    lightMeasurement[2] = analogRead(photoresistorPin);
    Serial.println("Light to the left:  " + String(lightMeasurement[2]));
    
    // Get the direction with the highest light level
    if (lightMeasurement[0] >= lightMeasurement[1] && lightMeasurement[0] >= lightMeasurement[2]) {
      optimalDirection = 0; //  Initial position
      // Return to the initial position (currently on the left)
      for (int i = 0; i < 3; i++) turnRight();
    } 
    else if (lightMeasurement[1] >= lightMeasurement[0] && lightMeasurement[1] >= lightMeasurement[2]) {
      optimalDirection = 1; // Right
      // Return to the right position (currently on the left)
      for (int i = 0; i < 6; i++) turnRight();
    }
    else {
      optimalDirection = 2; // Left
      // Already in the left position
    }
    
    int bestValue = max(max(lightMeasurement[0], lightMeasurement[1]), lightMeasurement[2]);
    
    // moveForward 
    for (int i = 0; i < 9; i++) {
      moveForward();
      
      // Check the distance to ensure there is no obstacle
      distance = mesureDistance();
      if (distance < 5) break;
      
      // Check if the light is still optimal
      int currentLight = analogRead(photoresistorPin);
      Serial.println("Cycle " + String(i+1) + " - Light: " + String(currentLight));
      
      if (currentLight < bestValue * 0.8) { // If the light decreases by more than 20%
        // Stop and restart the light search
        Serial.println("Loss of light, searching for new direction");
        break;
      }
    }
    
    // Stop for 5 minutes (300000 ms)
    Serial.println("5 minute break");
    reset_all_servos();
    haltTime = millis();
    while (millis() - haltTime < 300000) {
      // Continue to check the distance during the stop
      distance = mesureDistance();
      if (distance < 5) {
        Serial.println("Obstacle detected during the break!");
        break; // Exit the break if an obstacle is detected
      }
      delay(500); // Check every 500ms
    }
  }
}

// Function for moveForward (complete walking cycle)
void moveForward() {
  Serial.println("Forward walking cycle - with weight balancing");
  
  // 1. Right Front Leg (LFR) + help from left back leg (LBL)
  LBL_A2.write(110); // Slight push with the opposite diagonal leg
  LFR_A2.write(50);  // Lift the right front leg
  delay(waitTime);
  LFR_A1.write(120); // Move the hip forward
  delay(waitTime);
  LFR_A2.write(90);  // Rest the leg
  LBL_A2.write(90);  // Return the left back leg to neutral position
  delay(waitTime);
  LFR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 2. Left Back Leg (LBL) + help from right front leg (LFR)
  LFR_A2.write(70);  // Slight push with the opposite diagonal leg
  LBL_A2.write(130); // Lift the left back leg
  delay(waitTime);
  LBL_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LBL_A2.write(90);  // Rest the leg
  LFR_A2.write(90);  // Return the right front leg to neutral position
  delay(waitTime);
  LBL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 3. Left Front Leg (LFL) + help from right back leg (LBR)
  LBR_A2.write(70);  // Slight push with the opposite diagonal leg
  LFL_A2.write(130); // Lift the left front leg
  delay(waitTime);
  LFL_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LFL_A2.write(90);  // Rest the leg
  LBR_A2.write(90);  // Return the right back leg to neutral position
  delay(waitTime);
  LFL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 4. Right Back Leg (LBR) + help from left front leg (LFL)
  LFL_A2.write(70);  // Slight push with the opposite diagonal leg
  LBR_A2.write(50);  // Lift the right back leg
  delay(waitTime);
  LBR_A1.write(120); // Move the hip forward
  delay(waitTime);
  LBR_A2.write(90);  // Rest the leg
  LFL_A2.write(90);  // Return the left front leg to neutral position
  delay(waitTime);
  LBR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
}

// Left turn
void turnLeft() {
  Serial.println("Left rotation cycle - with weight balancing");
  
  // 1. Right Front Leg (LFR) + help from left back leg (LBL)
  LBL_A2.write(110); // Slight push with the opposite diagonal leg
  LFR_A2.write(50);  // Lift the right front leg
  delay(waitTime);
  LFR_A1.write(120); // Move the hip forward
  delay(waitTime);
  LFR_A2.write(90);  // Rest the leg
  LBL_A2.write(90);  // Return the left back leg to neutral position
  delay(waitTime);
  LFR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 2. Left Back Leg (LBL) + help from right front leg (LFR)
  LFR_A2.write(70);  // Slight push with the opposite diagonal leg
  LBL_A2.write(130); // Lift the left back leg
  delay(waitTime);
  LBL_A1.write(120); // Move the hip backward for left turn
  delay(waitTime);
  LBL_A2.write(90);  // Rest the leg
  LFR_A2.write(90);  // Return the right front leg to neutral position
  delay(waitTime);
  LBL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 3. Left Front Leg (LFL) + help from right back leg (LBR)
  LBR_A2.write(70);  // Slight push with the opposite diagonal leg
  LFL_A2.write(130); // Lift the left front leg
  delay(waitTime);
  LFL_A1.write(120); // Move the hip backward for left turn
  delay(waitTime);
  LFL_A2.write(90);  // Rest the leg
  LBR_A2.write(90);  // Return the right back leg to neutral position
  delay(waitTime);
  LFL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 4. Right Back Leg (LBR) + help from left front leg (LFL)
  LFL_A2.write(70);  // Slight push with the opposite diagonal leg
  LBR_A2.write(50);  // Lift the right back leg
  delay(waitTime);
  LBR_A1.write(120); // Move the hip forward
  delay(waitTime);
  LBR_A2.write(90);  // Rest the leg
  LFL_A2.write(90);  // Return the left front leg to neutral position
  delay(waitTime);
  LBR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
}

// Right turn
void turnRight() {
  Serial.println("Right rotation cycle - with weight balancing");
  
  // 1. Left Front Leg (LFL) + help from right back leg (LBR)
  LBR_A2.write(70);  // Slight push with the opposite diagonal leg
  LFL_A2.write(130); // Lift the left front leg
  delay(waitTime);
  LFL_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LFL_A2.write(90);  // Rest the leg
  LBR_A2.write(90);  // Return the right back leg to neutral position
  delay(waitTime);
  LFL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 2. Right Back Leg (LBR) + help from left front leg (LFL)
  LFL_A2.write(70);  // Slight push with the opposite diagonal leg
  LBR_A2.write(50);  // Lift the right back leg
  delay(waitTime);
  LBR_A1.write(60);  // Move the hip backward for right turn
  delay(waitTime);
  LBR_A2.write(90);  // Rest the leg
  LFL_A2.write(90);  // Return the left front leg to neutral position
  delay(waitTime);
  LBR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 3. Right Front Leg (LFR) + help from left back leg (LBL)
  LBL_A2.write(110); // Slight push with the opposite diagonal leg
  LFR_A2.write(50);  // Lift the right front leg
  delay(waitTime);
  LFR_A1.write(60);  // Move the hip backward for right turn
  delay(waitTime);
  LFR_A2.write(90);  // Rest the leg
  LBL_A2.write(90);  // Return the left back leg to neutral position
  delay(waitTime);
  LFR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 4. Left Back Leg (LBL) + help from right front leg (LFR)
  LFR_A2.write(70);  // Slight push with the opposite diagonal leg
  LBL_A2.write(130); // Lift the left back leg
  delay(waitTime);
  LBL_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LBL_A2.write(90);  // Rest the leg
  LFR_A2.write(90);  // Return the right front leg to neutral position
  delay(waitTime);
  LBL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
}

// Function to reset all servos to 90°
void reset_all_servos() {
  Serial.println("Resetting all servos to 90°");
  
  LFR_A1.write(posInitiale);
  LFR_A2.write(posInitiale);
  LFL_A1.write(posInitiale);
  LFL_A2.write(posInitiale);
  LBR_A1.write(posInitiale);
  LBR_A2.write(posInitiale);
  LBL_A1.write(posInitiale);
  LBL_A2.write(posInitiale);
  
  // Wait for all servos to reach their position
  delay(1000);
}

// Function to measure the distance with the ultrasonic sensor
long mesureDistance() {
  // Clear the trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Send a 10 microsecond pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the pulse time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance (in cm)
  // The speed of sound is 340 m/s or 29 microseconds per centimeter
  // The pulse makes a round trip, so we divide by 2
  long distance = duration / 58; // (duration * 0.034) / 2
  
  return distance;
}