from pyb import UART
import numpy as np
from light import light

class range_finder():
  uart_obj = UART(1)
  write_command = [0xD, 0xA, 0x4F, 0x4E, 0xD, 0xA]
  stop_command = [0xD, 0xA, 0x4F, 0x46, 0x46, 0xD, 0xA]

  def __init__(self):
    try:
      light.blink(True)
      baudrate = 115200
      bits = 8
      paritiy = None
      flow = 0
      read_buf_len = 0
      self.uart_obj.init(baudrate=baudrate, bits=bits,
                         paritiy=paritiy, flow=flow, read_buf_len=read_buf_len)
    except Exception:
      LED.blink(False)


  def test_rangefinder():
    new_range_finder = range_finder()

    for i in range(10):
        print(new_range_finder.find_range())

  def find_range(self):
    for i in range(6):
      self.uart_obj.write(self.write_command[i])

    dataset = np.array()
    for i in range(10):
      np.append(dataset, self.uart_obj.read())

    # Data Analysis
    data_mean, data_std = np.mean(dataset), np.std(dataset)

    # identify outliers
    cut_off = data_std * 3
    lower, upper = data_mean - cut_off, data_mean + cut_off

    # identify outliers
    outliers = [x for x in dataset if x < lower or x > upper]
    print('Identified outliers: %d' % len(outliers))

    # remove outliers
    dataset = [x for x in dataset if x >= lower and x <= upper]
    print('Non-outlier observations: %d' % len(dataset))

    for i in range(6):
      self.uart_obj.write(self.stop_command[i])

    return np.mean(dataset)
