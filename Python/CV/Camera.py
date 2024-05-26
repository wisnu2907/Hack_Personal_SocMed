import os

import cv2
import numpy as np
import torch
from ultralytics import YOLO

logitune=True

# Initialize the YOLO model
model = YOLO(r"C:\Users\wisnu\Coding\KRTMI2024\Python\best.pt")
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.model.to(device)

# setting device on GPU if available, else CPU)
print("Using device:", device)
print()

cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)  # Set resolution to 640x480 for faster processing
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
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

while True:
    # Read a frame from the webcam
    ret, frame = cap.read()
    
    if logitune:
        os.startfile(r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Logitech\LogiTune.lnk")
        logitune=False

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
    results = model.predict(source=frame, show=False, conf=0.8)
    results[0].boxes = results[0].boxes.to(device)

    # Draw the detection results on the frame
    for box in results[0].boxes:
        x1, y1, x2, y2 = box.xyxy[0]
        conf = box.conf[0]
        cls = box.cls[0]
        label = f"{model.names[int(cls)]}: {conf:.3f}"

        # Draw bounding box
        cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), COLORS, 2)
        
        # Ratio pixel to cm
        ratio_px_cm = 45 / frame.shape[0]

        # Calculate and display the center coordinates in centimeters
        center_x_cm = ((x1 + x2) / 2 - frame.shape[1] / 2) * ratio_px_cm
        center_y_cm = (frame.shape[0] - ((y1 + y2) / 2)) * ratio_px_cm


        # Draw labelq
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

        # Draw center coordinates in centimeters
        cv2.putText(
            frame,
            f"Center (cm): ({center_x_cm:.3f} cm, {center_y_cm:.3f} cm)",
            (10, 50),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            COLORS,
            2,
        )

    # Display the frame
    cv2.imshow("Astro_24", frame)

    # Exit if the user presses the 'q' keyP
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Release the webcam and destroy all windows
cap.release()
cv2.destroyAllWindows()
