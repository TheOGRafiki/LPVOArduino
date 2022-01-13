from pyb import delay
from pyb import LED

class light():

    def __init__(self) -> None:
        pass

    def blink(status):
        leb_obj = "Empty"

        if(status == True):
            led_obj = LED(2)  # Green Led
        else:
            led_obj = LED(1)  # Red Led

        for x in range(3):
            leb_obj.on()
            delay(1000)
            leb_obj.off()
            delay(1000)

    def test():
        green_led = LED(2)
        red_led = LED(1)
        blue_led = LED(3)

        def blink_fast(led):
            for i in range(3):
                led.on()
                delay(500)
                led.off()
                delay(500)
        
        def circus():
            for i in range(10):
                green_led.on()
                delay(200)
                green_led.off()
                red_led.on()
                delay(200)
                red_led.off()
                blue_led.on()
                delay(200)
                
        blink_fast(green_led)
        blink_fast(red_led)
        blink_fast(blue_led)
        circus()