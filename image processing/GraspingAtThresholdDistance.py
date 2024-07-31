#!/usr/bin/env python3
import cv2
import numpy as np
import RPi.GPIO as GPIO
import time

Known_distance = 20.0
Known_width = 2.5
size_of_hand = 10.0

# Note: Here servo 5 is the wrist motor:
servo_pins = [8, 10, 12, 16, 18, 22]

GPIO.setmode(GPIO.BOARD)
servos = []

# Initialize servo pins and create PWM instances
for pin in servo_pins:
    GPIO.setup(pin, GPIO.OUT)
    servo = GPIO.PWM(pin, 50)
    servo.start(0)
    servos.append(servo)

servo6 = servos[0]  # Servo at pin 8 is the wrist servo

#################################################################################
def move_wrist(cx, frame):
    duty = 7
    servo6.ChangeDutyCycle(duty)  # angle value is 0
    # at duty = 12 angle is 180 degree

    if 0 < duty < 12:
        if cx >= frame.shape[1] / 2:
            duty += 1
        else:
            duty -= 1
        servo6.ChangeDutyCycle(duty)
    elif duty == 0:
        if cx >= frame.shape[1] / 2:
            duty += 1
        else:
            print("please move the ball to the other side, you have reached the lower limit.")
        servo6.ChangeDutyCycle(duty)
    else:
        if cx <= frame.shape[1] / 2:
            duty -= 1
        else:
            print("please move the ball to the other side, you have reached the upper limit.")
        servo6.ChangeDutyCycle(duty)

#############################################################################

def move_fingers():
    if Known_width >= size_of_hand:
        print("sorry, the object is too large to grab")
    else:
        duty = 5  # Placeholder: Replace with the actual calculation for the duty cycle
        for servo in servos[1:]:
            servo.ChangeDutyCycle(duty)

def move_to_original_state():
    servo6.ChangeDutyCycle(7)
    for servo in servos[1:]:
        servo.ChangeDutyCycle(0)

def Focal_Length_Finder(Known_distance, real_width, width_in_rf_image):
    focal_length = (width_in_rf_image * Known_distance) / real_width
    return focal_length

def get_contour_centre(contour):
    # this is actually centroid because of irregular shape
    M = cv2.moments(contour)
    cx = -1
    cy = -1
    if M['m00'] != 0:
        cx = int(M['m10'] / M['m00'])
        cy = int(M['m01'] / M['m00'])
    return cx, cy

def tennis_ball_detection(frame):
    hsv_image = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower = (161, 155, 84)
    upper = (179, 255, 255)
    mask = cv2.inRange(hsv_image, lower, upper)
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    if contours:
        max_c = contours[0]
        max_area = cv2.contourArea(contours[0])
        for c in contours:
            area = cv2.contourArea(c)
            if area > max_area:
                max_area = area
                max_c = c

        black_image = np.zeros((mask.shape[0], mask.shape[1], 3), dtype="uint8")
        cv2.drawContours(black_image, [max_c], -1, (150, 250, 150), 4)
        cx, cy = get_contour_centre(max_c)
        ((x, y), radius) = cv2.minEnclosingCircle(max_c)
        cv2.circle(frame, (cx, cy), int(radius), (50, 255, 80), 2)
        cv2.circle(black_image, (cx, cy), int(radius), (0, 0, 255), 2)
        cv2.circle(black_image, (cx, cy), 2, (0, 0, 255), 2)

        return frame, 2 * int(radius), cx
    else:
        return frame, 0, -1  # No contours found

def Distance_finder(Focal_Length, Known_width, obj_width_in_frame):
    if obj_width_in_frame != 0:
        distance = (Known_width * Focal_Length) / obj_width_in_frame
        return distance
    return None

ref_image = cv2.imread("/home/pi/Pictures/rf.png")
_, ref_image_obj_width, _ = tennis_ball_detection(ref_image)
Focal_length_found = Focal_Length_Finder(Known_distance, Known_width, ref_image_obj_width)
cv2.imshow("ref_image", ref_image)

print(Focal_length_found)

video_capture = cv2.VideoCapture(0)
if video_capture.isOpened():
    print("we have started to capture the video.")

try:
    while True:
        ret, frame = video_capture.read()
        if ret:
            frame = cv2.resize(frame, (0, 0), fx=0.5, fy=0.5)
            frame, width, cx = tennis_ball_detection(frame)
            print(width)
            if width != 0:
                Distance = Distance_finder(Focal_length_found, Known_width, width)
                if Distance:
                    cv2.putText(frame, f"Distance: {round(Distance, 2)} CM", (30, 35), cv2.FONT_HERSHEY_COMPLEX, 0.6, (255, 0, 0), 2)

                move_wrist(cx, frame)
                if Distance and Distance < 10:
                    time.sleep(4)
                    move_fingers()

            cv2.imshow("frame", frame)
            if cv2.waitKey(10) & 0xFF == ord('q'):
                break
finally:
    move_to_original_state()
    for servo in servos:
        servo.stop()
    GPIO.cleanup()
    cv2.destroyAllWindows()



