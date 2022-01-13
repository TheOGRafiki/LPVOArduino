import logging
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


class dislay(self, i2c_obj):
    i2c_obj = I2C(1)

    def __init__(self, i2c_obj):

      self.i2c_obj = i2c_obj
      i2c_obj.init(I2C.CONTROLLER, baudrate="9600")

      pyb.delay(100)
      print("Running it down")
      pyb.delay(120)

      write_command(0xFD)
      write_command(0X12)
      write_command(0XAE)
      write_command(0xD5)
      write_command(0X30)
      write_command(0xA8)
      write_command(0X37)
      write_command(0xD3)
      write_command(0X08)
      write_command(0x40)
      write_command(0xA1)
      write_command(0xC8)
      write_command(0xDA)
      write_command(0x12)
      write_command(0x81)
      write_command(0x12)
      write_command(0x8F)
      write_command(0xD9)
      write_command(0x25)
      write_command(0xDB)
      write_command(0x34)
      write_command(0xA4)
      write_command(0xA6)









    def write_command(i2c_obj, text):
      i2c_obj.send(0x3C)
      i2c_obj.send(0x00)
      i2c_obj.send(text)

    def write_text(i2c_obj. data):
      i2c_obj.write(0x3C)
      i2c_obj.write(0xC0)
      i2c_obj.write(data)






