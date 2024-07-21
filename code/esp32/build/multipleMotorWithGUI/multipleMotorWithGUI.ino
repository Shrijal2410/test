#include <WiFi.h>
#include <WiFiServer.h>

// WiFi credentials
const char* ssid = "motog52";
const char* password = "12ka442ka1";

WiFiServer server(80);

// A class to compute the control signal
class SimplePID {
private:
    float kp, kd, ki, umax; // Parameters
    float eprev, eintegral; // Storage

public:
    // Constructor
    SimplePID() : kp(1), kd(0.1), ki(0.01), umax(255), eprev(0.0), eintegral(0.0) {}

    // A function to set the parameters
    void setParams(float kpIn, float kdIn, float kiIn, float umaxIn) {
        kp = kpIn; kd = kdIn; ki = kiIn; umax = umaxIn;
    }

    // A function to compute the control signal
    void evalu(int value, int target, float deltaT, int &pwr, int &dir) {
        // error
        int e = target - value;
        
        // derivative
        float dedt = (e-eprev)/(deltaT);
        
        // integral
        eintegral = eintegral + e*deltaT;
        
        // control signal
        float u = kp*e + kd*dedt + ki*eintegral;
        
        // motor power
        pwr = (int) fabs(u);
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
const int pwm[] = {15, 5, 21, 14, 33};
const int in1[] = {2, 17, 22, 27, 32};
const int enca[] = {4, 18, 13, 26, 35};
const int encb[] = {16, 19, 12, 25, 34};

int resetPin = 23;

// Globals
long prevT = 0;
volatile int posi[] = {0,0,0,0,0};

// PID class instances
SimplePID pid[NMOTORS];

// Target positions
int target[NMOTORS] = {5000, 5000, 5000, 5000, 5000};

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

    for(int k = 0; k < NMOTORS; k++) {
        pinMode(enca[k],INPUT);
        pinMode(encb[k],INPUT);
        pinMode(pwm[k],OUTPUT);
        pinMode(in1[k],OUTPUT);
        
        pid[k].setParams(1,0,0,255);
    }
    
    attachInterrupt(digitalPinToInterrupt(enca[0]),readEncoder<0>,RISING);
    attachInterrupt(digitalPinToInterrupt(enca[1]),readEncoder<1>,RISING);
    attachInterrupt(digitalPinToInterrupt(enca[2]),readEncoder<2>,RISING);
    attachInterrupt(digitalPinToInterrupt(enca[3]),readEncoder<3>,RISING);
    attachInterrupt(digitalPinToInterrupt(enca[4]),readEncoder<4>,RISING);
    
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
                        
                        // Web page
                        client.println("<html><head><title>Motor Control</title>");
                        client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
                        client.println("<style>body { font-family: Arial; text-align: center; }");
                        client.println(".slider { width: 300px; }</style></head><body>");
                        client.println("<h2>Motor Control</h2>");
                        
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
                        }
                        
                        client.println("<script>");
                        client.println("function updateMotor(motorIndex) {");
                        client.println("  var slider = document.getElementById('motor' + motorIndex);");
                        client.println("  var valueSpan = document.getElementById('motorValue' + motorIndex);");
                        client.println("  var value = slider.value;");
                        client.println("  valueSpan.textContent = value;");
                        client.println("  var xhr = new XMLHttpRequest();");
                        client.println("  xhr.open('GET', '/setTarget?motor=' + motorIndex + '&target=' + value, true);");
                        client.println("  xhr.send();");
                        client.println("}");
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
                            Serial.print("Updated motor ");
                            Serial.print(motorIndex);
                            Serial.print(" to target ");
                            Serial.println(targetValue);
                        }
                    }
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
    float deltaT = ((float) (currT - prevT)) / (1.0e6);
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
    }
    else if (dir == -1) {
        digitalWrite(in1, LOW);
    }
    else {
        digitalWrite(in1, LOW);
    }
}

template <int j>
void readEncoder() {
    int b = digitalRead(encb[j]);
    if (b > 0) {
        posi[j]++;
    }
    else {
        posi[j]--;
    }
}