/*
  This code was adapted from the MyoWare analogReadValue.ino example found here:
  https://github.com/AdvancerTechnologies/MyoWare_MuscleSensor

  This example streams the data from a single MyoWare sensor attached to ADC A3, A4, A5.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).

  *Only run on a laptop using its battery. Do not plug in laptop charger/dock/monitor.
  *Do not touch your laptop trackpad or keyboard while the MyoWare sensor is powered.

  Hardware:
  Wireless Shield (ESP32)
  USB from Shield to Computer.
*/

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Sensor Reading...");
}

void loop()
{
  int env = analogRead(A3);
  int raw = analogRead(A4);
  int ref = analogRead(A5);

  Serial.print(env);
  Serial.print(",");
  Serial.print(raw);
  Serial.print(",");
  Serial.println(ref);

  delay(50);
}
