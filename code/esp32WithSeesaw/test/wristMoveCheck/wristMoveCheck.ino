#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;

// Define the motor control pins
// const int DIR1 = 9;
// const int EN1 = 5;
const int DIR2 = 5;
const int EN2 = 18;

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
  // ss.pinMode(DIR1, OUTPUT);
  // ss.pinMode(EN1, OUTPUT);
  pinMode(EN2,OUTPUT);
  pinMode(DIR2,OUTPUT);
}

void loop() {
  // Move the motor forward
  // Serial.println("Motor1 is moving forward...");
  // ss.digitalWrite(DIR1, HIGH);
  // ss.analogWrite(EN1, 128);  // Full speed

  // // Run the motor for 2 seconds
  // delay(500);

  // // Stop the motor
  // Serial.println("Motor1 is stopped.");
  // ss.digitalWrite(DIR1, LOW);
  // ss.analogWrite(EN1, 0);  // Turn off the motor

  // // Wait for 2 seconds before the next loop
  // delay(2000);

  // Serial.println("Motor1 is moving BACKward...");
  // ss.digitalWrite(DIR1,LOW);
  // ss.analogWrite(EN1, 128); 
  // delay(500);

  // Serial.println("Motor1 is stopped.");
  // ss.digitalWrite(DIR1, LOW);
  // ss.analogWrite(EN1, 0);  // Turn off the motor

  // // Wait for 2 seconds before the next loop
  // delay(2000);

  Serial.println("Motor2 is moving forward...");
  digitalWrite(DIR2, HIGH);
  analogWrite(EN2, 128);  // Full speed

  // Run the motor for 2 seconds
  delay(500);

  // Stop the motor
  Serial.println("Motor2 is stopped.");
  digitalWrite(DIR2, LOW);
  analogWrite(EN2, 0);  // Turn off the motor

  // Wait for 2 seconds before the next loop
  delay(2000);

  Serial.println("Motor2 is moving BACKward...");
  digitalWrite(DIR2,LOW);
  analogWrite(EN2, 128); 
  delay(500);

  Serial.println("Motor2 is stopped.");
  digitalWrite(DIR2, LOW);
  analogWrite(EN2, 0);  // Turn off the motor

  // Wait for 2 seconds before the next loop
  delay(2000);

}