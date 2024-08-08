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

Motor motors[] = {
  {7, 24, -1, true, false, true, -1, -1},    // motor1 
  {6, 25, -1, true, false, true,-1,-1},   // motor2 
  {19, 10, -1, false, false, true,-1,-1},  // motor3 
  {23, 11, -1, false, false, true,-1,-1}, //motor 4
  {18, 14, -1, false, false, false, 16, 4},    // motor5 (reversed)
  {5, 9, -1, true, false, false, 5, 17},   //  motor6
};

const int motorCount = sizeof(motors) / sizeof(motors[0]);

long prevT = 0;
volatile int posi[] = {0, 0};
SimplePID pid[2];

// Target positions
int target[] = {0, 0};

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

  for (int i = 4; i < motorCount; i++) {
    pinMode(motors[i].encA, INPUT);
    pinMode(motors[i].encB, INPUT);
    pid[i-4].setParams(1, 0, 0, 128); // Set PID parameters
  }

  attachInterrupt(digitalPinToInterrupt(motors[4].encA), readEncoder<4>, RISING);
  attachInterrupt(digitalPinToInterrupt(motors[5].encA), readEncoder<5>, RISING);

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
  for (int i = 4; i < motorCount; i++) {
    pos[i - 4] = posi[i - 4];
  }
  interrupts();

  // Apply PID control to each motor
  for (int i = 4; i < motorCount; i++) {
    int pwr, dir;
    pid[i - 4].evalu(pos[i - 4], target[i], 0.1, pwr, dir); // Updated to include PID control
    setMotorDirection(i, dir == 1);
    setMotorSpeed(i, pwr);
  }

  // for (int i = 0; i < motorCount; i++) {
  //   Serial.print("Motor ");
  //   Serial.print(i);
  //   Serial.print(": Target = ");
  //   Serial.print(target[i]);
  //   Serial.print(", Position = ");
  //   Serial.print(pos[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Motor Control</h1>";
  
  // Master Control for All Motors
  html += "<h2>Master Control</h2>";
  html += "<button onclick=\"control('all_fingers', 'forward')\">All Fingers Forward</button>";
  html += "<button onclick=\"control('all_fingers', 'backward')\">All Fingers Backward</button>";
  html += "<button onclick=\"control('all_fingers', 'stop')\">All Fingers Stop</button>";
  html += "<button onclick=\"control('all_wrists', 'forward')\">All Wrists Forward</button>";
  html += "<button onclick=\"control('all_wrists', 'backward')\">All Wrists Backward</button>";
  html += "<button onclick=\"control('all_wrists', 'stop')\">All Wrists Stop</button>";

  // Individual Motor Control
  html += "<h2>Individual Motor Control</h2>";
  
  for (int i = 0; i < 4; i++) {
    html += "<h3>Finger " + String(i+1) + "</h3>";
    html += "<button onclick=\"control('finger" + String(i+1) + "', 'forward')\">Forward</button>";
    html += "<button onclick=\"control('finger" + String(i+1) + "', 'backward')\">Backward</button>";
    html += "<button onclick=\"control('finger" + String(i+1) + "', 'stop')\">Stop</button>";
  }
  
  for (int i = 0; i < 2; i++) {
    html += "<h3>Wrist " + String(i+1) + "</h3>";
    html += "<button onclick=\"control('wrist" + String(i+1) + "', 'forward')\">Forward</button>";
    html += "<button onclick=\"control('wrist" + String(i+1) + "', 'backward')\">Backward</button>";
    html += "<button onclick=\"control('wrist" + String(i+1) + "', 'stop')\">Stop</button>";
  }

  // Special Controls
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

  if (motorStr == "all") {
    for (int i = 0; i < motorCount; i++) {
      controlMotor(i, action);
    }
    server.send(200, "text/plain", "OK");
  } else if (motorStr == "pitch") {
    target[0] += 300;  // First motor target increases by 300
    target[1] -= 300;  // Second motor target decreases by 300
    server.send(200, "text/plain", "Pitch set");
  } else if (motorStr == "pitch_counter") {
    target[0] -= 300;  // First motor target decreases by 300
    target[1] += 300;  // Second motor target increases by 300
    server.send(200, "text/plain", "Pitch Counter set");
  } else {
    int motor = motorStr.toInt();
    if (motor >= 0 && motor < motorCount) {
      controlMotor(motor, action);
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Invalid motor");
    }
  }
}


void controlMotor(int motor, String action) {
  if (action == "forward") {
    target[motor] += 200; // Increase target by 200 encoder counts for forward movement
  } else if (action == "backward") {
    target[motor] -= 200; // Decrease target by 200 encoder counts for backward movement
  } else if (action == "stop") {
    target[motor] = posi[motor]; // Stop at current position
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
    ss.digitalWrite(motors[motor].dir1, actualDirection ? HIGH : LOW);
  }
}

void setMotorSpeed(int motor, int speed) {
  if (motors[motor].isPwmSeesaw) {
    ss.analogWrite(motors[motor].pwm, map(speed, 0, 255, 0, 65535), 16);
  } 
  else {
    analogWrite(motors[motor].pwm, speed);
  }
}

template <int j>
void readEncoder() {
  int b = digitalRead(motors[j].encB);
  if (b > 0) {
    posi[j]++;
  } else {
    posi[j]--;
  }
}
