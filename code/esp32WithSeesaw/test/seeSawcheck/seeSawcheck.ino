/*
 * This example shows how to blink a pin on a seesaw.
 * Attach the positive (longer lead) of the LED to pin 15 on the seesaw, and
 * the negative lead of the LED to ground through a 1k ohm resistor.
 */

#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;

#define BLINK_PIN1 15
#define BLINK_PIN2 11
#define PWM_PIN 5
#define ANA_PIN 2



void setup() {
  Serial.begin(115200);
  
  while (!Serial) delay(10);   // wait until serial port is opened
  
  if(!ss.begin()){
    Serial.println("seesaw not found!");
    while(1) delay(10);
  }
  
  Serial.println(F("seesaw started OK!"));

  ss.pinMode(BLINK_PIN1, OUTPUT);
  ss.pinMode(BLINK_PIN2, OUTPUT);
  ss.pinMode(PWM_PIN, OUTPUT);
  ss.pinMode(ANA_PIN, OUTPUT);

}

void loop() {
  ss.digitalWrite(BLINK_PIN1, HIGH); 
  ss.digitalWrite(BLINK_PIN2, HIGH); 
    // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  ss.digitalWrite(BLINK_PIN1, LOW);
  ss.digitalWrite(BLINK_PIN2, LOW);    // turn the LED off by making the voltage LOW
      // turn the LED off by making the voltage LOW
  delay(100);  
  uint16_t pwmValue = 32768;  // 50% duty cycle for 16-bit PWM (range is 0 to 65535)
  
  // Write a 16-bit PWM value to the pin
  ss.analogWrite(PWM_PIN, pwmValue, 16);
  
  delay(2000);  // keep the LED at this brightness for 2 seconds

  pwmValue = 65535;  // 100% duty cycle for 16-bit PWM
  ss.analogWrite(PWM_PIN, pwmValue, 16);

  delay(2000);  // keep the LED at this brightness for 2 seconds

  pwmValue = 0;  // 0% duty cycle for 16-bit PWM (LED off)
  ss.analogWrite(PWM_PIN, pwmValue, 16);

  delay(2000);
  ss.digitalWrite(ANA_PIN, HIGH);
  delay(1000);
  ss.digitalWrite(ANA_PIN, LOW);
  delay(1000);
}
