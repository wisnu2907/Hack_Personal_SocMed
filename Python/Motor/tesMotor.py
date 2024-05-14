import serial
# import time

Motor1 = serial.Serial("COM10", 9600) #ID 10
Motor2 = serial.Serial("COM11", 9600) #ID 11

if Motor1.is_open:
    Motor1.close()
Motor1.open()

if Motor2.is_open:
    Motor2.close()
Motor2.open()

while True:
    inp =input("cmd : ")
    Motor1.write(inp.encode("utf-8"))
    Motor2.write(inp.encode("utf-8"))
