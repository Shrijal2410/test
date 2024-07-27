#define ENCA 22
#define ENCB 23

volatile int posi = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);
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