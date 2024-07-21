#include <WiFi.h>
#include <WiFiServer.h>

// WiFi credentials
const char* ssid = "motog52";
const char* password = "12ka442ka1";

WiFiServer server(80);

// A class to compute the control signal
class SimplePID {
private:
  float kp, kd, ki, umax;  // Parameters
  float eprev, eintegral;  // Storage

public:
  // Constructor
  SimplePID()
    : kp(1), kd(0.1), ki(0.01), umax(255), eprev(0.0), eintegral(0.0) {}

  // A function to set the parameters
  void setParams(float kpIn, float kdIn, float kiIn, float umaxIn) {
    kp = kpIn;
    kd = kdIn;
    ki = kiIn;
    umax = umaxIn;
  }

  // A function to compute the control signal
  void evalu(int value, int target, float deltaT, int& pwr, int& dir) {
    // error
    int e = target - value;

    // derivative
    float dedt = (e - eprev) / (deltaT);

    // integral
    eintegral = eintegral + e * deltaT;

    // control signal
    float u = kp * e + kd * dedt + ki * eintegral;

    // motor power
    pwr = (int)fabs(u);
    if (pwr > umax) {
      pwr = umax;
    }

    // motor direction
    dir = 1;
    if (u < 0) {
      dir = -1;
    }

    // store previous error
    eprev = e;
  }
};

// How many motors
#define NMOTORS 5

// Pins
const int pwm[] = { 15, 5, 21, 14, 33 };
const int in1[] = { 2, 17, 22, 27, 32 };
const int enca[] = { 4, 18, 13, 26, 35 };
const int encb[] = { 16, 19, 12, 25, 34 };

int resetPin = 23;

// Globals
long prevT = 0;
volatile int posi[] = { 0, 0, 0, 0, 0 };

// PID class instances
SimplePID pid[NMOTORS];

// Target positions
int target[NMOTORS] = { 5000, 5000, 5000, 5000, 5000 };

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();

  pinMode(resetPin, INPUT);

  for (int k = 0; k < NMOTORS; k++) {
    pinMode(enca[k], INPUT);
    pinMode(encb[k], INPUT);
    pinMode(pwm[k], OUTPUT);
    pinMode(in1[k], OUTPUT);

    pid[k].setParams(1, 0, 0, 255);
  }

  attachInterrupt(digitalPinToInterrupt(enca[0]), readEncoder<0>, RISING);
  attachInterrupt(digitalPinToInterrupt(enca[1]), readEncoder<1>, RISING);
  attachInterrupt(digitalPinToInterrupt(enca[2]), readEncoder<2>, RISING);
  attachInterrupt(digitalPinToInterrupt(enca[3]), readEncoder<3>, RISING);
  attachInterrupt(digitalPinToInterrupt(enca[4]), readEncoder<4>, RISING);

  Serial.println("Starting main loop...");
  Serial.println("target pos");
  digitalWrite(resetPin, LOW);
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<html><head><title>Motor Control</title>");
            client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
            client.println("<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>");
            client.println("<style>");
            client.println("body { font-family: Arial; text-align: center; }");
            client.println(".slider { width: 300px; }");
            client.println(".chart-container { width: 30%; height: 300px; display: inline-block; margin: 10px; }");
            client.println(".chart-row { width: 100%; }");
            client.println("</style></head><body>");
            client.println("<h2>Motor Control</h2>");

            // Master slider
            client.println("<h3>Master Control</h3>");
            client.println("<input type='range' id='masterSlider' class='slider' min='0' max='15000' value='5000' oninput='updateMaster()'>");
            client.println("<span id='masterValue'>5000</span>");
            client.println("<input type='number' id='masterInput' value='5000' onchange='updateMasterFromInput()'>");

            // Individual motor sliders
            for (int i = 0; i < NMOTORS; i++) {
              client.print("<h3>Motor ");
              client.print(i + 1);
              client.println("</h3>");
              client.print("<input type='range' id='motor");
              client.print(i);
              client.print("' class='slider' min='0' max='15000' value='");
              client.print(target[i]);
              client.print("' oninput='updateMotor(");
              client.print(i);
              client.println(")'>");
              client.print("<span id='motorValue");
              client.print(i);
              client.print("'>");
              client.print(target[i]);
              client.println("</span>");
              client.print("<input type='number' id='motorInput");
              client.print(i);
              client.print("' value='");
              client.print(target[i]);
              client.print("' onchange='updateMotorFromInput(");
              client.print(i);
              client.println(")'>");
            }

            // Add canvas elements for charts
            client.println("<div class='chart-row'>");
            for (int i = 0; i < 3; i++) {
              client.print("<div class='chart-container'><canvas id='chart");
              client.print(i);
              client.println("'></canvas></div>");
            }
            client.println("</div>");
            client.println("<div class='chart-row'>");
            for (int i = 3; i < NMOTORS; i++) {
              client.print("<div class='chart-container'><canvas id='chart");
              client.print(i);
              client.println("'></canvas></div>");
            }
            client.println("</div>");



            client.println("<script>");
            client.println("function updateMaster() {");
            client.println("  var masterSlider = document.getElementById('masterSlider');");
            client.println("  var masterValue = document.getElementById('masterValue');");
            client.println("  var masterInput = document.getElementById('masterInput');");
            client.println("  var value = masterSlider.value;");
            client.println("  masterValue.textContent = value;");
            client.println("  masterInput.value = value;");
            client.println("  ");
            client.print("  for (var i = 0; i < ");
            client.print(NMOTORS);
            client.println("; i++) {");
            client.println("    var slider = document.getElementById('motor' + i);");
            client.println("    var valueSpan = document.getElementById('motorValue' + i);");
            client.println("    var input = document.getElementById('motorInput' + i);");
            client.println("    slider.value = value;");
            client.println("    valueSpan.textContent = value;");
            client.println("    input.value = value;");
            client.println("    updateMotorServer(i, value);");
            client.println("  }");
            client.println("}");
            client.println("");
            client.println("function updateMasterFromInput() {");
            client.println("  var masterInput = document.getElementById('masterInput');");
            client.println("  var masterSlider = document.getElementById('masterSlider');");
            client.println("  var masterValue = document.getElementById('masterValue');");
            client.println("  var value = masterInput.value;");
            client.println("  masterSlider.value = value;");
            client.println("  masterValue.textContent = value;");
            client.println("  updateMaster();");
            client.println("}");
            client.println("");
            client.println("function updateMotor(motorIndex) {");
            client.println("  var slider = document.getElementById('motor' + motorIndex);");
            client.println("  var valueSpan = document.getElementById('motorValue' + motorIndex);");
            client.println("  var input = document.getElementById('motorInput' + motorIndex);");
            client.println("  var value = slider.value;");
            client.println("  valueSpan.textContent = value;");
            client.println("  input.value = value;");
            client.println("  updateMotorServer(motorIndex, value);");
            client.println("}");
            client.println("");
            client.println("function updateMotorFromInput(motorIndex) {");
            client.println("  var input = document.getElementById('motorInput' + motorIndex);");
            client.println("  var slider = document.getElementById('motor' + motorIndex);");
            client.println("  var valueSpan = document.getElementById('motorValue' + motorIndex);");
            client.println("  var value = input.value;");
            client.println("  slider.value = value;");
            client.println("  valueSpan.textContent = value;");
            client.println("  updateMotorServer(motorIndex, value);");
            client.println("}");
            client.println("");
            client.println("function updateMotorServer(motorIndex, value) {");
            client.println("  var xhr = new XMLHttpRequest();");
            client.println("  xhr.open('GET', '/setTarget?motor=' + motorIndex + '&target=' + value, true);");
            client.println("  xhr.send();");
            client.println("}");
            client.println("var charts = [];");
            client.println("var chartData = Array(5).fill().map(() => ({");
            client.println("  labels: [],");
            client.println("  targetData: [],");
            client.println("  actualData: []");
            client.println("}));");

            client.println("function initCharts() {");
            client.println("  for (var i = 0; i < 5; i++) {");
            client.println("    var ctx = document.getElementById('chart' + i).getContext('2d');");
            client.println("    charts[i] = new Chart(ctx, {");
            client.println("      type: 'line',");
            client.println("      data: {");
            client.println("        labels: [],");
            client.println("        datasets: [{");
            client.println("          label: 'Target Position',");
            client.println("          borderColor: 'rgb(255, 99, 132)',");
            client.println("          data: []");
            client.println("        }, {");
            client.println("          label: 'Actual Position',");
            client.println("          borderColor: 'rgb(54, 162, 235)',");
            client.println("          data: []");
            client.println("        }]");
            client.println("      },");
            client.println("      options: {");
            client.println("        responsive: true,");
            client.println("        maintainAspectRatio: false,");
            client.println("        title: {");
            client.println("          display: true,");
            client.println("          text: 'Motor ' + (i + 1) + ' Position'");
            client.println("        },");
            client.println("        scales: {");
            client.println("          x: {");
            client.println("            type: 'linear',");
            client.println("            position: 'bottom',");
            client.println("            title: {");
            client.println("              display: true,");
            client.println("              text: 'Time (s)'");
            client.println("            }");
            client.println("          },");
            client.println("          y: {");
            client.println("            title: {");
            client.println("              display: true,");
            client.println("              text: 'Position'");
            client.println("            }");
            client.println("          }");
            client.println("        }");
            client.println("      }");
            client.println("    });");
            client.println("  }");
            client.println("}");

            client.println("function updateCharts(data) {");
            client.println("  var time = data.time;");
            client.println("  for (var i = 0; i < 5; i++) {");
            client.println("    chartData[i].labels.push(time);");
            client.println("    chartData[i].targetData.push(data.target[i]);");
            client.println("    chartData[i].actualData.push(data.actual[i]);");
            client.println("    if (chartData[i].labels.length > 50) {");
            client.println("      chartData[i].labels.shift();");
            client.println("      chartData[i].targetData.shift();");
            client.println("      chartData[i].actualData.shift();");
            client.println("    }");
            client.println("    charts[i].data.labels = chartData[i].labels;");
            client.println("    charts[i].data.datasets[0].data = chartData[i].targetData;");
            client.println("    charts[i].data.datasets[1].data = chartData[i].actualData;");
            client.println("    charts[i].update();");
            client.println("  }");
            client.println("}");

            client.println("function fetchData() {");
            client.println("  fetch('/getData')");
            client.println("    .then(response => response.json())");
            client.println("    .then(data => {");
            client.println("      updateCharts(data);");
            client.println("    });");
            client.println("}");

            client.println("initCharts();");
            client.println("setInterval(fetchData, 1000);");  // Fetch data every second

            client.println("</script>");
            client.println("</body></html>");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.startsWith("GET /setTarget")) {
          int motorIndex = -1;
          int targetValue = -1;

          // Find the position of "motor=" and "target=" in the request
          int motorPos = currentLine.indexOf("motor=");
          int targetPos = currentLine.indexOf("target=");

          if (motorPos != -1 && targetPos != -1) {
            // Extract the motor index
            motorIndex = currentLine.substring(motorPos + 6, currentLine.indexOf("&", motorPos)).toInt();

            // Extract the target value
            targetValue = currentLine.substring(targetPos + 7, currentLine.indexOf(" ", targetPos)).toInt();

            // Update only if both motor index and target value are valid
            if (motorIndex >= 0 && motorIndex < NMOTORS && targetValue >= 0) {
              target[motorIndex] = targetValue;
              // Serial.print("Updated motor ");
              // Serial.print(motorIndex);
              // Serial.print(" to target ");
              // Serial.println(targetValue);
            }
          }
        }
        if (currentLine.startsWith("GET /getData")) {
          String json = "{\"time\":" + String(millis() / 1000.0);
          json += ",\"target\":[";
          for (int i = 0; i < NMOTORS; i++) {
            json += String(target[i]);
            if (i < NMOTORS - 1) json += ",";
          }
          json += "],\"actual\":[";
          for (int i = 0; i < NMOTORS; i++) {
            json += String(posi[i]);
            if (i < NMOTORS - 1) json += ",";
          }
          json += "]}";

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();
          client.println(json);
          break;
        }
      }
    }
    client.stop();
  }

  // Motor control logic
  if (digitalRead(resetPin) == HIGH) {
    for (int k = 0; k < NMOTORS; k++) {
      posi[k] = 0;
    }
    delay(2000);
  }

  long currT = micros();
  float deltaT = ((float)(currT - prevT)) / (1.0e6);
  prevT = currT;

  int pos[NMOTORS];
  noInterrupts();
  for (int k = 0; k < NMOTORS; k++) {
    pos[k] = posi[k];
  }
  interrupts();

  for (int k = 0; k < NMOTORS; k++) {
    int pwr, dir;
    pid[k].evalu(pos[k], target[k], deltaT, pwr, dir);
    setMotor(dir, pwr, pwm[k], in1[k]);
  }

  for (int k = 0; k < NMOTORS; k++) {
    Serial.print("motor");
    Serial.print(k + 1);
    Serial.print(" ");
    Serial.print(target[k]);
    Serial.print(" ");
    Serial.print(pos[k]);
    Serial.print(" ");
  }
  Serial.println();
}

void setMotor(int dir, int pwmVal, int pwm, int in1) {
  analogWrite(pwm, pwmVal);
  if (dir == 1) {
    digitalWrite(in1, HIGH);
  } else if (dir == -1) {
    digitalWrite(in1, LOW);
  } else {
    digitalWrite(in1, LOW);
  }
}

template<int j>
void readEncoder() {
  int b = digitalRead(encb[j]);
  if (b > 0) {
    posi[j]++;
  } else {
    posi[j]--;
  }
}
