#define EMG_PIN1 36
#define EMG_PIN2 39

int count = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int value1 = analogRead(EMG_PIN1);
  int value2 = analogRead(EMG_PIN2);

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