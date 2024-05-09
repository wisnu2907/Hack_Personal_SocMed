import math
toRADS = 0.0174533

DEGA = 45  # Right Front
DEGB = 135  # Right Rear
DEGC = 225  # Left Rear
DEGD = 315 # Left Front
w = 0   
Turn = 0
radius = 10

def drive_mecanumOjan(sudut):
    # void gerak(float radius, float sudut, int w) {
#   //rumus baru, sudut dalam radian
    if sudut < 0: sudut += 360
    elif sudut >= 360: sudut -= 360

    m1 = radius * math.sin(sudut + 2.356) + w * 0.7;  #depankanan
    m2 = radius * math.sin(sudut + 0.785) - w * 0.7;  #depankiri
    m3 = radius * math.sin(sudut + 0.785) + w * 0.7;  #belakangkanan
    m4 = radius * math.sin(sudut + 2.356) - w * 0.7;  #belakangkiri

    print("Kanan depan =", m1)
    print("Kanan belakang =", m2)
    print("Kiri depan =", m3)
    print("Kiri belakang =", m4)
#   // Serial.print("M1 : "); Serial.println(m1);
    
def drive_mecanum(Dir):
    wa = (math.cos((DEGA + Dir) * toRADS) * 255) - Turn
    wc = (math.cos((DEGC + Dir) * toRADS) * 255) - Turn
    wb = (math.cos((DEGB + Dir) * toRADS) * 255) + Turn
    wd = (math.cos((DEGD + Dir) * toRADS) * 255) + Turn
    print("Kanan depan =", wa)
    print("Kanan belakang =", wc)
    print("Kiri depan =", wb)
    print("Kiri belakang =", wd)
    

while True:
    try:
        input_deg = float(input("Input Deg : "))
        drive_mecanumOjan(input_deg*toRADS)
    except ValueError:
        print("Invalid input. Please enter a valid number.")
