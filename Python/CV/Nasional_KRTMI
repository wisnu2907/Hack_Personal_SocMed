import os
import threading
import serial
import cv2
import torch
import time
from ultralytics import YOLO
import numpy as np

# Global variables
camera_running = False
objek_terdeteksi = False
logitune = False
Arms = False
Terambil = False
detected = []
class_name = ""
center_x_cm = 0
center_y_cm = 0
COLORS = (255, 255, 255)

# Serial communication setup
Mega = serial.Serial("COM18", 9600)  # Changed to a more common baud rate
if Mega.is_open:
    Mega.close()
Mega.open()
Mega.flush()

# Model setup
model = YOLO(r"C:\Users\NDARU\Downloads\rev4.pt")
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.model.to(device)
print("Using device:", device)

calibration_file = "calibration_settings.npz"
if os.path.exists(calibration_file):
    settings = np.load(calibration_file)
    pts1 = settings["pts1"]
    pts2 = settings["pts2"]
    calibrated = True
    print("Calibration settings loaded.")
else:
    calibrated = False

tempat_sampah = {
    "Ferro": 1,
    "Non-Ferro": 2,
    "Botol": 3,
    "Plastik-Biru": 3,
    "Plastik-Putih": 3,
    "Kertas-Bungkus": 4,
    "Koran": 4,
    "Daun-Kering": 5,
    "Daun-Basah": 5,
}

def pack(data):
    delimiter = '*'
    data = delimiter.join(map(str, [item for sublist in data for item in sublist]))
    data += '\n'
    return data

# Object detection function
def deteksi_objek():
    global objek_terdeteksi, logitune, class_name, detected, center_x_cm, center_y_cm, middle_x_start, middle_y_start,  middle_x_end, last_id_tempat_sampah, area_fulfillment_y, area_fulfillment_x
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_AUTOFOCUS, 1)
    last_id_tempat_sampah = 0

    while True:
        time.sleep(0.02)
        ret, frame = cap.read()
        if not ret:
            continue

        if not logitune:
            os.startfile(r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Logitech\LogiTune.lnk")
            logitune = True

        if calibrated:
        # Resize the frame based on the calibration points
            width = int(
                np.sqrt((pts1[1][0] - pts1[0][0]) ** 2 + (pts1[1][1] - pts1[0][1]) ** 2)
            )
            height = int(
                np.sqrt((pts1[2][0] - pts1[1][0]) ** 2 + (pts1[2][1] - pts1[1][1]) ** 2)
            )
            matrix = cv2.getPerspectiveTransform(pts1, pts2)
            frame = cv2.warpPerspective(frame, matrix, (width, height))

        results = model.predict(source=frame, show=False, conf=0.75)
        results[0].boxes = results[0].boxes.to(device)

        objek_terdeteksi = False
        collected_data = []

        # frame_height=frame.shape[0]
        frame_width = frame.shape[1]
        middle_x_start = frame_width * 0.1
        middle_x_end = frame_width * 0.9
        # middle_y_start = frame.shape[0] * 0.15
        middle_y_start = frame.shape[0] * 0.85
        # middle_y_end = frame_height * 0.7

        for box in results[0].boxes:
            objek_terdeteksi = True
            x1, y1, x2, y2 = box.xyxy[0]
            cls = box.cls[0]
            class_name = model.names[int(cls)]

            center_x = (x1 + x2) / 2
            center_y = (y1 + y2) / 2

            center_x_cm = ((x1 + x2) / 2 - frame.shape[1] / 2) * (40 / frame.shape[0])
            center_y_cm = (frame.shape[0] - (y1 + y2) / 2) * (40 / frame.shape[0])

            # Cek apakah objek memenuhi 90% dari area yang ditentukan
            object_height = y2 - y1
            object_width = x2 - x1
            area_fulfillment_x_from_left = (center_x - x1) / object_width
            area_fulfillment_x_from_right = (x2 - center_x) / object_width
            area_fulfillment_y = (center_y - y1) / object_height

            if class_name in tempat_sampah:
                    # print("Kondisi normal")
                    id_tempat_sampah = tempat_sampah[class_name]
                    # last_id_tempat_sampah = id_tempat_sampah
                    collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm])
            
            # if middle_x_start <= center_x <= middle_x_end and middle_y_start <= center_y:
            # if middle_y_start >= center_y:
            #     print("AMAN Y START")
            #     if class_name in tempat_sampah:
            #         # print("Kondisi normal")
            #         id_tempat_sampah = tempat_sampah[class_name]
            #         # last_id_tempat_sampah = id_tempat_sampah
            #         collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm])
            # elif middle_x_end >= center_x:
            #     print("AMAN X END")
            #     if class_name in tempat_sampah:
            #         # print("Kondisi normal")
            #         id_tempat_sampah = tempat_sampah[class_name]
            #         # last_id_tempat_sampah = id_tempat_sampah
            #         collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm])
            # elif middle_x_start <= center_x:
            #     print("AMAN X START")
            #     if class_name in tempat_sampah:
            #         # print("Kondisi normal")
            #         id_tempat_sampah = tempat_sampah[class_name]
            #         # last_id_tempat_sampah = id_tempat_sampah
            #         collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm])
            #         # if area_fulfillment_y >= 0.9:
            #         #     print("area_fulfillment_y AMAN")
            #         #     collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm])
            #         # elif area_fulfillment_y < 0.9:
            #         #     print("area_fulfillment_y GA AMAN")
            #         #     center_y_cm_corrected = center_y_cm - 3
            #         #     collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm_corrected])
            #         # elif area_fulfillment_x_from_left >= 0.9:
            #         #     print("area_fulfillment_x_from_left AMAN")
            #         #     collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm])
            #         # elif area_fulfillment_x_from_left < 0.9:
            #         #     print("area_fulfillment_x_from_left GA AMAN")
            #         #     center_x_cm_corrected = center_x_cm - 3
            #         #     collected_data.append([id_tempat_sampah, center_x_cm, center_x_cm_corrected])
            #         # elif area_fulfillment_x_from_right >= 0.9:
            #         #     print("area_fulfillment_x_from_right AMAN")
            #         #     collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm])
            #         # elif area_fulfillment_x_from_right < 0.9:
            #         #     print("area_fulfillment_x_from_right GA AMAN")
            #         #     center_x_cm_corrected = center_x_cm + 6
            #         #     collected_data.append([id_tempat_sampah, center_x_cm, center_x_cm_corrected])
            #         # else:
            #         #     center_y_cm_corrected = center_y_cm - 2
            #         #     collected_data.append([id_tempat_sampah, center_x_cm, center_y_cm_corrected])
            # # else:
            # #     print("DI LUAR BATAS BAWAH Y")
                
            # last_id_tempat_sampah = 0
                    
                # else:
                #     collected_data.append([last_id_tempat_sampah, center_x_cm, center_y_cm])

            cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), COLORS, 2)

        while len(collected_data)<5:
            collected_data.append([0,0,0])
            # last_id_tempat_sampah = 0
        packed_data = pack(collected_data)
        Mega.write(packed_data.encode('utf-8'))

        cv2.imshow("Frame", frame)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    cap.release()
    cv2.destroyAllWindows()

# Start threads for object detection
thread_kamera = threading.Thread(target=deteksi_objek)
thread_kamera.daemon = True
thread_kamera.start()

# Main thread waits for threads to complete
thread_kamera.join()
