#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>

/* Constructor */
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 8, /* dc=*/ A0, /* reset=*/ 9);

const int buttonPin = 2;
int buttonState = 0;
/* u8x8.begin() is required and will sent the setup/init sequence to the display */
void setup(void)
{
  u8x8.begin();

  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  

}

void loop(void)
  {
    buttonState = digitalRead(buttonPin);
  if(buttonState != HIGH) {
    u8x8.drawString(1,3,"Button Pressed");
    delay(200);

  }
  else {
      u8x8.clearDisplay();
  }

  delay(1000); 
}
