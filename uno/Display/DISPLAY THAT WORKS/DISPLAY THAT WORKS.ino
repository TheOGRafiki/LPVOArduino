#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <SoftwareSerial.h>

/* Pin Declaration */ 
const int buttonPin = 2;
const int rangefinderTx = 4;
const int rangefinderRx = 3;

/* Constructor */
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 8, /* dc=*/ A0, /* reset=*/ 9);
SoftwareSerial Rangefinder(/*RX*/rangefinderRx, /*TX*/rangefinderTx);

/* Button Debounce Paramas*/
volatile long lastDebounceTime = 0;
volatile long debounceDelay = 50;

/* States of button and display */
volatile char* textDisplay;
volatile bool buttonState = false; /* False = Not Pressed  True = Pressed*/


// JSON Objects

M4_CARBINE = {}


void setup(void)
{
  u8x8.begin();

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), range, RISING);

  Serial.begin(115200);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  
  Rangefinder.begin(115200);
}

void loop(void) { 
  u8x8.drawString(1, 3, textDisplay);
  delay(1000);
}

void range() { 
  if((millis() - lastDebounceTime) > debounceDelay) {
      buttonState = !buttonState; 
      const unsigned char startCommand[] = {0xD, 0xA, 0x4F, 0x4E, 0xD, 0xA};
      const unsigned char stopCommand[] = {0xD, 0xA, 0x4F, 0x46, 0x46, 0xD, 0xA};

      /* Start Command*/
      Rangefinder.write(startCommand, 6);
      u8x8.drawString(1, 3, "Range Finder Active");

      Serial.print("Data Read: "); 

      for(int i = 0; i < 7; i++) {
        Serial.print((char)Rangefinder.read());
        textDisplay[i] = (char)Rangefinder.read();
      }
      Serial.println(); 

      // u8x8.drawString()
      Rangefinder.write(stopCommand, 7);

      lastDebounceTime = millis();
    }
  }

  void calcMOA() {
      // Used to determine Angle
      // Bullet Drop in Inches / MOA Inches at distance = MOA Adjustment needed 
      // 1 MOA = 1/60TH Of a degree
      // 800 Yards => 8 INCHES MOA 
  }

  void calcFallOff() {

    // Used to determine fall off function
    // Use Pejsa 
  }