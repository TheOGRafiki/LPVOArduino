from pyb import I2C
import logging

class OLED:
  def __init__(self):
    try:
        global i2c_con = I2C(1, I2C.CONTROLLER)
    except:
      # Load Json Error Message Here
      logging.error("Random Error Message")

  def write_data(string):
    i2c_con.mem_write()

  def reset():
    i2c_con.mem_write()

