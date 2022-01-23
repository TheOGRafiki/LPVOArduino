#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <SoftwareSerial.h>

/* Constructor */
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 8, /* dc=*/ A0, /* reset=*/ 9);
SoftwareSerial Rangefinder(/*RX*/3, /*TX*/4);

const int buttonPin = 2;
int buttonState = 0;
/* u8x8.begin() is required and will sent the setup/init sequence to the display */
void setup(void)
{
  u8x8.begin();

  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  
  Rangefinder.begin(115200);
}

void loop(void)
  {
    buttonState = digitalRead(buttonPin);
    const unsigned char buf[] = {0xD, 0xA, 0x4F, 0x4E, 0xD, 0xA};
    const unsigned char buf2[] = {0xD, 0xA, 0x4F, 0x46, 0x46, 0xD, 0xA};

     if(buttonState != HIGH) {
      Rangefinder.write(buf, 6);
      u8x8.drawString(1, 3, "Range Finder Active");

      Serial.print("Data Read: "); 
      for(int i = 0; i < 7; i++) {
        Serial.print((char)Rangefinder.read());
        Serial.print(" ");
        delay(300);
      }
      Serial.println(); 
      delay(500);
     }

      u8x8.clearDisplay();
      Rangefinder.write(buf2, 7);
      return; 
  }
