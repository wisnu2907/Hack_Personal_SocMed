import cv2
import torch
from ultralytics import YOLO

# Initialize the YOLO model
model = YOLO("Python\\CV\\best.pt")

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
cap = cv2.VideoCapture(r"Geser6.mp4")

# Define the codec and create a VideoWriter object
fourcc = cv2.VideoWriter_fourcc(*"XVID")
out = cv2.VideoWriter("output.mp4", fourcc, 20.0, (640, 480))

# Define the colors for the bounding boxes
COLORS = [(0, 255, 0), (255, 0, 0), (0, 0, 255), (255, 255, 0), (0, 255, 255)]

while True:
    # Read a frame from the webcam
    ret, frame = cap.read()

    # Resize the frame to 640x480
    frame = cv2.resize(frame, (640, 480))

    # Perform object detection on the frame
    results = model.predict(source=frame, show=False, conf=0.7)

    # Move the results to the same device as the model
    results[0].boxes = results[0].boxes.to(device)

    # Draw the detection results on the frame
    for box in results[0].boxes:
        x1, y1, x2, y2 = box.xyxy[0]
        conf = box.conf[0]
        cls = box.cls[0]
        label = f"{model.names[int(cls)]}: {conf:.2f}"
        color = COLORS[int(cls) % len(COLORS)]

        # Draw bounding box
        cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), color, 2)

        # Draw label
        cv2.putText(
            frame,
            label,
            (int(x1), int(y1) - 10),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            color,
            2,
        )

        # Calculate and display the center coordinates with (0,0) at the center of the frame
        center_x = int((x1 + x2) / 2) - frame.shape[1] // 2
        center_y = frame.shape[0] // 2 - int((y1 + y2) / 2)
        cv2.circle(
            frame,
            (int(x1 + (x2 - x1) / 2), int(y1 + (y2 - y1) / 2)),
            5,
            (0, 255, 255),
            -1,
        )
        cv2.putText(
            frame,
            f"Center: ({center_x}, {center_y})",
            (int(x1), int(y2) + 20),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.5,
            color,
            2,
        )

    # Write the frame with the detection results to the output video
    out.write(frame)

    # Display the frame
    cv2.imshow("Object Detection", frame)

    # Exit if the user presses the 'q' key
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Release the video writer, webcam, and destroy all windows
out.release()
cap.release()
cv2.destroyAllWindows()
