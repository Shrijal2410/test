#include "Adafruit_seesaw.h"


Adafruit_seesaw ss;



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
    {18, 14, -1, false, false, false, 16, 4},    // motor1 (reversed)
    {5, 9, -1, true, false, false, 5, 17},       // motor2 
};

const int motorCount = sizeof(motors) / sizeof(motors[0]);

long prevT = 0;
volatile int posi[] = {0, 0};
SimplePID pid[2];

// Target positions
int target[] = {200, -200};

void setup() {
    Serial.begin(115200);

    while (!Serial) delay(10);   // wait until serial port is opened

    if(!ss.begin()){
        Serial.println("Seesaw not found!");
        while(1) delay(10);
    }
    
    Serial.println(F("Seesaw started OK!"));

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

    for (int i = 0; i < motorCount; i++) {
        pinMode(motors[i].encA, INPUT);
        pinMode(motors[i].encB, INPUT);
        pid[i].setParams(1, 0, 0, 128); // Set PID parameters
    }

    attachInterrupt(digitalPinToInterrupt(motors[0].encA), readEncoder<0>, RISING);
    attachInterrupt(digitalPinToInterrupt(motors[1].encA), readEncoder<1>, RISING);

    
   
}

void loop() {
    // time difference
    long currT = micros();
    float deltaT = ((float)(currT - prevT)) / (1.0e6);
    prevT = currT;

    // Read the position
    int pos[motorCount];
    noInterrupts(); // disable interrupts temporarily while reading
    for (int k = 0; k < motorCount; k++) {
        pos[k] = posi[k];
    }
    interrupts(); // turn interrupts back on

    // loop through the motors
    for (int k = 0; k < motorCount; k++) {
        int pwr, dir;
        // evaluate the control signal
        pid[k].evalu(pos[k], target[k], deltaT, pwr, dir);
        // signal the motor
        setMotor(k, dir, pwr);
    }

    for (int k = 0; k < motorCount; k++) {
        Serial.print("motor");
        Serial.print(k + 1); // Adding 1 to start motor names from 1 instead of 0
        Serial.print(" ");
        Serial.print(target[k]);
        Serial.print(" ");
        Serial.print(pos[k]);
        Serial.print(" ");
    }

    Serial.println();

   
}

void setMotor(int motorIndex, int dir, int pwmVal) {
    Motor motor = motors[motorIndex];

    if (motor.isPwmSeesaw) {
        ss.analogWrite(motor.pwm, pwmVal);
    } else {
        analogWrite(motor.pwm, pwmVal);
    }

    if (dir == 1) {
        ss.digitalWrite(motor.dir1, HIGH);
        if (motor.isDualDir) {
            ss.digitalWrite(motor.dir2, LOW);
        }
    } else if (dir == -1) {
        ss.digitalWrite(motor.dir1, LOW);
        if (motor.isDualDir) {
            ss.digitalWrite(motor.dir2, HIGH);
        }
    } else {
        ss.digitalWrite(motor.dir1, LOW);
        if (motor.isDualDir) {
            ss.digitalWrite(motor.dir2, LOW);
        }
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


