#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;

// Define the motor control pins
const int IN1 = 11;
const int IN2 = 16;
const int EN = 5;

void setup() {
  delay(50);  // Give seesaw time to wake up
  Serial.begin(115200);
  while (!Serial) delay(10);  // Wait for serial port to open

  if (!ss.begin()) {
    Serial.println("Seesaw not found!");
    while(1) delay(10);
  }
  Serial.println("Seesaw started OK!");

  // Set the motor control pins as outputs
  ss.pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
}

void loop() {
  // Move the motor forward
  Serial.println("Motor is moving forward...");
  ss.digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 128);  // Full speed

  // Run the motor for 2 seconds
  delay(1000);

  // Stop the motor
  Serial.println("Motor is stopped.");
  ss.digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 0);  // Turn off the motor

  // Wait for 2 seconds before the next loop
  delay(1000);

  Serial.println("Motor is moving BACKward...");
  ss.digitalWrite(IN1,LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN, 128); 
  delay(1000);

}