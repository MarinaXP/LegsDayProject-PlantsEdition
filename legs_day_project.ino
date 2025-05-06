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
// initial position for all servos A2
const int posInitialeA2 = 90;

// initial position for servos A1 
const int posInit_LFR_A1 = 140;
const int posInit_LFL_A1 = 40;
const int posInit_LBR_A1 = 40;
const int posInit_LBL_A1 = 140;

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
  // Check if there is an obstacle
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
  Serial.println("Forward walking cycle - better weight distribution");
  
  // 1. Move LFR_A2 and LBL_A2 simultaneously
  Serial.println("Step 1: LFR_A2 and LBL_A2");
  LFR_A2.write(50);
  LBL_A2.write(80);
  delay(waitTime);

  // Return to 90° for both
  Serial.println("Return to 90°");
  LFR_A2.write(90);
  LBL_A2.write(90);
  delay(waitTime);

  // 2. Move LFL_A2 and LBR_A2 simultaneously
  Serial.println("Step 2: LFL_A2 and LBR_A2");
  LFL_A2.write(130);
  LBR_A2.write(100);
  delay(waitTime);

  // Return to 90° for both
  Serial.println("Return to 90°");
  LFL_A2.write(90);
  LBR_A2.write(90);
  delay(waitTime);
}

// Left turn
void turnLeft() {
  Serial.println("Left rotation cycle - with weight balancing");
  
  // Ensure all A2 servos are at 90° before starting
  reset_all_servos();
  delay(500);

  // 1. Move all A1 joints simultaneously to prepare for rotation
  Serial.println("1. Positioning A1 joints for rotation");
  LFR_A1.write(120); 
  LBR_A1.write(90);  
  LFL_A1.write(90);  
  LBL_A1.write(120); 
  delay(waitTime * 2); // Wait a bit longer to ensure all servos are in position

  // 2. Execute a simplified forward walking cycle
  Serial.println("2. Executing a walking cycle");

  // 2.1 Move LFR_A2 and LBL_A2
  Serial.println("  2.1 Lifting LFR_A2 and LBL_A2");
  LFR_A2.write(50);
  LBL_A2.write(70);
  delay(waitTime);

  // Return to 90° for these two servos
  LFR_A2.write(90);
  LBL_A2.write(90);
  delay(waitTime);

  // 2.2 Move LFL_A2 and LBR_A2
  Serial.println("  2.2 Lifting LFL_A2 and LBR_A2");
  LFL_A2.write(130);
  LBR_A2.write(110);
  delay(waitTime);

  // Return to 90° for these two servos
  LFL_A2.write(90);
  LBR_A2.write(90);
  delay(waitTime);

  // 3. Return the A1 joints to their initial position
  Serial.println("3. Returning A1 joints to their initial position");
  LFR_A1.write(140); 
  LBR_A1.write(40); 
  LFL_A1.write(40); 
  LBL_A1.write(140); 
  delay(waitTime * 2); // Wait a bit longer to ensure all servos are in position

  Serial.println("End of left turn");
}

// Right turn
void turnRight() {
  Serial.println("Right rotation cycle - with weight balancing");
  
  // Ensure all A2 servos are at 90° before starting
  reset_all_servos();
  delay(500);

  // 1. Move all A1 joints simultaneously to prepare for rotation
  Serial.println("1. Positioning A1 joints for rotation");
  LFR_A1.write(90); 
  LBR_A1.write(20); 
  LFL_A1.write(20); 
  LBL_A1.write(90); 
  delay(waitTime * 2); // Wait a bit longer to ensure all servos are in position

  // 2. Execute a simplified forward walking cycle
  Serial.println("2. Executing a walking cycle");

  // 2.1 Move LFR_A2 and LBL_A2
  Serial.println("  2.1 Lifting LFR_A2 and LBL_A2");
  LFR_A2.write(50);
  LBL_A2.write(70);
  delay(waitTime);

  // Return to 90° for these two servos
  LFR_A2.write(90);
  LBL_A2.write(90);
  delay(waitTime);

  // 2.2 Move LFL_A2 and LBR_A2
  Serial.println("  2.2 Lifting LFL_A2 and LBR_A2");
  LFL_A2.write(130);
  LBR_A2.write(110);
  delay(waitTime);

  // Return to 90° for these two servos
  LFL_A2.write(90);
  LBR_A2.write(90);
  delay(waitTime);

  // 3. Return the A1 joints to their initial position
  Serial.println("3. Returning A1 joints to their initial position");
  LFR_A1.write(140); 
  LBR_A1.write(40); 
  LFL_A1.write(40); 
  LBL_A1.write(140); 
  delay(waitTime * 2); // Wait a bit longer to ensure all servos are in position

  Serial.println("End of right turn");
}

// Function to reset all servos to their initial positions
void reset_all_servos() {
  Serial.println("Resetting all servos to their initial positions");
  
  // Reset A1 servos to their specific initial positions
  LFR_A1.write(posInit_LFR_A1); // Front right leg A1 joint to 140°
  LFL_A1.write(posInit_LFL_A1); // Front left leg A1 joint to 40°
  LBR_A1.write(posInit_LBR_A1); // Back right leg A1 joint to 40°
  LBL_A1.write(posInit_LBL_A1); // Back left leg A1 joint to 140°
  
  // Reset all A2 servos to the common initial position (90°)
  LFR_A2.write(posInitialeA2);
  LFL_A2.write(posInitialeA2);
  LBR_A2.write(posInitialeA2);
  LBL_A2.write(posInitialeA2);
  
  // Wait for all servos to reach their positions
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