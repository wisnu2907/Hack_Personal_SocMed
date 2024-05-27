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
kanan_habis = False
kiri_habis = False

center_x_cm = 0
center_y_cm = 0
count_tot = 0
counter =0
counter2 =0

detected = []
received_array = []
class_name = ""
command = ""

# Initialize the YOLO model
model = YOLO(r"C:\Ndaru\KRTMI\programming\best.pt")
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.model.to(device)

print("Using device:", device)

cap = cv2.VideoCapture(1)
cap.set(cv2.CAP_PROP_AUTOFOCUS, 1)

# Define the colors for the bounding boxes
COLORS = (255, 255, 255)

calibration_file = "calibration_settings.npz"
if os.path.exists(calibration_file):
    settings = np.load(calibration_file)
    pts1 = settings["pts1"]
    pts2 = settings["pts2"]
    calibrated = True
    print("Calibration settings loaded.")
else:
    calibrated = False

Mega = serial.Serial("COM12", 9600)
Arm = serial.Serial("COM11", 1000000)
B = serial.Serial("COM5", 9600)  # ID 10
F = serial.Serial("COM6", 9600)  # ID 11


if B.is_open:
    B.close()
B.open()
B.flush()
# print("B is opened")

if F.is_open:
    F.close()
F.open()
F.flush
# print("F is opened")

if Arm.is_open:
    Arm.close()
Arm.open()
Arm.flush()
# print("Arm is opened")

if Mega.is_open:
    Mega.close()
Mega.open()
Mega.flush()
# print("Mega is opened")

def delay(s):
    time.sleep(s)

def stop():
    B.write("0".encode("utf-8"))
    F.write("0".encode("utf-8"))

def slideL():
    B.write("L\n".encode("utf-8"))
    F.write("L\n".encode("utf-8"))



def baca_encoder():
    global counter
    while True:
        counter = B.readline().decode().rstrip()
        print(counter)


thread_enc = threading.Thread(target=baca_encoder)
thread_enc.daemon = True
thread_enc.start()
delay(2)

print("menunggu nyala")

B.write("L\n".encode("utf-8"))
F.write("L\n".encode("utf-8"))
while True:
    # delay(0.02)
    if float(counter) >=95 :
        stop()
        break


def baca_sensor():
    global data, received_array
    while True:
        data = Mega.readline().decode().rstrip()
        received_array = list(map(int, data.split(",")))
        # print("Received array:", received_array)


def deteksi_objek():
    global  objek_terdeteksi, logitune, ret, frame, cap, class_name, detected, center_x_cm, center_y_cm, command
    while True:
        ret, frame = cap.read()
        if not ret:
            continue

        if not logitune:
            os.startfile(
                r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Logitech\LogiTune.lnk"
            )
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

            cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), COLORS, 2)

            ratio_px_cm = 45 / frame.shape[0]

            center_x_cm = ((x1 + x2) / 2 - frame.shape[1] / 2) * ratio_px_cm
            center_y_cm = (frame.shape[0] - ((y1 + y2) / 2)) * ratio_px_cm
            command = f"{center_x_cm + 6.5:.5f} {center_y_cm - 0.3:.5f}\n"

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


def taruhSampahMaju(detected):
    if detected:
        if detected[0] == "Ferro":
            Arm.write("a".encode("utf-8"))
        elif detected[0] == "Non-Ferro":
            Arm.write("b".encode("utf-8"))
        elif detected[0] in ["Plastik-Biru", "Plastik-Putih", "Botol"]:
            Arm.write("c".encode("utf-8"))
        elif detected[0] in ["Koran", "Kertas-Bungkus"]:
            Arm.write("d".encode("utf-8"))
        elif detected[0] in ["Daun-Basah", "Daun-Kering"]:
            Arm.write("e".encode("utf-8"))
            
def taruhSampahMundur(detected):
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
    global received_array, tengah, kanan_habis
    if len(received_array) < 9:
        print("Error: received_array does not have 9 elements")
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        return
    
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1):
        #111000111 maju
        B.write("a".encode('utf-8'))
        F.write("a".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111100111 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111110111 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1):
        #111110011 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111001111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110001111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #100011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #000011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #1 1 1 1 1 1 1 1 1 1
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = False
        kanan_habis= True
    else :
        B.write("a".encode('utf-8'))
        F.write("a".encode('utf-8'))
        tengah = False

def kondisiMajuKeTengah():
    global received_array, tengah  
    if len(received_array) < 9:
        print("Error: received_array does not have 9 elements")
        B.write("e".encode('utf-8'))
        F.write("e".encode('utf-8'))
        return
    
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1):
        #111000111 maju
        B.write("e".encode('utf-8'))
        F.write("e".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111100111 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111110111 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1):
        #111110011 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111001111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110001111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #100011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #000011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #0 0 0 0 0 0 0 0 0 0
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #0 0 0 1 0 0 0 0 0 0
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 0 0 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 0 0 1 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 1 0 0 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 1 0 1 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 1 1 0 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 1 1 1 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    else :
        B.write("e".encode('utf-8'))
        F.write("e".encode('utf-8'))
        
def kondisiMajuKeObjek():
    global received_array, tengah  
    if len(received_array) < 9:
        print("Error: received_array does not have 9 elements")
        B.write("e".encode('utf-8'))
        F.write("e".encode('utf-8'))
        return
    
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1):
        #111000111 maju
        B.write("e".encode('utf-8'))
        F.write("e".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111100111 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111110111 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1):
        #111110011 nganan
        B.write("c".encode('utf-8'))
        F.write("c".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111001111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110001111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #100011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #000011111 ngiri
        B.write("d".encode('utf-8'))
        F.write("d".encode('utf-8'))
        tengah = False
    else :
        B.write("e".encode('utf-8'))
        F.write("e".encode('utf-8'))


def kondisiMundur():
    global received_array, tengah, kiri_habis
    if len(received_array) < 9:
        print("Error: received_array does not have 9 elements")
        B.write("1".encode('utf-8'))
        F.write("1".encode('utf-8'))
        return
    
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1):
        #111000111 maju
        B.write("1".encode('utf-8'))
        F.write("1".encode('utf-8'))
        tengah = False

    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111100111 ngiri
        B.write("3".encode('utf-8'))
        F.write("3".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111110111 ngiri
        B.write("3".encode('utf-8'))
        F.write("3".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1):
        #111110011 ngiri
        B.write("3".encode('utf-8'))
        F.write("3".encode('utf-8'))
        tengah = False

    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111001111 nganan
        B.write("4".encode('utf-8'))
        F.write("4".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110001111 nganan
        B.write("4".encode('utf-8'))
        F.write("4".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110011111 nganan
        B.write("4".encode('utf-8'))
        F.write("4".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #100011111 nganan
        B.write("4".encode('utf-8'))
        F.write("4".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #000011111 nganan
        B.write("4".encode('utf-8'))
        F.write("4".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1 ):
        #1 1 1 1 1 1 1 1 1 1
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = False
        kiri_habis = True
    else :
        B.write("1".encode('utf-8'))
        F.write("1".encode('utf-8'))      

def kondisiMundurKeTengah():
    global received_array, tengah, tengah2, tes
    if len(received_array) < 9:
        print("Error: received_array does not have 9 elements")
        B.write("5".encode('utf-8'))
        F.write("5".encode('utf-8'))
        return
    
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1):
        #111000111 maju
        B.write("5".encode('utf-8'))
        F.write("5".encode('utf-8'))
        tengah = False

    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111100111 ngiri
        B.write("7".encode('utf-8'))
        F.write("7".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111110111 ngiri
        B.write("7".encode('utf-8'))
        F.write("7".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1):
        #111110011 ngiri
        B.write("7".encode('utf-8'))
        F.write("7".encode('utf-8'))
        tengah = False

    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111001111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110001111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110011111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #100011111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #000011111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False

    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #0 0 0 0 0 0 0 0 0 0
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 0 0 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 0 0 1 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 1 0 0 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 1 0 1 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and  
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 1 1 0 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8'))
        tengah = True
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and  
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 0 and received_array[8] == 0):
        #1 1 1 1 0 0...
        B.write("0".encode('utf-8'))
        F.write("0".encode('utf-8')) 
        tengah = True
    else :
        B.write("5".encode('utf-8'))
        F.write("5".encode('utf-8'))
        
def kondisiMundurKeObjek():
    global received_array, tengah, tengah2, tes
    if len(received_array) < 9:
        print("Error: received_array does not have 9 elements")
        B.write("5".encode('utf-8'))
        F.write("5".encode('utf-8'))
        return
    
    if (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
        received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 0 and 
        received_array[6] == 1 and received_array[7] == 1 and received_array[8] == 1):
        #111000111 maju
        B.write("5".encode('utf-8'))
        F.write("5".encode('utf-8'))
        tengah = False

    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 0 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111100111 ngiri
        B.write("7".encode('utf-8'))
        F.write("7".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111110111 ngiri
        B.write("7".encode('utf-8'))
        F.write("7".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 1 and received_array[4] == 1 and received_array[5] == 0 and 
            received_array[6] == 0   and received_array[7] == 1 and received_array[8] == 1):
        #111110011 ngiri
        B.write("7".encode('utf-8'))
        F.write("7".encode('utf-8'))
        tengah = False

    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 1 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #111001111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 0 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110001111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 1 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #110011111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 1 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #100011111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    elif (received_array[0] == 0 and received_array[1] == 0 and received_array[2] == 0 and 
            received_array[3] == 0 and received_array[4] == 1 and received_array[5] == 1 and 
            received_array[6] == 1   and received_array[7] == 1 and received_array[8] == 1):
        #000011111 nganan
        B.write("8".encode('utf-8'))
        F.write("8".encode('utf-8'))
        tengah = False
    else :
        B.write("5".encode('utf-8'))
        F.write("5".encode('utf-8'))

delay(10)
while True:
    # kondisiMajuKeObjek()
    # print("di luar cuy")
    time.sleep(0.03)
    if not objek_terdeteksi and count_tot % 2 == 0 and not Sedot and not tengah and not Arms:

        Arm.write("0 20\n".encode("utf-8")) 
        if kanan_habis:
            stop()
            turun = True
            Sedot = True
            Arms = False
            
        else:
            kondisiMaju()
    elif objek_terdeteksi and count_tot % 2 == 0  and not Sedot and not tengah and not Arms and not kanan_habis:
        if center_x_cm > 9 and not Arms:
            Arm.write("0 20\n".encode("utf-8"))
            kondisiMajuKeObjek()
            # Arms = False
        elif center_x_cm <= 9  and center_x_cm >= -13 and not Sedot and not Arms  and not turun:
            stop() 
            delay(1.3)
            detected.append(class_name)
            delay(0.1)
            Arm.write(command.encode("utf-8"))
            Arms = True
            
    elif Arms and not Sedot and not turun and count_tot % 2 == 0:
        Arm.write(command.encode("utf-8"))
        delay(1.5)
        Mega.write("2\n".encode("UTF-8"))
        delay(1.8)
        Mega.write("5\n".encode("UTF-8"))
        delay(1.3)
        taruhSampahMundur(detected)
        delay(0.1)               
        turun = True
        Sedot = True
        Arms = False
    elif Sedot and turun and not Arms and count_tot % 2 == 0 and not tengah:
        kondisiMundurKeTengah()
        # taruhSampah(detected)
    elif Sedot and turun and not Arms and count_tot % 2 == 0 and tengah:
        taruhSampahMundur(detected)
        delay(0.75)
        Mega.write("4\n".encode("UTF-8"))
        delay(0.27)
        detected.clear()

        count_tot += 1
        Sedot = False
        Arms = False
        turun = False
        tengah = False
        
    # #########################################################################################
    # #######################################BUAT MUNDUR#######################################
    # #########################################################################################
    
    elif not objek_terdeteksi and count_tot % 2 == 1 and not Sedot and not tengah and not Arms:
        Arm.write("0 20\n".encode("utf-8"))
        if kiri_habis:
            stop()
            turun = True
            Sedot = True
            Arms = False
        else:
            kondisiMundur()
            
    elif objek_terdeteksi and count_tot % 2 == 1 and not Sedot and not tengah and not Arms:
        if  center_x_cm < -9 and not Arms:
            Arm.write("0 20\n".encode("utf-8"))
            kondisiMundurKeObjek()
            # Arms = False
            
        elif center_x_cm <= 13  and center_x_cm >= -9 and not Sedot and not Arms  and not turun:
            stop() 
            delay(1.3)
            detected.append(class_name)
            delay(0.1)
            Arm.write(command.encode("utf-8"))
            Arms = True
    elif Arms and not Sedot and not turun and count_tot % 2 == 1:
        Arm.write(command.encode("utf-8"))
        delay(1.5)
        Mega.write("2\n".encode("UTF-8"))
        delay(1.8)
        Mega.write("5\n".encode("UTF-8"))
        delay(1.3)
        taruhSampahMaju(detected)
        delay(0.1)               
        turun = True
        Sedot = True
        Arms = False
    elif Sedot and turun and not Arms and count_tot % 2 == 1 and not tengah:
        kondisiMajuKeTengah()
        # taruhSampah(detected)
    elif Sedot and turun and not Arms and count_tot % 2 == 1 and tengah:
        taruhSampahMaju(detected)
        delay(0.75)
        Mega.write("4\n".encode("UTF-8"))
        delay(0.27)
        detected.clear()
        count_tot += 1
        Sedot = False
        Arms = False
        turun = False
        tengah = False
    

    if cv2.waitKey(1) & 0xFF == ord("q") or not thread_kamera.is_alive():
        Mega.close()
        Arm.close()
        F.close()
        B.close()
        break


Mega.close()
Arm.close()
F.close()
B.close()
cap.release()
cv2.destroyAllWindows()
