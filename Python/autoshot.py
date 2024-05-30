import cv2
import os
import time

# Directory path to save the images
output_dir = r"C:\Users\wisnu\OneDrive\Pictures\Database"

# Open the default camera
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
first=True

# Check if the camera opened successfully
if not cap.isOpened():
    print("Error: Unable to open the camera.")
    exit()

# Capture 30 images
for i in range(1, 11):
    # Read a frame from the camera
    ret, frame = cap.read()
    if first:
        print("buka logitune blok")
        time.sleep(7)
        first=False
    

    # Check if the frame was read successfully
    if not ret:
        print("Error: Unable to capture frame.")
        break

    # Save the captured image with a unique filename
    image_path = os.path.join(output_dir, f"Daun-Basah_{i}.jpg")
    cv2.imwrite(image_path, frame)
    print(f"Image {i} captured and saved at {image_path}.")

    # Display the captured image
    cv2.imshow("Captured Image", frame)
    cv2.waitKey(1000)  # Display image for 1 second

    # Delay for 3 seconds before capturing the next image
    if i%5 ==0 and i !=10:
        print(f"Ganti posisi {i}")
        time.sleep(4)
    
# Release the camera and close all OpenCV windows
cap.release()
cv2.destroyAllWindows()
