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
  
  // Attacher tous les servos à leurs pins respectifs
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
    
    // Déterminer la direction avec le plus de lumière
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
  Serial.println("Forward walking cycle");
  
  // 1. Right Front Leg (LFR)
  LFR_A2.write(130); // Lift the leg
  delay(waitTime);
  LFR_A1.write(120); // Move the hip forward
  delay(waitTime);
  LFR_A2.write(90);  // Rest the leg
  delay(waitTime);
  LFR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 2. Left Back Leg (LBL)
  LBL_A2.write(130); // Lift the leg
  delay(waitTime);
  LBL_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LBL_A2.write(90);  // Rest the leg
  delay(waitTime);
  LBL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 3. Left Front Leg (LFL)"
  LFL_A2.write(130); // Lift the leg
  delay(waitTime);
  LFL_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LFL_A2.write(90);  // Rest the leg
  delay(waitTime);
  LFL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 4. Right Back Leg (LBR)
  LBR_A2.write(50);  // Lift the leg (note the different angle here)
  delay(waitTime);
  LBR_A1.write(120); // Move the hip forward
  delay(waitTime);
  LBR_A2.write(90);  // Rest the leg
  delay(waitTime);
  LBR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
}

// Function to turn left
void turnLeft() {
  Serial.println("Left rotation cycle");
  
  // 1. Right Front Leg (LFR)
  LFR_A2.write(130); // Lift the leg
  delay(waitTime);
  LFR_A1.write(120); // Move the hip forward
  delay(waitTime);
  LFR_A2.write(90);  // Rest the leg
  delay(waitTime);
  LFR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 2. Left Back Leg (LBL)
  LBL_A2.write(130); // Lift the leg
  delay(waitTime);
  LBL_A1.write(120); // Move the hip forward
  delay(waitTime);
  LBL_A2.write(90);  // Rest the leg
  delay(waitTime);
  LBL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 3. Left Front Leg (LFL)"
  LFL_A2.write(130); // Lift the leg
  delay(waitTime);
  LFL_A1.write(120); // Move the hip forward
  delay(waitTime);
  LFL_A2.write(90);  // Rest the leg
  delay(waitTime);
  LFL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 4. Right Back Leg (LBR)
  LBR_A2.write(50);  // Lift the leg
  delay(waitTime);
  LBR_A1.write(120); // Move the hip forward
  delay(waitTime);
  LBR_A2.write(90);  // Rest the leg
  delay(waitTime);
  LBR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
}

// Fonction pour tourner à droite
void turnRight() {
  Serial.println("Cycle de rotation droite");
  
  // 1. Left Front Leg (LFL)"
  LFL_A2.write(130); // Lift the leg
  delay(waitTime);
  LFL_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LFL_A2.write(90);  // Rest the leg
  delay(waitTime);
  LFL_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 2. Right Back Leg (LBR)
  LBR_A2.write(50);  // Lift the leg
  delay(waitTime);
  LBR_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LBR_A2.write(90);  // Rest the leg
  delay(waitTime);
  LBR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 3. Right Front Leg (LFR)
  LFR_A2.write(130); // Lift the leg
  delay(waitTime);
  LFR_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LFR_A2.write(90);  // Rest the leg
  delay(waitTime);
  LFR_A1.write(90);  // Return the hip to the initial position
  delay(waitTime);
  
  // 4. Left Back Leg (LBL)
  LBL_A2.write(130); // Lift the leg
  delay(waitTime);
  LBL_A1.write(60);  // Move the hip forward
  delay(waitTime);
  LBL_A2.write(90);  // Rest the leg
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