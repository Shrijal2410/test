// #include <util/atomic.h> // For the ATOMIC_BLOCK macro

#define ENCA 22
#define ENCB 23
#define PWM 21
#define IN2 18
#define IN1 19

volatile int posi = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);

  pinMode(PWM, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop()
{
  // Read the position in an atomic block to avoid a potential misread if the interrupt coincides with this code running
  // https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
  int pos = 0;
  noInterrupts();
  pos = posi;
  interrupts();

  Serial.println(pos);
  setMotor(1, 128, PWM, IN1, IN2);
  delay(2000);
  Serial.println(pos);
  setMotor(-1, 128, PWM, IN1, IN2);
  delay(2000);
  Serial.println(pos);
  setMotor(0, 128, PWM, IN1, IN2);
  delay(2000);
  Serial.println(pos);
}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2)
{
  analogWrite(pwm, pwmVal);
  if (dir == 1)
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else if (dir == -1)
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}

void readEncoder()
{
  int b = digitalRead(ENCB);
  if (b > 0)
  {
    posi++;
  }
  else
  {
    posi--;
  }
}