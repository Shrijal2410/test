#include "Adafruit_seesaw.h"

Adafruit_seesaw ss;

#define ENCA 4 
#define ENCB 16
#define PWM 15
#define DIR_PIN 8  // This will be controlled by the seesaw

volatile int posi = 0;
long prevT = 0;
float eprev = 0;
float eintegral = 0;

void setup() {
  Serial.begin(9600);
  
  while (!Serial) delay(10);   // wait until serial port is opened
  
  if(!ss.begin()){
    Serial.println("seesaw not found!");
    while(1) delay(10);
  }
  
  Serial.println(F("seesaw started OK!"));

  ss.pinMode(DIR_PIN, OUTPUT);

  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);
  
  pinMode(PWM, OUTPUT);
  
  Serial.println("target pos");
}

void loop() {
  // set target position
  Serial.print(10000); 
  Serial.print(", ");
  Serial.print(-10000);
  Serial.print(", ");
  int target = 1525;

  // PID constants
  float kp = 1.0;
  float kd = 0.1;
  float ki = 0.01;

  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT)) / (1.0e6);
  prevT = currT;

  // Read the position
  int pos = 0; 
  noInterrupts(); // disable interrupts temporarily while reading
  pos = posi;
  interrupts(); // turn interrupts back on
  
  // error
  int e = target - pos;

  // derivative
  float dedt = (e-eprev) / (deltaT);

  // integral
  eintegral = eintegral + e * deltaT;

  // control signal
  float u = kp * e + kd * dedt + ki * eintegral;

  // motor power
  float pwr = fabs(u);
  if (pwr > 127) {
    pwr = 127;
  }

  // motor direction
  int dir = 1;
  if (u < 0) {
    dir = -1;
  }

  // signal the motor
  setMotor(dir, pwr, PWM, DIR_PIN);

  // store previous error
  eprev = e;

  Serial.print(target);
  Serial.print(", ");
  Serial.print(pos);
  Serial.println();
}

void setMotor(int dir, int pwmVal, int pwm, int dirPin) {
  analogWrite(pwm, pwmVal);
  if (dir == 1) {
    ss.digitalWrite(dirPin, HIGH);
  }
  else if (dir == -1) {
    ss.digitalWrite(dirPin, LOW);
  }
  else {
    // For stop condition, set PWM to 0
    analogWrite(pwm, 0);
  }  
}

void readEncoder() {
  int b = digitalRead(ENCB);
  if (b > 0) {
    posi++;
  }
  else {
    posi--;
  }
}