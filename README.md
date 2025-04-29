# LegsDayProject-PlantsEdition
_You can't soak up the sun by just rooting around..._

Forget just leaving your plants stuck in one place! The Legs Day Project: Plant Edition is giving them the "stem-ina" to go find their own sunshine. No longer tied down by pots, these botanical beings are on the move, driven by a serious craving for light.

![Teasing screen](https://github.com/MarinaXP/LegsDayProject-PlantsEdition/blob/8695c3020c0e6a7ebcedbb1bd697e0167b86c355/screens/presentation_screen.png?raw=true)

## Project Origin & Goals

My plant care struggles due to a north-facing apartment and inconsistent repositioning for sunlight led to the development of this solution.
This is designed to be made from reclaimed/recycled materials, so there's no need for a 3D printer,  though other possibilities can be investigated.

### Objectives:

- **Make indoor plants autonomous :** give them legs! Let each plant move toward the light it needs, based on its own requirements.
- **Build using reclaimed/recycled materials :** avoid 3D printing and make the design as universal and accessible as possible.
- **Learn & share through robotics : m**ake it fun and collaborative while learning about electronics, sensors, and movement.

## 2. Components

![Components screen](https://github.com/MarinaXP/LegsDayProject-PlantsEdition/blob/14770e5b5dd6bb3246399408cfe04ad0a832e10e/screens/components_screen.png?raw=true)

### Arduino Mega 2560

(or Arduino Uno with a PWM/Servo Driver)

A microcontroller board based on the [ATmega2560](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf). This was simply the board I had on hand, so that's what I started with.

### Ultrasonic Sensor (HC-SR04)

This sensor measures distances between 2cm and 400cm using ultrasonic waves. It works by sending out ultrasonic waves from a transmitter and then detecting them with a receiver after they've bounced off an object, allowing it to identify obstacles.

### Photoresistor Module

We use a photoresistor module to measure light intensity. The internal photoresistor's resistance changes with the amount of light, going down in bright conditions and up in darkness, thus indicating the light level.

### SG90 Servo Motors (180°)

For applications requiring specific angular positioning, the SG90 180-degree servo motor is a common choice.

## 3. Other Useful Stuff

The robot is built mainly using reclaimed/recycled materials, but you’ll still need a few tools to get the job done:

- Precision screwdrivers
- M1 to M3 screw/nut kit
- Cutter
- Jumper wires
- A compact breadboard
- Optional but helpful: Dremel tool for cutting and shaping materials


## 4. How It Works

### Construction

Start from the provided **frame design template** as a guide.

This can (and should) be adapted depending on the materials you have at hand.

[![frame design template button](https://img.shields.io/badge/Open%20the%20Template-PDF-red?style=for-the-badge&logo=adobeacrobatreader&logoColor=white&color=white)](https://github.com/MarinaXP/LegsDayProject-PlantsEdition/blob/14770e5b5dd6bb3246399408cfe04ad0a832e10e/template/frame_design_template.pdf)

### Code Logic

The robot follows a simple decision loop:

1. Measure light intensity in **three directions**: center, left, right.
2. **Compare values** and move toward the area with the most light.
3. Once settled in a good spot, **pause to absorb light**.
4. Repeat the process to stay optimized for lighting throughout the day.

The **ultrasonic sensor** acts as a safety mechanism:

- If an obstacle is detected **within ~5 cm**, the robot stops and **turns around** to avoid collision.

## Contribute

Feel free to fork, remix, improve or completely reimagine the build.