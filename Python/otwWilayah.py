import os
import threading
import time
import cv2
import numpy as np
import serial
import torch
from ultralytics import YOLO

# Initialize global flags and variables
logitune = False
turun = False
objek_terdeteksi = False
Sedot = False
Arms = False
tengah = False

center_x_cm=0
center_y_cm=0
count_tot = 0

detected = []
detected_sem=[]
class_name = ""
received_array = []

# Initialize the YOLO model
model = YOLO(r"C:\Users\wisnu\Coding\KRTMI2024\Python\best.pt")
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.model.to(device)

print("Using device:", device)

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_AUTOFOCUS, 1)

# Define the colors for the bounding boxes
COLORS = (255, 255, 255)

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

Mega = serial.Serial("COM9", 9600)
Arm = serial.Serial("COM18", 1000000)
Motor1 = serial.Serial("COM8", 9600) #ID 10
Motor2 = serial.Serial("COM7", 9600) #ID 11

if Arm.is_open:
    Arm.close()
Arm.open()

if Motor1.is_open:
    Motor1.close()
Motor1.open()
# Motor1.flush()

if Motor2.is_open:
    Motor2.close()
Motor2.open()
# Motor2.flush()

if Mega.is_open:
    Mega.close()
Mega.open()

time.sleep(2)  
# Motor1.write("L".encode('utf-8'))
# Motor2.write("L".encode('utf-8'))
# time.sleep(5.20)
# Motor1.write("d".encode('utf-8'))
# Motor2.write("d".encode('utf-8'))
# time.sleep(2)


def baca_sensor():
    global data, received_array
    while True:
        data = Mega.readline().decode().rstrip()
        received_array = list(map(int, data.split(',')))
        # print("Received array:", received_array)

            
def deteksi_objek():
    global  objek_terdeteksi, logitune, ret, frame, cap, class_name, detected, center_x_cm, center_y_cm, command
    
    while True:
        ret, frame = cap.read()
        if not ret:
            continue

        if not logitune:
            os.startfile(r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Logitech\LogiTune.lnk")
            logitune = True

        if calibrated:
            width = int(np.sqrt((pts1[1][0] - pts1[0][0]) ** 2 + (pts1[1][1] - pts1[0][1]) ** 2))
            height = int(np.sqrt((pts1[2][0] - pts1[1][0]) ** 2 + (pts1[2][1] - pts1[1][1]) ** 2))
            matrix = cv2.getPerspectiveTransform(pts1, pts2)
            frame = cv2.warpPerspective(frame, matrix, (width, height))

        results = model.predict(source=frame, show=False, conf=0.8)
        results[0].boxes = results[0].boxes.to(device)

        objek_terdeteksi = False
        for box in results[0].boxes:
            objek_terdeteksi = True
            x1, y1, x2, y2 = box.xyxy[0]
            conf = box.conf[0]
            cls = box.cls[0]
            class_name = model.names[int(cls)]
            label = f"{class_name}: {conf:.3f}"
            # if class_name not in detected or class_name == "Botol":
            

            cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), COLORS, 2)

            ratio_px_cm = 45 / frame.shape[0]

            center_x_cm = ((x1 + x2) / 2 - frame.shape[1] / 2) * ratio_px_cm
            center_y_cm = (frame.shape[0] - ((y1 + y2) / 2)) * ratio_px_cm
            command = f"{center_x_cm + 19:.5f} {center_y_cm+3 :.5f}\n"

            label_size, _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 2)
            cv2.putText(
                frame,
                label,
                (10, 30),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.5,
                COLORS,
                2,
            )

            cv2.putText(
                frame,
                f"Center (cm): ({center_x_cm:.3f} cm, {center_y_cm:.3f} cm)",
                (10, 50),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.5,
                COLORS,
                2,
            )

        cv2.imshow("Astro_24", frame)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

thread_kamera = threading.Thread(target=deteksi_objek)
thread_kamera.daemon = True
thread_kamera.start()

thread_sensor = threading.Thread(target=baca_sensor)
thread_sensor.daemon = True
thread_sensor.start()

def stop():
    Motor1.write("d".encode('utf-8'))
    Motor2.write("d".encode('utf-8'))

def taruhSampah(detected):
    if detected:
        if detected[0] == "Ferro":
            Arm.write("1".encode("utf-8"))
        elif detected[0] == "Non-Ferro":
            Arm.write("2".encode("utf-8"))
        elif detected[0] in ["Plastik-Biru", "Plastik-Putih", "Botol"]:
            Arm.write("3".encode("utf-8"))
        elif detected[0] in ["Koran", "Kertas-Bungkus"]:
            Arm.write("4".encode("utf-8"))
        elif detected[0] in ["Daun-Basah", "Daun-Kering"]:
            Arm.write("5".encode("utf-8"))

def kondisiMaju():
    global received_array, tengah    
    if len(received_array) < 10:
        # print("Error: received_array does not have 10 elements")
        Motor1.write("a".encode('utf-8'))
        Motor2.write("a".encode('utf-8'))
        return
    
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1 and 
        received_array[9] == 1):
        #1 1 1 0 0 0 1 1 1 1 
        Motor1.write("a".encode('utf-8'))
        Motor2.write("a".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 0 0 0 1 1 1
        Motor1.write("b".encode('utf-8'))
        Motor2.write("b".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 1 0 0 0 1 1
        Motor1.write("b".encode('utf-8'))
        Motor2.write("b".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 1 1 0 0 0 0
        Motor1.write("b".encode('utf-8'))
        Motor2.write("b".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 0 0 0 0 0
        Motor1.write("b".encode('utf-8'))
        Motor2.write("b".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 1 0 0 1 1 1 1 1
        Motor1.write("c".encode('utf-8'))
        Motor2.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 0 0 0 1 1 1 1
        Motor1.write("c".encode('utf-8'))
        Motor2.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 0 0 1 1 1 1 1
        Motor1.write("c".encode('utf-8'))
        Motor2.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 1 1 1 1 1 1 1 1
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = False

    else :
        Motor1.write("i".encode('utf-8'))
        Motor2.write("i".encode('utf-8'))

def kondisiMundur():
    global received_array, tengah  
    if len(received_array) < 10:
        print("Error: received_array does not have 10 elements")
        return
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1 and 
        received_array[9] == 1):
        #1 1 1 0 0 0 1 1 1 1 
        Motor1.write("e".encode('utf-8'))
        Motor2.write("e".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 0 0 0 1 1 1
        Motor1.write("f".encode('utf-8'))
        Motor2.write("f".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 1 0 0 0 1 1
        Motor1.write("f".encode('utf-8'))
        Motor2.write("f".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 1 1 0 0 0 0
        Motor1.write("f".encode('utf-8'))
        Motor2.write("f".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 0 0 0 0 0
        Motor1.write("f".encode('utf-8'))
        Motor2.write("f".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 1 0 0 1 1 1 1 1
        Motor1.write("g".encode('utf-8'))
        Motor2.write("g".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 0 0 0 1 1 1 1
        Motor1.write("g".encode('utf-8'))
        Motor2.write("g".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 0 0 1 1 1 1 1
        Motor1.write("g".encode('utf-8'))
        Motor2.write("g".encode('utf-8'))
        tengah = False
    # elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
    #         received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
    #         received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
    #     #1 1 0 0 0 0 0 0 0 0
    #     Motor1.write("d".encode('utf-8'))
    #     Motor2.write("d".encode('utf-8'))
    #     tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 1 1 1 1 1 1 1 1
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    else :
        Motor1.write("h".encode('utf-8'))
        Motor2.write("h".encode('utf-8'))
            
def kondisiMajuKeTengah():
    global received_array, tengah    
    if len(received_array) < 10:
        print("Error: received_array does not have 10 elements")
        return
    
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1 and 
        received_array[9] == 1):
        #1 1 1 0 0 0 1 1 1 1 
        Motor1.write("a".encode('utf-8'))
        Motor2.write("a".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 1 1 1 1 1 1
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 0 0 0 1 1 1
        Motor1.write("b".encode('utf-8'))
        Motor2.write("b".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 1 0 0 0 1 1
        Motor1.write("b".encode('utf-8'))
        Motor2.write("b".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 1 1 0 0 0 0
        Motor1.write("b".encode('utf-8'))
        Motor2.write("b".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 0 0 0 0 0
        Motor1.write("b".encode('utf-8'))
        Motor2.write("b".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 1 0 0 1 1 1 1 1
        Motor1.write("c".encode('utf-8'))
        Motor2.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 0 0 0 1 1 1 1
        Motor1.write("c".encode('utf-8'))
        Motor2.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 0 0 1 1 1 1 1
        Motor1.write("c".encode('utf-8'))
        Motor2.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 0 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #0 0 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    
    
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 0 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #0 0 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True

    else :
        Motor1.write("i".encode('utf-8'))
        Motor2.write("i".encode('utf-8'))

def kondisiMundurKeTengah():
    global received_array, tengah  
    if len(received_array) < 10:
        print("Error: received_array does not have 10 elements")
        return
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1 and 
        received_array[9] == 1):
        #1 1 1 0 0 0 1 1 1 1 
        Motor1.write("e".encode('utf-8'))
        Motor2.write("e".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 0 0 0 1 1 1
        Motor1.write("f".encode('utf-8'))
        Motor2.write("f".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1 and 
            received_array[9] == 1):
        #1 1 1 1 1 0 0 0 1 1
        Motor1.write("f".encode('utf-8'))
        Motor2.write("f".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 1 1 0 0 0 0
        Motor1.write("f".encode('utf-8'))
        Motor2.write("f".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 0 0 0 0 0
        Motor1.write("f".encode('utf-8'))
        Motor2.write("f".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 1 0 0 1 1 1 1 1
        Motor1.write("g".encode('utf-8'))
        Motor2.write("g".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 0 0 0 1 1 1 1
        Motor1.write("g".encode('utf-8'))
        Motor2.write("g".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 and received_array[9] == 1):
        #1 1 0 0 1 1 1 1 1
        Motor1.write("g".encode('utf-8'))
        Motor2.write("g".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 0 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #0 0 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    
    
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 1 1 1 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #1 0 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0 and received_array[9] == 0):
        #0 0 0 0 0 0 0 0 0 0
        Motor1.write("d".encode('utf-8'))
        Motor2.write("d".encode('utf-8'))
        tengah = True
    
    else :
        Motor1.write("h".encode('utf-8'))
        Motor2.write("h".encode('utf-8'))

time.sleep(1.5)            
while True:
    if not  objek_terdeteksi and count_tot < 5 and not Sedot and not tengah :
        Arm.write("0 20\n".encode("utf-8"))
        kondisiMaju()
    if objek_terdeteksi :
        if center_x_cm > 3 and center_x_cm < 7 and not Arms:
            Arm.write("0 20\n".encode("utf-8"))
            Motor1.write("1".encode('utf-8'))
            Motor2.write("1".encode('utf-8')) 
            Arms = False 
        elif center_x_cm <= 3  and center_x_cm >= -3 and not Sedot and not Arms and count_tot < 5:
            detected.append(class_name)
            stop()
            Arm.write(command.encode("utf-8"))
            time.sleep(1)
            Arms = True
            if Arms and not Sedot and not turun and count_tot < 5:
                Arm.write(command.encode("utf-8"))
                time.sleep(0.3)
                Mega.write("2\n".encode("UTF-8"))
                time.sleep(3)
                Mega.write("5\n".encode("UTF-8"))
                time.sleep(1.7)
                taruhSampah(detected)
                time.sleep(1.5)
                turun = True
                Sedot = True
                Arms = False
    elif Sedot and turun and not Arms and count_tot < 5:
            kondisiMundurKeTengah()
            taruhSampah(detected)
            if tengah :
                taruhSampah(detected)
                time.sleep(1)
                detected.clear()
                time.sleep(0.5)
                Mega.write("4\n".encode("UTF-8"))
                time.sleep(0.3)
                count_tot +=1
                print(count_tot)
                Sedot = False
                Arms = False
                turun = False
                tengah = False
                
    if not  objek_terdeteksi and count_tot >= 5 and not Sedot and not tengah :
        Arm.write("0 20\n".encode("utf-8"))
        # kondisiMundur()
        stop()
    if objek_terdeteksi :
        if center_x_cm < -3 and center_x_cm < -7 and not Arms:
            Arm.write("0 20\n".encode("utf-8"))
            Motor1.write("2".encode('utf-8'))
            Motor2.write("2".encode('utf-8')) 
            Arms = False 
        elif center_x_cm <= 3  and center_x_cm >=-3 and not Sedot and not Arms and count_tot >= 5:
            detected.append(class_name)
            stop()
            Arm.write(command.encode("utf-8"))
            time.sleep(1)
            Arms = True
            if Arms and not Sedot and not turun and count_tot >= 5:
                Arm.write(command.encode("utf-8"))
                Mega.write("2\n".encode("UTF-8"))
                time.sleep(3.3)
                Mega.write("5\n".encode("UTF-8"))
                time.sleep(1.9)
                # taruhSampah(detected)
                # time.sleep(1.7)
                turun = True
                Sedot = True
                Arms = False
    elif not  objek_terdeteksi  and Sedot and turun and not Arms and count_tot >= 5:
            kondisiMajuKeTengah()
            taruhSampah(detected)
            if tengah :
                taruhSampah(detected)
                time.sleep(1)
                detected.clear()
                time.sleep(0.9)
                Mega.write("4\n".encode("UTF-8"))
                time.sleep(1)
                count_tot +=1
                print(count_tot)
                Sedot = False
                Arms = False
                turun = False
                tengah = False
            

    if cv2.waitKey(1) & 0xFF == ord("q") or not thread_kamera.is_alive():
        Mega.close()
        Arm.close()
        Motor2.close()
        Motor1.close()
        break

Mega.close()
Arm.close()
Motor2.close()
Motor1.close()
cap.release()
cv2.destroyAllWindows()
