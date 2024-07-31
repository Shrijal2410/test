# #!/usr/bin/env python3
# import cv2
# import numpy as np
# import RPi.GPIO as GPIO
# import time

# # note : here servo 5 is the wrist motor : 

# GPIO.setmode(GPIO.BOARD)
# GPIO.setup(35,GPIO.OUT)
# GPIO.setup(33,GPIO.OUT)
# GPIO.setup(31,GPIO.OUT)
# GPIO.setup(29,GPIO.OUT)

# servo5=GPIO.PWM(35,50)
# servo6=GPIO.PWM(33,50)
# servo7=GPIO.PWM(31,50)
# servo8=GPIO.PWM(29,50)

# #################################################################################
# def move_wrist(cx,frame):

#    servo5.start(0)
#    time.sleep(2)
#    duty=7 
#    servo5.ChangeDutyCycle(duty) # angle value is 0
#    # at duty =12 angle is 180 degree

#    if(duty>0 and duty<12):
#        if (cx >= (frame.shape[1])/2):
#            duty+=1
#            servo5.ChangeDutyCycle(duty)
#        else:
#            duty-=1
#            servo5.ChangeDutyCycle(duty)
#    elif duty==0:
#        if (cx >= (frame.shape[1])/2):
#            duty+=1
#            servo5.ChangeDutyCycle(duty)
#        else:
#            print("please move the ball to other side you have reached the lower limit.")
#    else:
#        if (cx <= (frame.shape[1])/2):
#            duty-=1
#            servo5.ChangeDutyCycle(duty)
#        else:
#            print("please move the ball to other side you have reached the upper limit.")
       



# # # turning back to 90 degrees for 2 seconds 

# # servo5.ChangeDutyCycle(7)
# # time.sleep(2)

# # print("turning back to 0 degrees.")

# # servo5.ChangeDutyCycle(2)

# # time.sleep(2)


# #############################################################################

# def get_contour_centre(contour):
#     # this is actually centroid kyunki irregular shape hai toh that's why
#     M=cv2.moments(contour)
#     print(M)
#     cx=-1
#     cy=-1
#     if(M['m00']!=0):
#         cx=M['m10']/M['m00']
#         cy=M['m01']/M['m00']
#     return cx,cy

# def tennis_ball_detection(frame):
#    color_image=cv2.imread(frame,cv2.IMREAD_COLOR)
#     # img=cv2.resize(img,(0,0),fx=0.3,fy=0.3)
# #    cv2.imshow("tennis ball image",color_image)
#    hsv_image=cv2.cvtColor(color_image,cv2.COLOR_BGR2HSV)
# #    cv2.imshow("hsv tennis ball image",hsv_image)
#    lower=(30,20,50)
#    upper=(55,255,255)
#    mask=cv2.inRange(hsv_image,lower,upper)
# #    cv2.imshow("masked image",mask)
#    index=-1 # to show all the contours
#    thickness=2
#    color=(255,0,255)
#    contours,heirarchy=cv2.findContours(mask,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
#    cv2.drawContours(color_image,contours,index,color,thickness)
#    # cv2.imshow("image",color_image)

#    # let's start with contour processing now 
#    black_image=np.zeros(((mask.shape)[0],(mask.shape)[1],3),dtype="uint8")
#    max_c=contours[0]
#    max_area=cv2.contourArea(contours[0])
#    for c in contours:
#        area=cv2.contourArea(c)
#        if area>max_area:
#            max_area=area
#            max_c=c

#    print(max_c.shape)
#    area=cv2.contourArea(max_c)
#    perimeter=cv2.arcLength(max_c,True)
#    ((x,y),radius)=cv2.minEnclosingCircle(max_c)
#    radius= int(radius)
#    # cv2.drawContours(color_image,c,-1,color,thickness)
#    cv2.drawContours(black_image,max_c,-1,(150,250,150),4)
#    cx,cy=get_contour_centre(max_c)
#    cx=int(cx)
#    cy=int(cy)
#    cv2.circle(color_image,(cx,cy),radius,(50,255,80),2)
#    cv2.circle(black_image,(cx,cy),radius,(0,0,255),2)
#    cv2.circle(black_image,(cx,cy),2,(0,0,255),2)
   
# #       print("Area : {} Perimeter : {}\n".format(area,perimeter))
# #    print("number of contours : {}".format(len(contours)))
#    return(color_image,cx)
    
  
# #    cv2.imshow("countoured_circle_image",color_image)
# #    cv2.imshow("only_contoured_image",black_image)

# ################################################################################

# video_capture = cv2.VideoCapture(0) 
# if(video_capture.isOpened()):
#     print("we have started to capture the video.")

# while(True):
# # this method returns the next image frame in frame, together with a boolean value ret
# # that is true if an image frame has been successfully grabbed or conversely false
# # if the camera has returned an empty image the latter happens when the camera is disconnected
#     ret,frame=video_capture.read()
#     if ret:
#         frame=cv2.resize(frame,(0,0),fx=0.5,fx=0.5)
#         frame,cx=tennis_ball_detection(frame)

#         cv2.imshow("frame",frame)
#         move_wrist(cx,frame)
#         # A key is presed in 10 ms and it is q 
#         if cv2.waitKey(10) & 0xFF==ord('q'):
#             break

# servo5.stop()
# 
# # print("GOODBYE")
# cv2.waitKey(0)
# cv2.destroyAllWindows()


#!/usr/bin/env python3
import cv2
import numpy as np
import RPi.GPIO as GPIO
import time

# Note: Here servo 5 is the wrist motor:

GPIO.setmode(GPIO.BOARD)
GPIO.setup(8, GPIO.OUT)
servo5 = GPIO.PWM(8, 50)

def move_wrist(cx, frame):
    servo5.start(0)
    time.sleep(2)
    duty = 7  # Initial duty cycle

    if cx == -1:  # No ball detected
        # Keep the motor in the current position (do nothing)
        pass
    else:
        if 0 < duty < 12:
            if cx >= frame.shape[1] / 2:
                duty += 1
            else:
                duty -= 1
            servo5.ChangeDutyCycle(duty)
        elif duty == 0:
            if cx >= frame.shape[1] / 2:
                duty += 1
            else:
                print("Please move the ball to the other side; you have reached the lower limit.")
        else:
            if cx <= frame.shape[1] / 2:
                duty -= 1
            else:
                print("Please move the ball to the other side; you have reached the upper limit.")
        servo5.ChangeDutyCycle(duty)

def get_contour_centre(contour):
    M = cv2.moments(contour)
    cx = -1
    cy = -1
    if M['m00'] != 0:
        cx = int(M['m10'] / M['m00'])
        cy = int(M['m01'] / M['m00'])
    return cx, cy

def tennis_ball_detection(frame):
    hsv_image = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower = (30, 20, 50)
    upper = (55, 255, 255)
    mask = cv2.inRange(hsv_image, lower, upper)
    contours, hierarchy = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    cx = -1  # Default value if no ball is detected
    if contours:
        max_c = max(contours, key=cv2.contourArea)  # Find contour with maximum area
        max_area = cv2.contourArea(max_c)
        if max_area > 1000:  # Adjust this threshold as per your requirement
            cv2.drawContours(frame, [max_c], -1, (150, 250, 150), 4)
            cx, cy = get_contour_centre(max_c)
            cv2.circle(frame, (cx, cy), int(cv2.minEnclosingCircle(max_c)[1]), (50, 255, 80), 2)

    return frame, cx

video_capture = cv2.VideoCapture(0, cv2.CAP_V4L)
if video_capture.isOpened():
    print("Started capturing video.")

while True:
    ret, frame = video_capture.read()
    if ret:
        frame = cv2.resize(frame, (0, 0), fx=0.5, fy=0.5)
        frame, cx = tennis_ball_detection(frame)

        cv2.imshow("frame", frame)
        move_wrist(cx, frame)

        if cv2.waitKey(10) & 0xFF == ord('q'):
            break

servo5.stop()
GPIO.cleanup()
cv2.destroyAllWindows()
