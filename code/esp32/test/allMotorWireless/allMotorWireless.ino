#include <WiFi.h>
#include <WebServer.h>

// Define pin connections for 7 motors
const int ena[] = { 33, 25, 26, 27, 14, 12, 13 };  // PWM pins for speed control
const int dir[] = { 18, 5, 17, 16, 4, 2, 15 };     // Direction control pins

// SSID and Password of your WiFi router
const char* ssid = "motog52";
const char* password = "12ka442ka1";

// Create a web server object
WebServer server(80);

// HTML webpage contents with added style
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f0f0f0; }
        .container { width: 300px; margin: 0 auto; padding: 20px; background-color: white; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; }
        button { display: block; width: 100%; padding: 10px; margin: 10px 0; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }
        button:hover { background-color: #45a049; }
    </style>
</head>
<body>
<div class="container">
    <h1>Motor Control</h1>
    <button onclick="location.href='/startSimultaneous'">Start Motors Simultaneously</button>
    <button onclick="location.href='/startSequential'">Start Motors Sequentially</button>
    <button onclick="location.href='/stop'">Stop Motors</button>
</div>
</body>
</html>
)=====";

void setup() {
  Serial.begin(115200);
  
  // Set the pins as outputs for all motors
  for (int i = 0; i < 7; i++) {
    pinMode(ena[i], OUTPUT);
    pinMode(dir[i], OUTPUT);
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Define server routes
  server.on("/", handleRoot);
  server.on("/startSimultaneous", handleStartSimultaneous);
  server.on("/startSequential", handleStartSequential);
  server.on("/stop", handleStop);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleStartSimultaneous() {
  for (int i = 0; i < 7; i++) {
    // if(i==3){
    //   digitalWrite(dir[i],LOW);
    // }
    // else{
    //   digitalWrite(dir[i], HIGH);
    // }
    digitalWrite(dir[i], HIGH);
    analogWrite(ena[i], 255);
  }
  server.send(200, "text/plain", "Motors started simultaneously");
  Serial.println("All motors running simultaneously");
}

void handleStartSequential() {
  for (int i = 0; i < 7; i++) {
    // if(i==3){
    //   digitalWrite(dir[i],LOW);
    // }
    // else{
    //   digitalWrite(dir[i], HIGH);
    // }
    digitalWrite(dir[i], HIGH);
    analogWrite(ena[i], 255);
    delay(500);  // 500ms delay between each motor start
    Serial.print("Motor ");
    Serial.print(i+1);
    Serial.println(" started");
  }
  server.send(200, "text/plain", "Motors started sequentially");
  Serial.println("All motors running sequentially");
}

void handleStop() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(dir[i], LOW);
    analogWrite(ena[i], 0);
  }
  server.send(200, "text/plain", "Motors stopped");
  Serial.println("All motors stopped");
}