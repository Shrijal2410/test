// Define the pins connected to the TB6612FNG motor driver
#define AIN1 14    // Motor A input 1
#define AIN2 27    // Motor A input 2
#define PWMA 26    // Motor A PWM
// Encoder pins
#define ENCODER_A 34 // Encoder output A
#define ENCODER_B 35 // Encoder output B

// Define PWM properties
const int freq = 5000; // PWM frequency
const int pwmChannel = 0; // PWM channel
const int resolution = 8; // PWM resolution (8 bits)

// Encoder variables
volatile long encoderPosition = 0;
volatile long lastEncoderPosition = 0;
int encoderState;
int lastEncoderState;
const int encoderPulsesPerRevolution = 1000; // Change according to your encoder specs

// Target turns
int targetTurns = 0;
int maxTurns = 20;

void IRAM_ATTR handleEncoder() {
  encoderState = digitalRead(ENCODER_A);
  if (encoderState != lastEncoderState) {
    if (digitalRead(ENCODER_B) != encoderState) {
      encoderPosition++;
    } else {
      encoderPosition--;
    }
  }
  lastEncoderState = encoderState;
}

void setup() {
  // Initialize serial communication for debugging and user input
  Serial.begin(115200);

  // Set motor control pins as outputs
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  // Set encoder pins as inputs
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);

  // Attach interrupt for the encoder
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), handleEncoder, CHANGE);

  // Configure PWM channel
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(PWMA, pwmChannel);

  // Initialize encoder state
  lastEncoderState = digitalRead(ENCODER_A);
}

void loop() {
  // Check for user input
  if (Serial.available() > 0) {
    int inputTurns = Serial.parseInt();
    if (inputTurns >= -maxTurns && inputTurns <= maxTurns) {
      targetTurns = inputTurns;
      moveMotor(targetTurns);
    } else {
      Serial.println("Input out of range. Enter a value between -20 and 20.");
    }
  }
}

void moveMotor(int turns) {
  long targetPosition = turns * encoderPulsesPerRevolution;
  if (targetPosition > encoderPosition) {
    // Rotate motor clockwise
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    while (encoderPosition < targetPosition) {
      ledcWrite(pwmChannel, 128); // 50% duty cycle (128/255)
    }
  } else if (targetPosition < encoderPosition) {
    // Rotate motor counterclockwise
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    while (encoderPosition > targetPosition) {
      ledcWrite(pwmChannel, 128); // 50% duty cycle (128/255)
    }
  }
  // Stop the motor
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  ledcWrite(pwmChannel, 0); // 0% duty cycle
  Serial.print("Reached target position: ");
  Serial.println(encoderPosition / encoderPulsesPerRevolution);
}
