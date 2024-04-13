import cv2
import numpy as np
import torch
from ultralytics import YOLO

# Initialize the YOLO model
model = YOLO("Python\\CV\\best.pt")
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.model.to(device)

# Open the webcam
cap = cv2.VideoCapture(0)

# File name for saving calibration settings
settings_file = "calibration_settings.npz"

# Initialize variables for calibration
clicked_points = []
calibrated = False


# Mouse callback function to capture points for calibration
def mouse_callback(event, x, y, flags, param):
    global clicked_points, calibrated

    if event == cv2.EVENT_LBUTTONDOWN:
        clicked_points.append((x, y))

        if len(clicked_points) == 4:
            calibrated = True


# Set mouse callback
cv2.namedWindow("Object Detection")
cv2.setMouseCallback("Object Detection", mouse_callback)

while True:
    # Read a frame from the webcam
    ret, frame = cap.read()

    # Calibration
    if calibrated:
        # Resize the frame based on the calibration points
        width = int(
            np.linalg.norm(np.array(clicked_points[1]) - np.array(clicked_points[0]))
        )
        height = int(
            np.linalg.norm(np.array(clicked_points[2]) - np.array(clicked_points[1]))
        )

        pts1 = np.float32(clicked_points)
        pts2 = np.float32([[0, height], [width, height], [width, 0], [0, 0]])

        matrix = cv2.getPerspectiveTransform(pts1, pts2)
        frame = cv2.warpPerspective(frame, matrix, (width, height))

    # Display the frame
    cv2.imshow("Object Detection", frame)

    # Exit if the user presses the 'q' key
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Release the webcam and destroy all windows
cap.release()
cv2.destroyAllWindows()

# Save calibration settings if calibration is done
if calibrated:
    np.savez(settings_file, pts1=pts1, pts2=pts2)
    print("Calibration settings saved.")
