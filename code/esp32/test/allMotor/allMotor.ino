// Define pin connections for 5 motors
const int ena[] = { 13, 12, 14, 27, 26, 25, 33 };  // PWM pins for speed control
const int dir[] = { 15, 2, 4, 16, 17, 5, 18 };     // Direction control pins

void setup() {
  // Set the pins as outputs for all motors
  for (int i = 0; i < 7; i++) {
    pinMode(ena[i], OUTPUT);
    pinMode(dir[i], OUTPUT);
  }

  Serial.begin(115200);
}

void loop() {
  // Example: Run all motors forward
  // for (int i = 0; i < 5; i++) {
  //   digitalWrite(dir[i], LOW);
  //   analogWrite(ena[i], 255);
  // }

  // delay(5000);
  // Serial.println("All motors running forward");

  // // Stop all motors
  // for (int i = 0; i < 5; i++) {
  //   analogWrite(ena[i], 0);
  // }

  // delay(1000); // Wait for 1 second

  // Example: Run all motors backward
  for (int i = 0; i < 7; i++) {
    digitalWrite(dir[i], HIGH);
    analogWrite(ena[i], 255);
  }

  delay(1000);
  Serial.println("All motors running");

  // Stop all motors
  for (int i = 0; i < 7; i++) {
    digitalWrite(dir[i], LOW);
    analogWrite(ena[i], 0);
  }
  Serial.println("All motors stop");

  delay(1000);  // Wait for 1 second
}
