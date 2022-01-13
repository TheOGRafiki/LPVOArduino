from pyb import LED
from time import sleep
import micropython
from pyb import I2C

def blink(status):

  leb_obj = "Empty"

  if(status == True):
    led_obj = pyb.LED(2) # Green Led
  else:
    led_obj = pyb.LED(1) # Red Led

  for x in range(3):
    leb_obj.on()
    sleep(1)
    leb_obj.off()
    sleep(1)


class dislay(self):
    i2c_obj = I2C(X)
    devices = "Empty"

    def __init__(self):
      blink(True)
      try:
        i2c_obj.init(I2C.CONTROLLER, baudrate="9600")
        devices = i2c_obj.scan()
        print(devices)

      except Exception:
        blink(False)

    def write_command(text):
      self.i2c_obj.send(0x3C)
      self.i2c_obj.send(0x00)
      self.i2c_obj.send(text)

    def write_text(data):
      self.i2c_obj.write(0x3C)
      self.i2c_obj.write(0xC0)
      self.i2c_obj.write(data)

    def set_column_address(a, b):

    def set_page_address(a, b):

    def fill_ram(data):


class range_finder():
    def __init__(self, spi_obj):
        self.spi_obj = spi_obj
        spi_obj.init()


def __main__():
    # main loop goes here

