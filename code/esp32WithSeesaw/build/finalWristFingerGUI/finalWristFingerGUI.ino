#include "Adafruit_seesaw.h"
#include <WiFi.h>
#include <WebServer.h>

Adafruit_seesaw ss;

const char* ssid = "motog52";
const char* password = "12ka442ka1";

WebServer server(80);

class SimplePID
{
private:
    float kp, kd, ki, umax; // Parameters
    float eprev, eintegral; // Storage

public:
    // Constructor
    SimplePID() : kp(1), kd(0.1), ki(0.01), umax(255), eprev(0.0), eintegral(0.0) {}

    // A function to set the parameters
    void setParams(float kpIn, float kdIn, float kiIn, float umaxIn)
    {
        kp = kpIn;
        kd = kdIn;
        ki = kiIn;
        umax = umaxIn;
    }

    // A function to compute the control signal
    void evalu(int value, int target, float deltaT, int &pwr, int &dir)
    {
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
        if (pwr > umax)
        {
            pwr = umax;
        }

        // motor direction
        dir = 1;
        if (u < 0)
        {
            dir = -1;
        }

        // store previous error
        eprev = e;
    }
};

// Define motor pinouts
struct Motor {
  int pwm;
  int dir1;
  int dir2;
  bool isPwmSeesaw;
  bool isDualDir;
  bool reversed;
  int encA;
  int encB;
};

Motor fingerMotors[] = {
  {7, 24, -1, true, false, true, -1, -1},    // finger1 
  {6, 25, -1, true, false, true, -1, -1},   // finger2 
  {19, 10, -1, false, false, true, -1, -1},  // finger3 
  {23, 11, -1, false, false, true, -1, -1}, // finger4 
};

Motor wristMotors[] = {
  {18, 14, -1, false, false, false, 16, 4},    // wrist1 (reversed)
  {5, 9, -1, true, false, false, 5, 17},   // wrist2 
};

const int fingerMotorCount = sizeof(fingerMotors) / sizeof(fingerMotors[0]);
const int wristMotorCount = sizeof(wristMotors) / sizeof(wristMotors[0]);

long prevT = 0;
volatile int posi[] = {0, 0};
SimplePID pid[2];

// Target positions for wrist motors
int target[] = {0, 0};

void setup() {
  Serial.begin(115200);
  
  while (!Serial) delay(10);   // wait until serial port is opened
  
  if(!ss.begin()){
    Serial.println("seesaw not found!");
    while(1) delay(10);
  }
  
  Serial.println(F("seesaw started OK!"));

  // Set up finger motors
  for (int i = 0; i < fingerMotorCount; i++) {
    if (fingerMotors[i].isPwmSeesaw) {
      ss.pinMode(fingerMotors[i].pwm, OUTPUT);
    } else {
      pinMode(fingerMotors[i].pwm, OUTPUT);
    }
    ss.pinMode(fingerMotors[i].dir1, OUTPUT);
    if (fingerMotors[i].isDualDir) {
      ss.pinMode(fingerMotors[i].dir2, OUTPUT);
    }
  }

  // Set up wrist motors
  for (int i = 0; i < wristMotorCount; i++) {
    if (wristMotors[i].isPwmSeesaw) {
      ss.pinMode(wristMotors[i].pwm, OUTPUT);
    } else {
      pinMode(wristMotors[i].pwm, OUTPUT);
    }
    ss.pinMode(wristMotors[i].dir1, OUTPUT);
    if (wristMotors[i].isDualDir) {
      ss.pinMode(wristMotors[i].dir2, OUTPUT);
    }
    pinMode(wristMotors[i].encA, INPUT);
    pinMode(wristMotors[i].encB, INPUT);
    pid[i].setParams(1, 0, 0, 128); // Set PID parameters
  }

  attachInterrupt(digitalPinToInterrupt(wristMotors[0].encA), readEncoder<0>, RISING);
  attachInterrupt(digitalPinToInterrupt(wristMotors[1].encA), readEncoder<1>, RISING);

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

  int pos[2];
  noInterrupts();
  for (int i = 0; i < wristMotorCount; i++) {
    pos[i] = posi[i];
  }
  interrupts();

  // Apply PID control to wrist motors
  for (int i = 0; i < wristMotorCount; i++) {
    int pwr, dir;
    pid[i].evalu(pos[i], target[i], 0.1, pwr, dir);
    setWristMotorDirection(i, dir == 1);
    setWristMotorSpeed(i, pwr);
  }

  for (int i = 0; i < wristMotorCount; i++) {
    Serial.print("Wrist Motor ");
    Serial.print(i);
    Serial.print(": Target = ");
    Serial.print(target[i]);
    Serial.print(", Position = ");
    Serial.print(pos[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Motor Control</h1>";
  
  // Finger Motor Controls
  html += "<h2>Finger Motor Control</h2>";
  html += "<button onclick=\"control('all_fingers', 'forward')\">All Fingers Forward</button>";
  html += "<button onclick=\"control('all_fingers', 'backward')\">All Fingers Backward</button>";
  html += "<button onclick=\"control('all_fingers', 'stop')\">All Fingers Stop</button>";
  
  for (int i = 0; i < fingerMotorCount; i++) {
    html += "<h3>Finger " + String(i+1) + "</h3>";
    html += "<button onclick=\"control('finger" + String(i) + "', 'forward')\">Forward</button>";
    html += "<button onclick=\"control('finger" + String(i) + "', 'backward')\">Backward</button>";
    html += "<button onclick=\"control('finger" + String(i) + "', 'stop')\">Stop</button>";
  }
  
  // Wrist Motor Controls
  html += "<h2>Wrist Motor Control</h2>";
  html += "<button onclick=\"control('all_wrists', 'forward')\">All Wrists Forward</button>";
  html += "<button onclick=\"control('all_wrists', 'backward')\">All Wrists Backward</button>";
  html += "<button onclick=\"control('all_wrists', 'stop')\">All Wrists Stop</button>";
  
  for (int i = 0; i < wristMotorCount; i++) {
    html += "<h3>Wrist " + String(i+1) + "</h3>";
    html += "<button onclick=\"control('wrist" + String(i) + "', 'forward')\">Forward</button>";
    html += "<button onclick=\"control('wrist" + String(i) + "', 'backward')\">Backward</button>";
    html += "<button onclick=\"control('wrist" + String(i) + "', 'stop')\">Stop</button>";
  }
  
  html += "<h2>Special Controls</h2>";
  html += "<button onclick=\"control('pitch', '')\">Pitch</button>";
  html += "<button onclick=\"control('pitch_counter', '')\">Pitch Counter</button>";
  
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
  String motorStr = server.arg("motor");
  String action = server.arg("action");

  if (motorStr == "all_fingers") {
    for (int i = 0; i < fingerMotorCount; i++) {
      controlFingerMotor(i, action);
    }
  } else if (motorStr == "all_wrists") {
    for (int i = 0; i < wristMotorCount; i++) {
      controlWristMotor(i, action);
    }
  } else if (motorStr.startsWith("finger")) {
    int motor = motorStr.substring(6).toInt();
    if (motor >= 0 && motor < fingerMotorCount) {
      controlFingerMotor(motor, action);
    } else {
      server.send(400, "text/plain", "Invalid finger motor");
      return;
    }
  } else if (motorStr.startsWith("wrist")) {
    int motor = motorStr.substring(5).toInt();
    if (motor >= 0 && motor < wristMotorCount) {
      controlWristMotor(motor, action);
    } else {
      server.send(400, "text/plain", "Invalid wrist motor");
      return;
    }
  } else if (motorStr == "pitch") {
    target[0] += 300;
    target[1] -= 300;
  } else if (motorStr == "pitch_counter") {
    target[0] -= 300;
    target[1] += 300;
  } else {
    server.send(400, "text/plain", "Invalid command");
    return;
  }
  
  server.send(200, "text/plain", "OK");
}

void controlFingerMotor(int motor, String action) {
  if (action == "forward") {
    setFingerMotorDirection(motor, true);
    setFingerMotorSpeed(motor, 128);
  } else if (action == "backward") {
    setFingerMotorDirection(motor, false);
    setFingerMotorSpeed(motor, 128);
  } else if (action == "stop") {
    setFingerMotorSpeed(motor, 0);
  }
}

void controlWristMotor(int motor, String action) {
  if (action == "forward") {
    target[motor] += 200;
  } else if (action == "backward") {
    target[motor] -= 200;
  } else if (action == "stop") {
    target[motor] = posi[motor];
  }
}

void setFingerMotorDirection(int motor, bool forward) {
  bool actualDirection = fingerMotors[motor].reversed ? !forward : forward;
  ss.digitalWrite(fingerMotors[motor].dir1, actualDirection ? HIGH : LOW);
}

void setFingerMotorSpeed(int motor, int speed) {
  if (fingerMotors[motor].isPwmSeesaw) {
    ss.analogWrite(fingerMotors[motor].pwm, map(speed, 0, 255, 0, 65535), 16);
  } else {
    analogWrite(fingerMotors[motor].pwm, speed);
  }
}

void setWristMotorDirection(int motor, bool forward) {
  bool actualDirection = wristMotors[motor].reversed ? !forward : forward;
  ss.digitalWrite(wristMotors[motor].dir1, actualDirection ? HIGH : LOW);
}

void setWristMotorSpeed(int motor, int speed) {
  if (wristMotors[motor].isPwmSeesaw) {
    ss.analogWrite(wristMotors[motor].pwm, map(speed, 0, 255, 0, 65535), 16);
  } else {
    analogWrite(wristMotors[motor].pwm, speed);
  }
}

template <int j>
void readEncoder() {
  int b = digitalRead(wristMotors[j].encB);
  if (b > 0) {
    posi[j]++;
  } else {
    posi[j]--;
  }
}