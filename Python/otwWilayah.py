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
Arm = False

detected = []
class_name = ""

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
Arm = serial.Serial("COM16", 1000000)

if Mega.is_open:
    Mega.close()
Mega.open()

if Arm.is_open:
    Arm.close()
Arm.open()

def taruhSampah(detected):
    if detected[-1] == "Ferro":
        Arm.write("1".encode("utf-8"))
    elif detected[-1] == "Non-Ferro":
        Arm.write("2".encode("utf-8"))
    elif detected[-1] in ["Plastik-Biru", "Plastik-Putih", "Botol"]:
        Arm.write("3".encode("utf-8"))
    elif detected[-1] in ["Koran", "Kertas-Bungkus"]:
        Arm.write("4".encode("utf-8"))
    elif detected[-1] in ["Daun-Fresh", "Daun-Kering"]:
        Arm.write("5".encode("utf-8"))

def baca_sensor():
    global data
    while True:
        data = Mega.readline().decode().rstrip()
        print("Data sensor:", data)

def deteksi_objek():
    global turun, objek_terdeteksi, logitune, ret, frame, cap, class_name, detected, Sedot, center_x_cm, center_y_cm, command
    
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
            if class_name not in detected or class_name == "Botol":
                detected.append(class_name)

            cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), COLORS, 2)

            ratio_px_cm = 45 / frame.shape[0]

            center_x_cm = ((x1 + x2) / 2 - frame.shape[1] / 2) * ratio_px_cm
            center_y_cm = (frame.shape[0] - ((y1 + y2) / 2)) * ratio_px_cm
            command = f"{center_x_cm + 23:.5f} {center_y_cm :.5f}\n"

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
            
        
        # elif not objek_terdeteksi and not turun and not Sedot:
        #     Arm.write("0 20\n".encode("utf-8"))
        #     Mega.write("1\n".encode("utf-8"))

        cv2.imshow("Astro_24", frame)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

thread_sensor = threading.Thread(target=baca_sensor)
thread_sensor.daemon = True
thread_sensor.start()

thread_kamera = threading.Thread(target=deteksi_objek)
thread_kamera.daemon = True
thread_kamera.start()

while True:
    if objek_terdeteksi:
        if -3 < center_x_cm < 3 and not Arm:
            Arm.write(command.encode("utf-8"))
            Arm = True
        elif center_x_cm <= -3 or center_x_cm >= 3 and not Arm:
            Arm.write("0 20\n".encode("utf-8"))
            Arm= False
    elif not objek_terdeteksi and not Arm and not turun:
        Mega.write("1\n".encode("UTF-8"))
        turun = False
        Arm.write("0 20\n".encode("utf-8"))
        Arm = False

    if Arm and not Sedot and not turun:
        Mega.write("2\n".encode("UTF-8"))
        turun = True
        time.sleep(3)
        Mega.write("5\n".encode("UTF-8"))
        Sedot = True
        time.sleep(2.6)
        taruhSampah(detected)
        time.sleep(1.2)
        Mega.write("4\n".encode("UTF-8"))
        Sedot = False
        Arm = False
        turun = False

    if cv2.waitKey(1) & 0xFF == ord("q") or not thread_kamera.is_alive():
        Mega.close()
        break

Mega.close()
cap.release()
cv2.destroyAllWindows()
