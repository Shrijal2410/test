#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;

// Define motor pinouts
struct Motor {
  int pwm;
  int dir;
  int enca;
  int encb;
  bool isPwmSeesaw;
};

// Motor motors[] = {
//   {7, 24, -1, -1, true},   // motor1
//   {6, 25, -1, -1, true},   // motor2
//   {19, 15, -1, -1, false}, // motor3
//   {18, 14, -1, -1, false}, // motor4
//   {5, 11, -1, -1, false},  // motor5
//   {17, 10, -1, -1, false}, // motor6
// };

Motor motors[] = {
  {7, 24,  true},   // motor1
  {6, 25,  true},   // motor2
  {19, 15, false}, // motor3
  {18, 14, false}, // motor4
  {5, 11, false},  // motor5
  // {17, 10, false}, // motor6
};

const int motorCount = sizeof(motors) / sizeof(motors[0]);

void setup() {
  Serial.begin(115200);
  
  while (!Serial) delay(10);   // wait until serial port is opened
  
  if(!ss.begin()){
    Serial.println("seesaw not found!");
    while(1) delay(10);
  }
  
  Serial.println(F("seesaw started OK!"));

  // Set the pins as outputs for all motors
  for (int i = 0; i < motorCount; i++) {
    if (motors[i].isPwmSeesaw) {
      ss.pinMode(motors[i].pwm, OUTPUT);
    } else {
      pinMode(motors[i].pwm, OUTPUT);
    }
    ss.pinMode(motors[i].dir, OUTPUT);  // All direction pins are on seesaw
  }
}

void loop() {
  // Run all motors
  for (int i = 0; i < motorCount; i++) {
    ss.digitalWrite(motors[i].dir, HIGH);  // Set direction using seesaw
    if (motors[i].isPwmSeesaw) {
      ss.analogWrite(motors[i].pwm, 16384, 16);  // Full speed for 16-bit PWM on seesaw
    } else {
      analogWrite(motors[i].pwm, 64);  // Quarter speed for 8-bit PWM on main board
    }
  }

  delay(1000);
  Serial.println("All motors running");

  // Stop all motors
  for (int i = 0; i < motorCount; i++) {
    ss.digitalWrite(motors[i].dir, LOW);  // Set direction using seesaw
    if (motors[i].isPwmSeesaw) {
      ss.analogWrite(motors[i].pwm, 0, 16);  // Stop motor on seesaw
    } else {
      analogWrite(motors[i].pwm, 0);  // Stop motor on main board
    }
  }
  Serial.println("All motors stop");

  delay(1000);  // Wait for 1 second
}