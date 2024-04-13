import serial


def Connect(port):
    motor = serial.Serial(port, 9600)

    if motor.is_open:
        motor.close()

    motor.open()
    return motor


def Forward(pwm, dist, motor):
    command = "{} {}\n".format(pwm, dist)
    motor.write(command.encode("utf-8"))
    while True:
        data = read_data(motor)
        if data == "Stop":
            break


def Backward(pwm, dist, motor):
    command = "-{} {}\n".format(pwm, dist)
    motor.write(command.encode("utf-8"))
    while True:
        data = read_data(motor)
        if data == "Stop":
            break


def read_data(motor):
    if motor.in_waiting > 0:
        data = motor.readline().decode().strip()
        return data
    else:
        return None


if __name__ == "__main__":
    motor = Connect("COM7")
    if motor:
        Forward(150, 10, motor)
        Backward(150, 10, motor)
