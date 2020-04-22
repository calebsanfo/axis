import serial


class Axis:

    def __init__(self, COM_port):
        self.connection = serial.Serial(COM_port, 9600, timeout=5) 

    def get_x_pos(self):
        message = "get x".encode()
        self.connection.write(message)
        return float(self.connection.readline())

    def get_y_pos(self):
        message = "get y".encode()
        self.connection.write(message)
        return float(self.connection.readline())

    def get_z_pos(self):
        message = "get z".encode()
        self.connection.write(message)
        return float(self.connection.readline())

    def set_xy_pos(self, x, y):
        self.connection.write(("moveXY "+str(x)+" "+str(y)).encode())
        return self.connection.readline()

    def set_z_pos(self, z):
        self.connection.write(("moveZ "+str(z)).encode())
        return self.connection.readline()

    def get_analog_input(self):
        self.connection.write("analogread".encode())
        return float(self.connection.readline())