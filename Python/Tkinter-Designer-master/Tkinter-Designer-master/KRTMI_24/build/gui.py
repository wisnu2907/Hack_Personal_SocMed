from pathlib import Path
from tkinter import Tk, Canvas, Button, PhotoImage
import cv2
from PIL import Image, ImageTk

# Path setup
OUTPUT_PATH = Path(__file__).parent
ASSETS_PATH = OUTPUT_PATH / Path(r"C:\Users\HP\Downloads\Tkinter-Designer-master\Tkinter-Designer-master\KRTMI_24\build\assets\frame0")

def relative_to_assets(path: str) -> Path:
    return ASSETS_PATH / Path(path)

# Global flag for camera state and capture object
camera_running = False
cap = None

# Fungsi untuk membuka kamera
def open_camera():
    global camera_running, cap
    if not camera_running:
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            print("Error: Kamera tidak bisa dibuka.")
            return
        camera_running = True

    def update_frame():
        if camera_running:
            ret, frame = cap.read()
            if not ret:
                print("Error: Tidak dapat menerima frame (stream end?).")
                cap.release()
                return

            # Convert frame to ImageTk format
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            img = Image.fromarray(frame)
            imgtk = ImageTk.PhotoImage(image=img)

            # Update image_1 on canvas
            canvas.itemconfig(image_1, image=imgtk)
            canvas.image = imgtk  # Keep a reference to avoid garbage collection

            # Continue updating the frame
            window.after(10, update_frame)

    update_frame()

# Fungsi untuk menghentikan program
def stop_program():
    global camera_running
    camera_running = False
    if cap and cap.isOpened():
        cap.release()
    window.destroy()

# Fungsi untuk retry
def retry_program():
    if camera_running:
       open_camera()

# UI setup
window = Tk()
window.geometry("1280x720")
window.configure(bg = "#FFFFFF")

canvas = Canvas(
    window,
    bg = "#FFFFFF",
    height = 720,
    width = 1280,
    bd = 0,
    highlightthickness = 0,
    relief = "ridge"
)

canvas.place(x = 0, y = 0)

button_image_1 = PhotoImage(
    file=relative_to_assets("button_1.png"))
button_1 = Button(
    image=button_image_1,
    borderwidth=0,
    highlightthickness=0,
    command=open_camera,
    relief="flat"
)
button_1.place(
    x=18.0,
    y=561.0,
    width=258.2608642578125,
    height=125.0
)

button_image_2 = PhotoImage(
    file=relative_to_assets("button_2.png"))
button_2 = Button(
    image=button_image_2,
    borderwidth=0,
    highlightthickness=0,
    command=retry_program,
    relief="flat"
)
button_2.place(
    x=348.0,
    y=556.0,
    width=195.0,
    height=125.0
)

button_image_3 = PhotoImage(
    file=relative_to_assets("button_3.png"))
button_3 = Button(
    image=button_image_3,
    borderwidth=0,
    highlightthickness=0,
    command=stop_program,
    relief="flat"
)
button_3.place(
    x=675.0,
    y=556.0,
    width=195.0,
    height=125.0
)

button_image_4 = PhotoImage(
    file=relative_to_assets("button_4.png"))
button_4 = Button(
    image=button_image_4,
    borderwidth=0,
    highlightthickness=0,
    command=lambda: print("button_4 clicked"),
    relief="flat"
)
button_4.place(
    x=990.0,
    y=555.0,
    width=195.0,
    height=125.0
)

image_image_1 = PhotoImage(
    file=relative_to_assets("image_1.png"))
image_1 = canvas.create_image(
    655.0,
    319.0,
    image=image_image_1
)

canvas.create_rectangle(
    87.0,
    0.0,
    1454.0,
    85.0,
    fill="#D9D9D9",
    outline="")

canvas.create_text(
    39.0,
    12.0,
    anchor="nw",
    text="Astro",
    fill="#000000",
    font=("Inter", 64 * -1)
)

window.resizable(False, False)
window.mainloop()
