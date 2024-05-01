import serial

# Initialize the serial port for communication
serialInst = serial.Serial("COM8", 115200)
serialInst1 = serial.Serial("COM7", 115200)

# Close the serial port if it's already open
if serialInst.is_open:
    serialInst.close()

# Open the serial port
serialInst.open()

if serialInst1.is_open:
    serialInst1.close()

# Open the serial port
serialInst1.open()

# def maju(pwm, dist):
#     command = "{} {}\n".format(pwm, dist)
#     print(command)
#     serialInst1.write(command.encode('utf-8'))
#     serialInst.write(command.encode('utf-8'))

# maju(10, 100)
command =("1")
print(command)
serialInst1.write(command.encode("utf-8"))
serialInst.write(command.encode("utf-8"))

serialInst.close()
serialInst1.close()
