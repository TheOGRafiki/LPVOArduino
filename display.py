from pyb import I2C
from light import LED

class display(self):
    i2c_obj = I2C(X)
    devices = "Empty"

    def __init__(self):
      LED.blink(True)
      try:
        i2c_obj.init(I2C.CONTROLLER, baudrate="9600")
        devices = i2c_obj.scan()

        print(devices)
        pyb.delay(100)

      except Exception:
       LED.blink(False)

    def write_command(text):
      self.i2c_obj.send(0x3C)
      pyb.delay(100)

      self.i2c_obj.send(0x00)
      pyb.delay(100)

      self.i2c_obj.send(text)
      pyb.delay(100)

    def write_text(data):
      self.i2c_obj.send(0x3C)
      pyb.delay(100)

      self.i2c_obj.send(0xC0)
      pyb.delay(100)

      self.i2c_obj.send(data)
      pyb.delay(100)

    def set_column_address(a, b):
      self.i2c_obj.write_command(0x21)
      pyb.delay(100)

      self.i2c_obj.write_command(a)
      pyb.delay(100)

      self.i2c_obj.write_command(b)
      pyb.delay(100)

    def set_page_address(a, b):
      self.i2c_obj.write_command(0x21)
      pyb.delay(100)

      self.i2c_obj.write_command(a)
      pyb.delay(100)

      self.i2c_obj.write_command(b)
      pyb.delay(100)

    def fill_ram(data):
      for i in range(8):
        set_column_address(0x00)

        for j in range(128):
          write_data(data)


    def test_display():
        new_display = display()

        new_display.write_text("Hello World")
        new_display.fill_ram(0x00)
        delay(3000)

        new_display.write_text("123456789")
        new_display.fill_ram(0x00)
        delay(3000)

        new_display.write_text("!@#$%^&*()")
        new_display.fill_ram(0x00)
        delay(3000)

        new_display.write_text("It works mother fuckers")
        new_display.fill_ram(0x00)
        delay(3000)
