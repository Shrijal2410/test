// #include <Servo.h>

//Threshold for servo motor control with muscle sensor. 
//You can set a threshold according to the maximum and minimum values of the muscle sensor.
#define THRESHOLD 250

//Pin number where the sensor is connected. (Analog 0)
#define EMG_PIN1 36
#define EMG_PIN2 39


//Pin number where the servo motor is connected. (Digital PWM 3)
// #define SERVO_PIN 3

//Define Servo motor
// Servo SERVO_1;



/*-------------------------------- void setup ------------------------------------------------*/
int count = 0;
void setup(){
  
  //BAUDRATE set to 115200, remember it to set monitor serial properly. 
  //Used this Baud Rate and "NL&CR" option to visualize the values correctly.
  Serial.begin(115200);
  
  //Set servo motor to digital pin 3
  // SERVO_1.attach(SERVO_PIN);
}

/*--------------------------------  void loop  ------------------------------------------------*/

void loop(){

  //The "Value" variable reads the value from the analog pin to which the sensor is connected.
  
  int value1 = analogRead(EMG_PIN1); //up
  int value2 = analogRead(EMG_PIN2); //down

  


  //If the sensor value is GREATER than the THRESHOLD, the servo motor will turn to 170 degrees.
  // if(value > THRESHOLD){
  //   SERVO_1.write(170);
  // }

  // //If the sensor is LESS than the THRESHOLD, the servo motor will turn to 10 degrees.
  // else{
  //   SERVO_1.write(10);
  // }

  //You can use serial monitor to set THRESHOLD properly, comparing the values shown when you open and close your hand.
  Serial.print(0);
  Serial.print(", ");
  Serial.print(4000);
  Serial.print(", ");
  Serial.print(count);
  Serial.print(", ");
  Serial.print(value1);
  Serial.print(", ");
  Serial.println(value2);
  count++;
  delay(100);
}
