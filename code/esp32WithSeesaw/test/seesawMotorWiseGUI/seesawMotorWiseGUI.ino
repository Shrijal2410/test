#include "Adafruit_seesaw.h"
#include <WiFi.h>
#include <WebServer.h>

Adafruit_seesaw ss;

const char* ssid = "motog52";
const char* password = "12ka442ka1";

WebServer server(80);

// Define motor pinouts
struct Motor {
  int pwm;
  int dir1;
  int dir2;  // Added for 6th motor
  bool isPwmSeesaw;
  bool isDualDir;  // Flag for dual direction motor
  bool reversed; 
};

// {s7, s24, -1, true, false},   // motor1
// {s6, s25, -1, true, false}, //motor2
// {19, s10, -1, false, false}, //motor3
// {23, s11, -1, true, false}, //motor4
// {s5, s9, -1, true, false}, //motor5



Motor motors[] = {
  {7, 24, -1, true, false,true},   // motor1 ulta
  {6, 25, -1, true, false,false},  // motor2 
  {23, 11, -1, false, false,false}, // motor3 
  {19, 10, -1, false, false,true}, // motor4 ulta
  {5, 9, -1, true, false,true},  // motor5 ulta
  // {17, 10, 4, false, true},   // motor6 with dual direction pins
};

const int motorCount = sizeof(motors) / sizeof(motors[0]);

void setup() {
  Serial.begin(115200);
  
  while (!Serial) delay(10);   // wait until serial port is opened
  
  if(!ss.begin()){
    Serial.println("seesaw not found!");
    while(1) delay(10);
  }
  
  Serial.println(F("seesaw started OK!"));

  // Set the pins as outputs for all motors
  for (int i = 0; i < motorCount; i++) {
    if (motors[i].isPwmSeesaw) {
      ss.pinMode(motors[i].pwm, OUTPUT);
    } else {
      pinMode(motors[i].pwm, OUTPUT);
    }
    ss.pinMode(motors[i].dir1, OUTPUT);
    if (motors[i].isDualDir) {
      ss.pinMode(motors[i].dir2, OUTPUT);
    }
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/control", HTTP_POST, handleControl);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Motor Control</h1>";
  for (int i = 0; i < motorCount; i++) {
    html += "<h2>Motor " + String(i+1) + "</h2>";
    html += "<button onclick=\"control(" + String(i) + ", 'forward')\">Forward</button>";
    html += "<button onclick=\"control(" + String(i) + ", 'backward')\">Backward</button>";
    html += "<button onclick=\"control(" + String(i) + ", 'stop')\">Stop</button>";
  }
  html += "<script>";
  html += "function control(motor, action) {";
  html += "  fetch('/control', {";
  html += "    method: 'POST',";
  html += "    headers: {'Content-Type': 'application/x-www-form-urlencoded'},";
  html += "    body: 'motor=' + motor + '&action=' + action";
  html += "  }).then(response => response.text()).then(data => console.log(data));";
  html += "}";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleControl() {
  int motor = server.arg("motor").toInt();
  String action = server.arg("action");

  if (motor >= 0 && motor < motorCount) {
    if (action == "forward") {
      setMotorDirection(motor, true);
      setMotorSpeed(motor, 64);
    } else if (action == "backward") {
      setMotorDirection(motor, false);
      setMotorSpeed(motor, 64);
    } else if (action == "stop") {
      setMotorSpeed(motor, 0);
    }
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Invalid motor");
  }
}

void setMotorDirection(int motor, bool forward) {
  if (motors[motor].isDualDir) {
    if (forward) {
      ss.digitalWrite(motors[motor].dir1, HIGH);
      ss.digitalWrite(motors[motor].dir2, LOW);
    } else {
      ss.digitalWrite(motors[motor].dir1, LOW);
      ss.digitalWrite(motors[motor].dir2, HIGH);
    }
  } else {
    bool actualDirection = motors[motor].reversed ? !forward : forward;
    ss.digitalWrite(motors[motor].dir1, forward ? HIGH : LOW);
  }
}

void setMotorSpeed(int motor, int speed) {
  if (motors[motor].isPwmSeesaw) {
    ss.analogWrite(motors[motor].pwm, map(speed, 0, 255, 0, 65535), 16);
  } else {
    analogWrite(motors[motor].pwm, speed);
  }
}