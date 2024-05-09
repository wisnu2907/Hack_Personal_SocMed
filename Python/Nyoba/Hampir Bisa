import os
import time

import cv2
import numpy as np
import serial.tools.list_ports
import threading
import torch
import serial
from ultralytics import YOLO   

cap = cv2.VideoCapture(0)  
cap.set(cv2.CAP_PROP_AUTOFOCUS, 1)

# Define the colors for the bounding boxes
COLORS = [(0, 255, 0)]

# Load calibration settings if the file exists
calibration_file = "calibration_settings.npz"
if os.path.exists(calibration_file):
    settings = np.load(calibration_file)
    pts1 = settings["pts1"]
    pts2 = settings["pts2"]
    calibrated = True
    print("Calibration settings loaded.")
    
else:
    calibrated = False 

model = YOLO(r"C:\Users\wisnu\Coding\KRTMI2024\Python\best.pt")
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.model.to(device)

Motor1 = serial.Serial("COM8", 9600) #ID 10
Motor2 = serial.Serial("COM7", 9600) #ID 11
Mega = serial.Serial("COM5", 9600, timeout=1)
Arm = serial.Serial("COM9", 1000000)

if Arm.is_open:
    Arm.close()
Arm.open()

if Motor1.is_open:
    Motor1.close()
Motor1.open()

if Motor2.is_open:
    Motor2.close()
Motor2.open()

if Mega.is_open:
    Mega.close()
Mega.open()


# setting device on GPU if available, else CPU)
print("Using device:", device)

# time.sleep(2)   
# Motor1.write("3".encode('utf-8'))
# Motor2.write("3".encode('utf-8'))
# time.sleep(8.30)
# Motor1.write("0".encode('utf-8'))
# Motor2.write("0".encode('utf-8'))

logitune = False
start_time = False
start_time2 = False
Grab = False

counter_tot = 0
t1 = 0
t2 = 0
data = 0

detected_classes = []
class_name = ""

def taruhSampah(detected):
    if detected[-1] == "Ferro":
        Arm.write("1".encode("utf-8"))
    elif detected[-1] == "Non-Ferro":
        Arm.write("2".encode("utf-8"))
    elif detected[-1] == "Plastik-Biru" or detected[-1] == "Plastik-Putih" or detected[-1] == "Botol":
        Arm.write("3".encode("utf-8"))
    elif detected[-1] == "Koran" or detected[-1] == "Kertas-Bungkus":
        Arm.write("4".encode("utf-8"))
    elif detected[-1] == "Daun-Fresh" or detected[-1] == "Daun-Kering":
        Arm.write("5".encode("utf-8"))

def baca_sensor():
    global data
    while True:
        # Membaca data sensor dari Arduino
        data = Mega.readline().decode().rstrip()
        print("Data sensor:", data)
        
# Buat thread untuk membaca sensor
thread_sensor = threading.Thread(target=baca_sensor)
thread_sensor.daemon = True
thread_sensor.start()

while True:
    # Read a frame from the webcam
    ret, frame = cap.read()
    if logitune:
        os.startfile(r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Logitech\LogiTune.lnk")
        logitune = True
        
    # Calibration
    if calibrated:
        # Resize the frame based on the calibration points
        width = int(np.sqrt((pts1[1][0] - pts1[0][0]) ** 2 + (pts1[1][1] - pts1[0][1]) ** 2))
        height = int(np.sqrt((pts1[2][0] - pts1[1][0]) ** 2 + (pts1[2][1] - pts1[1][1]) ** 2))
        matrix = cv2.getPerspectiveTransform(pts1, pts2)
        frame = cv2.warpPerspective(frame, matrix, (width, height))

    # Perform object detection on the frame
    results = model.predict(source=frame, show=False, conf=0.5)
    results[0].boxes = results[0].boxes.to(device)

    # ratio pixel to cm
    ratio_px_cm = 194 / 100
    
    '''
    Proses mencari objek menggunakan kamera    
    '''
    
    
    if len(results[0].boxes) == 0 and counter_tot <5 and not Grab:
        Motor1.write("5".encode('utf-8'))
        Motor2.write("5".encode('utf-8'))  
        Mega.write("1".encode('UTF-8'))
        Arm.write("0 7".encode("utf-8"))
    elif len(results[0].boxes) >= 0 and counter_tot <=5 and Grab :
        Motor1.write("E".encode('utf-8'))
        Motor2.write("E".encode('utf-8')) 
        Mega.write("5".encode('UTF-8')) 
        if data == "1" and not start_time2 :
            Motor1.write("0".encode("utf-8"))   
            Motor2.write("0".encode("utf-8"))
            t2 = time.time()
            start_time2 = True
        if start_time2 and time.time()-t2>3.0:
            Grab = False
            start_time2 = False
            if "Botol" in detected_classes:
                detected_classes.remove("Botol")
        elif start_time2 and time.time()-t2<=2.5:
            Motor1.write("0".encode("utf-8"))   
            Motor2.write("0".encode("utf-8"))
            taruhSampah(detected_classes)
        elif start_time2 and time.time()-t2>2.5 and time.time()-t2<=3.0:
            Motor1.write("0".encode("utf-8"))   
            Motor2.write("0".encode("utf-8"))
            taruhSampah(detected_classes)
            Mega.write("4".encode("UTF-8"))
    
    """
    Ini nanti dulu bang
            
    elif len(results[0].boxes) == 0 and counter_tot <8 and counter_tot>5 and Grab == 0:
        Motor1.write("A".encode('utf-8'))
        Motor2.write("A".encode('utf-8'))  
        # Mega.write("5".encode("UTF-8"))
    elif len(results[0].boxes) == 0  and counter_tot <=8 and counter_tot>5 and Grab == 1:
        Motor1.write("1".encode('utf-8'))
        Motor2.write("1".encode('utf-8')) 
        # Arm.write("3".encode("utf-8"))     
        if data == "1" and not start_time2 :
            Motor1.write("0".encode("utf-8"))   
            Motor2.write("0".encode("utf-8"))
            if "Botol" in detected_classes:
                detected_classes.remove("Botol")
            t2 = time.time()
            start_time2 = True
        if start_time2 and time.time()-t2>3.0:
            Grab = 0
            start_time2 = False        
            # time.sleep(3)
        # time.sleep(5)
        elif start_time2 and time.time()-t2<=3.0:
            Motor1.write("0".encode("utf-8"))   
            Motor2.write("0".encode("utf-8"))
            taruhSampah(class_name)
    """
                        
    # Draw the detection results on the frame
    for box in results[0].boxes:
        x1, y1, x2, y2 = box.xyxy[0]
        conf = box.conf[0]
        cls = box.cls[0]
        class_name = model.names[int(cls)]
        label = f"{model.names[int(cls)]}: {conf:.2f}"
        color = COLORS[int(cls) % len(COLORS)]

        # Draw bounding box
        cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), color, 2)

        center_x_cm = ((int((x1 + x2) / 2) - frame.shape[1] // 2) / ratio_px_cm / 10) 
        center_y_cm = ((frame.shape[0] - y2) / ratio_px_cm / 10)  

        command = f"{center_x_cm + 25-14.5:.5f} {center_y_cm + 20-7.3-3.5:.5f}\n"
        
        cv2.circle(frame, (int(center_x_cm), int(center_y_cm)), 5, (0, 255, 255), -1)

        # Draw labelq
        label_size, _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 2)
        cv2.putText(frame, label, (int(x1), int(y2) + label_size[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2,)
        cv2.putText(frame, label, (int(x1), int(y1) - 40), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2,)

        # Draw center coordinates in centimeters
        cv2.putText(frame, f"Center (cm): ({center_x_cm:.3f} cm, {center_y_cm:.3f} cm)", (int(x1), int(y2) + label_size[1] + 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2,)
        cv2.putText(frame, f"Center (cm): ({center_x_cm:.3f} cm, {center_y_cm:.3f} cm)", (int(x1), int(y1) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2,)
        
        if center_x_cm > 4.1:
            Motor1.write("1".encode('utf-8'))
            Motor2.write("1".encode('utf-8'))
        elif center_x_cm < 3.0 and center_x_cm > -3.0 :
            Motor1.write("0".encode('utf-8'))
            Motor2.write("0".encode('utf-8')) 
            if class_name not in detected_classes and not start_time and conf>0.7:
                detected_classes.append(class_name)               
                t1 = time.time()
                print("nunggu 2detik")
                start_time = True
            if start_time and time.time()-t1>5.7:
                counter_tot+=1
                Grab = True              
                start_time = False
                Mega.write("5".encode("UTF-8"))
                taruhSampah(detected_classes)
            elif start_time and time.time()-t1<=2.5 :
                Arm.write(command.encode("utf-8"))
                Mega.write("2".encode("UTF-8"))
            elif start_time and time.time()-t1>2.5 and time.time()-t1>=3.5:
                Arm.write(command.encode("utf-8"))
                Mega.write("3".encode("UTF-8"))
            elif start_time and time.time()-t1>3.5 and time.time()-t1<=5.7:
                Arm.write(command.encode("utf-8"))
                Mega.write("5".encode("UTF-8"))

    # Display the frame with counter_tot
    cv2.putText(frame, f"Counter_tot: {counter_tot}", (20, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2,)
    
    # Display the frame
    cv2.imshow("Astro_24", frame)

    # Exit if the user presses the 'q' key
    if cv2.waitKey(1) & 0xFF == ord("q"):
        Motor1.write("0".encode('utf-8'))
        Motor2.write("0".encode('utf-8'))
        Motor1.close()
        Motor2.close()
        Mega.close()
        Arm.close()
        break
    
# Release the webcam and destroy all windows
Motor1.close()
Motor2.close()
Mega.close()
Arm.close()
cap.release()
cv2.destroyAllWindows()
exit()
