void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;  // optionally wait for serial terminal to open
  Serial.println("MyoWare Example_01_analogRead_SINGLE");
}

void loop() {
  int env = analogRead(A3);
  int raw = analogRead(A4);  // read the input on analog pin A0
  int ref = analogRead(A5);  // read the input on analog pin A0
                             // read the input on analog pin A0



  Serial.print(0);
  Serial.print(",");
  Serial.print(5000);
  Serial.print(",");
  Serial.println(env);  // print out the value you read
  // Serial.print(", raw"); // print out the value you read
  // Serial.print(raw);
  // Serial.print(", ref:");
  // Serial.println(ref); // print out the value you read
  // print out the value you read
  // print out the value you read
  // print out the value you read

  delay(50);  // to avoid overloading the serial terminal
}