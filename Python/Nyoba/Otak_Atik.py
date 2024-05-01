import os

import cv2
import numpy as np
import serial.tools.list_ports
import torch
# from serial import Serial
from ultralytics import YOLO
from sort import *

serialInst = serial.Serial("COM6", 1000000)

# # Check if the serial port is already open and close it if necessary
if serialInst.is_open:
    serialInst.close()

serialInst.open()

# Initialize the YOLO modelq
model = YOLO(r"C:\Users\wisnu\Coding\KRTMI2024\Python\CV\best.pt")
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.model.to(device)

# setting device on GPU if available, else CPU)
print("Using device:", device)
print()

# Additional Info when using cuda
if device.type == "cuda":
    print(torch.cuda.get_device_name(0))
    print("Memory Usage:")
    print("Allocated:", round(torch.cuda.memory_allocated(0) / 1024**3, 1), "GB")
    print("Cached:   ", round(torch.cuda.memory_reserved(0) / 1024**3, 1), "GB")

# Open the webcam
cap = cv2.VideoCapture(0)

# Set autofocus
cap.set(cv2.CAP_PROP_AUTOFOCUS, 1)

# Define the colors for the bounding boxes
COLORS = [(0, 255, 0), (255, 0, 0), (0, 0, 255), (255, 255, 0), (0, 255, 255)]

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

tracker = Sort(max_age=20, min_hits=3, iou_threshold=0.3)

ids=[]
obj=np.empty((0,5))

while True:
    # Read a frame from the webcam
    detections = np.empty((0,5))
    ret, frame = cap.read()

    # Check if the frame is valid
    if not ret or frame is None:
        print("Error: Failed to read frame from the webcam")
        break

    # Calibration
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

    # Perform object detection on the frame
    results = model.predict(source=frame, show=False, conf=0.5)
    results[0].boxes = results[0].boxes.to(device)

    # 1 cm = 22.4 pixels
    # ratio pixel to cm
    ratio_px_cm = 194 / 100

    # Draw the detection results on the frame
    for box in results[0].boxes:
        x1, y1, x2, y2 = box.xyxy[0]
        conf = box.conf[0]
        cls = box.cls[0]
        label = f"{model.names[int(cls)]}: {conf:.2f}"
        color = COLORS[int(cls) % len(COLORS)]

        currentArray = np.array([x1.cpu().item(), y1.cpu().item(), x2.cpu().item(), y2.cpu().item(), conf.cpu().item()])

        detections = np.vstack((detections, currentArray))

        # Draw bounding box
        cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), color, 2)

        # Calculate and display the center coordinates in centimeters
        # center_x_cm = (
        #     (int((x1 + x2) / 2) - frame.shape[1] // 2) / ratio_px_cm / 10
        # )  # divide by 10 to convert mm to cm
        # center_y_cm = y1 / ratio_px_cm / 10  # divide by 10 to convert mm to cm

        # command = f"{center_x_cm:.5f} {center_y_cm:.5f}\n"
        # serialInst.write(command.encode("utf-8"))

        # cv2.circle(frame, (int(center_x_cm), int(center_y_cm)), 5, (0, 255, 255), -1)

        # Draw label
        label_size, _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 2)
        cv2.putText(
            frame,
            label,
            (int(x1), int(y2) + label_size[1]),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            (255, 255, 255),
            2,
        )
        cv2.putText(
            frame,
            label,
            (int(x1), int(y1) - 40),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            (255, 255, 255),
            2,
        )

        # Draw center coordinates in centimeters
        # cv2.putText(
        #     frame,
        #     f"Center (cm): ({center_x_cm:.2f} cm, {center_y_cm:.2f} cm)",
        #     (int(x1), int(y2) + label_size[1] + 20),
        #     cv2.FONT_HERSHEY_SIMPLEX,
        #     0.5,
        #     (255, 255, 255),
        #     2,
        # )
        # cv2.putText(
        #     frame,
        #     f"Center (cm): ({center_x_cm:.2f} cm, {center_y_cm:.2f} cm)",
        #     (int(x1), int(y1) - 10),
        #     cv2.FONT_HERSHEY_SIMPLEX,
        #     0.5,
        #     (255, 255, 255),
        #     2,
        # )
    
    # Sort the detections array based on x1 from lower to higher
    sorted_detections = detections[detections[:, 0].argsort()]

    # Update results with the sorted detections
    results = tracker.update(sorted_detections)

    min_id = float('inf')
    min_track = None
    for track in results:
        x1, y1, x2, y2, id = map(int, track[:])
        if id < min_id:
            min_id = id
            min_track = track
            cv2.putText(frame, f"Id: {id}",(x1,y1+10), cv2.FONT_HERSHEY_SIMPLEX, fontScale=0.5,color=(255,255,255), thickness=2)

    if min_track is not None:
        x1, y1, x2, y2, id = map(int, min_track[:])
        center_x_cm = (
        (frame.shape[1] // 2 - int((x1 + x2) / 2)) / ratio_px_cm / 10
        )  # divide by 10 to convert mm to cm
        center_y_cm = y1 / ratio_px_cm / 10  # divide by 10 to convert mm to cm
        command = f"{center_x_cm + 25 + 31:.5f} {center_y_cm + 20 - 12:.5f}\n"
        serialInst.write(command.encode("utf-8"))
    else:
        command = "3 13\n"

# Display the frame
    cv2.imshow("Astro_24", frame)

    # Exit if the user presses the 'q' key
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Release the webcam, close the serial port, and destroy all windows
cap.release()
serialInst.close()
cv2.destroyAllWindows()
