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
sedot = False
Arms = False

detected = []
class_name = ""

center_x_cm = 0
center_y_cm = 0
command = ""

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

Mega = serial.Serial("COM5", 9600)
Arm = serial.Serial("COM2", 1000000)

if Mega.is_open:
    Mega.close()
Mega.open()

if Arm.is_open:
    Arm.close()
Arm.open()

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


def baca_sensor():
    global data
    while True:
        data = Mega.readline().decode().rstrip()

def deteksi_objek():
    global objek_terdeteksi, logitune, ret, frame, cap, class_name, detected, center_y_cm, center_x_cm, command
    
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

        results = model.predict(source=frame, show=False, conf=0.85)
        results[0].boxes = results[0].boxes.to(device)

        objek_terdeteksi = False
        for box in results[0].boxes:
            x1, y1, x2, y2 = box.xyxy[0]
            conf = box.conf[0]
            cls = box.cls[0]
            class_name = model.names[int(cls)]
            label = f"{class_name}: {conf:.3f}"
            objek_terdeteksi = True
            
            cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), COLORS, 2)

            ratio_px_cm = 45 / frame.shape[0]

            center_x_cm = ((x1 + x2) / 2 - frame.shape[1] / 2) * ratio_px_cm
            center_y_cm = (frame.shape[0] - ((y1 + y2) / 2)) * ratio_px_cm
            command = f"{center_x_cm + 19:.5f} {center_y_cm :.5f}\n"

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

thread_sensor = threading.Thread(target=baca_sensor)
thread_sensor.daemon = True
thread_sensor.start()

thread_kamera = threading.Thread(target=deteksi_objek)
thread_kamera.daemon = True
thread_kamera.start()

while True:
    if objek_terdeteksi:
        if center_x_cm >= -3 and center_x_cm <=3 and not turun and not Arms:           
            detected.append(class_name)
            Arm.write(command.encode("utf-8"))
            Arms = True
            # turun = True
        elif center_x_cm < -3 or center_x_cm > 3 and not turun and not Arms:
            Arm.write("0 20".encode("utf-8"))
            # Mega.write("1".encode("UTF-8"))
            turun = False
            Arms= False
    if not turun and not sedot and Arms:
        Arm.write(command.encode("utf-8"))
        time.sleep(1)
        Mega.write("2\n".encode("UTF-8"))
        turun = True
        time.sleep(2.3)
        Mega.write("5\n".encode("UTF-8"))
        time.sleep(2)
        taruhSampahMaju(detected)
        time.sleep(1)
        Mega.write("4\n".encode("UTF-8"))
        detected.clear()
        sedot = False
        turun = False
        Arms = False

    if cv2.waitKey(1) & 0xFF == ord("q") or not thread_kamera.is_alive():
        Mega.close()
        break

Mega.close()
Arm.close()
cap.release()
cv2.destroyAllWindows()