// A class to compute the control signal
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
volatile int posi[] = {0, 0, 0, 0, 0};

// PID class instances
SimplePID pid[NMOTORS];

void setup()
{
    Serial.begin(115200);

    pinMode(resetPin, INPUT);

    for (int k = 0; k < NMOTORS; k++)
    {
        pinMode(enca[k], INPUT);
        pinMode(encb[k], INPUT);
        pinMode(pwm[k], OUTPUT);
        pinMode(in1[k], OUTPUT);

        pid[k].setParams(1, 0, 0, 255);
    }

    // attachInterrupt(digitalPinToInterrupt(enca[0]),readEncoder<0>,CHANGE);
    // attachInterrupt(digitalPinToInterrupt(enca[1]),readEncoder<1>,CHANGE);
    // attachInterrupt(digitalPinToInterrupt(enca[2]),readEncoder<2>,CHANGE);
    // attachInterrupt(digitalPinToInterrupt(enca[3]),readEncoder<3>,CHANGE);
    // attachInterrupt(digitalPinToInterrupt(enca[4]),readEncoder<4>,CHANGE);

    attachInterrupt(digitalPinToInterrupt(enca[0]), readEncoder<0>, RISING);
    attachInterrupt(digitalPinToInterrupt(enca[1]), readEncoder<1>, RISING);
    attachInterrupt(digitalPinToInterrupt(enca[2]), readEncoder<2>, RISING);
    attachInterrupt(digitalPinToInterrupt(enca[3]), readEncoder<3>, RISING);
    attachInterrupt(digitalPinToInterrupt(enca[4]), readEncoder<4>, RISING);

    for (int k = 0; k < NMOTORS; k++)
    {
        Serial.print("Encoder ");
        Serial.print(k);
        Serial.print(": ");
        Serial.println(posi[k]);
    }

    Serial.println("Resetting encoder values...");

    delay(2000);

    // Reset all encoder values to 0
    for (int k = 0; k < NMOTORS; k++)
    {
        posi[k] = 0;
    }

    for (int k = 0; k < NMOTORS; k++)
    {
        Serial.print("Encoder ");
        Serial.print(k);
        Serial.print(": ");
        Serial.println(posi[k]);
    }
    delay(2000);

    Serial.println("Starting main loop...");
    Serial.println("target pos");
    digitalWrite(resetPin, LOW);
}

void loop()
{

    if (digitalRead(resetPin) == HIGH)
    {
        // Serial.println("Reset pin high. Resetting encoder values...");
        for (int k = 0; k < NMOTORS; k++)
        {
            posi[k] = 0;
        }
        // Serial.println("Encoder values reset to zero.");
        // Optional: Add a small delay to avoid multiple resets
        delay(2000);
    }

    // set target position
    int target[NMOTORS];
    target[0] = 5000;
    target[1] = 5000;
    target[2] = 5000;
    target[3] = 5000;
    target[4] = 5000;

    // time difference
    long currT = micros();
    float deltaT = ((float)(currT - prevT)) / (1.0e6);
    prevT = currT;

    // Read the position
    int pos[NMOTORS];
    noInterrupts(); // disable interrupts temporarily while reading
    for (int k = 0; k < NMOTORS; k++)
    {
        pos[k] = posi[k];
    }
    interrupts(); // turn interrupts back on

    // loop through the motors
    for (int k = 0; k < NMOTORS; k++)
    {
        int pwr, dir;
        // evaluate the control signal
        pid[k].evalu(pos[k], target[k], deltaT, pwr, dir);
        // signal the motor
        setMotor(dir, pwr, pwm[k], in1[k]);
    }

    for (int k = 0; k < NMOTORS; k++)
    {
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

void setMotor(int dir, int pwmVal, int pwm, int in1)
{
    analogWrite(pwm, pwmVal);
    if (dir == 1)
    {
        digitalWrite(in1, HIGH);
    }
    else if (dir == -1)
    {
        digitalWrite(in1, LOW);
    }
    else
    {
        digitalWrite(in1, LOW);
    }
}

template <int j>
void readEncoder()
{
    int b = digitalRead(encb[j]);
    if (b > 0)
    {
        posi[j]++;
    }
    else
    {
        posi[j]--;
    }
}
