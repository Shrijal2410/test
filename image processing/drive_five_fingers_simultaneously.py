#!/usr/bin/env python3
import RPi.GPIO as GPIO
import time
import threading

# Pin definitions 
servo_pins = [18, 10, 12, 22, 16]

# Frequency in Hz
frequency = 50

# Setup
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)

# Initialize servo pins and create PWM instances
GPIO.setup(servo_pins[0], GPIO.OUT)
servo1 = GPIO.PWM(servo_pins[0], frequency)
servo1.start(0)

GPIO.setup(servo_pins[1], GPIO.OUT)
servo2 = GPIO.PWM(servo_pins[1], frequency)
servo2.start(0)

GPIO.setup(servo_pins[2], GPIO.OUT)
servo3 = GPIO.PWM(servo_pins[2], frequency)
servo3.start(0)

GPIO.setup(servo_pins[3], GPIO.OUT)
servo4 = GPIO.PWM(servo_pins[3], frequency)
servo4.start(0)

GPIO.setup(servo_pins[4], GPIO.OUT)
servo5 = GPIO.PWM(servo_pins[4], frequency)
servo5.start(0)

def set_angle(servo, pin, angle):
    duty = 2 + (angle / 18)
    GPIO.output(pin, True)
    servo.ChangeDutyCycle(duty)
    time.sleep(0.5)
    GPIO.output(pin, False)
    servo.ChangeDutyCycle(0)

def move_servo(servo, pin, angles):
    for angle in angles:
        set_angle(servo, pin, angle)
        time.sleep(1)  # Adjust the delay as needed

try:
    # Define the angles to move to for each servo
    angles_servo1 = [0,180]
    angles_servo2 = [0, 90, 180, 0]
    angles_servo3 = [0, 90, 180, 0]
    angles_servo4 = [0, 90, 180, 0]
    angles_servo5 = [0, 90, 180, 0]

    # Create threads for each servo
   # thread1 = threading.Thread(target=move_servo, args=(servo1, servo_pins[0], angles_servo1))
  #  thread2 = threading.Thread(target=move_servo, args=(servo2, servo_pins[1], angles_servo2))
    thread3 = threading.Thread(target=move_servo, args=(servo3, servo_pins[2], angles_servo3))
    thread4 = threading.Thread(target=move_servo, args=(servo4, servo_pins[3], angles_servo4))
    thread5 = threading.Thread(target=move_servo, args=(servo5, servo_pins[4], angles_servo5))

    # Start the threads
  #  thread1.start()
    thread3.start()
  #  thread2.start()
    thread4.start()
    thread5.start()

    # Wait for all threads to complete
  #  thread1.join()
   # thread2.join()
    thread3.join()
    thread4.join()
    thread5.join()
    move_servo(servo1,servo_pins[0], angles_servo1)
    move_servo(servo2,servo_pins[1], angles_servo1)
except KeyboardInterrupt:
    # Stop all servos
    servo1.stop()
    servo2.stop()
    servo3.stop()
    servo4.stop()
    servo5.stop()
    GPIO.cleanup()

