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
volatile bool buttonState = false; /* False = Not Pressed  True = Pressed*/

void setup(void) {
  u8x8.begin();

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), range, RISING);

  Serial.begin(115200);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  
  Rangefinder.begin(115200);

  // Loading Screen
  displayStart();

  // Test Components 
  displayLoad();
}

void loop(void) { 
}

void displayStart() {
  for(int i = 0; i < u8x8.getCols(); i++) {

    for(int j = 0; j < u8x8.getRows(); j++) {
      
      uint8_t tiles[16] = { 0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0, 1, 3, 7, 15, 31, 63, 127, 255};
      u8x8.drawTile(i, j, 1, tiles);
      delay(50);
    }
    delay(150);
  }

  u8x8.clearDisplay();
  delay(100);
  
  u8x8.drawString(1, 3, "Starting...");
  delay(700);
  u8x8.clearDisplay();
}

void displayLoad() {
  u8x8.drawString(0, 0, "Screen: ");

  for(int i = 7; i < u8x8.getCols() - 1; i++) {
      u8x8.drawString(i,0, "%");
      delay(50);
  } 

  u8x8.drawString(0, 1, "Range: ");
  
  for(int i = 7; i < u8x8.getCols() - 1; i++) {
        u8x8.drawString(i,1, "%");
        delay(50);
  } 

  u8x8.drawString(0, 2, "Button: ");
  
  for(int i = 7; i < u8x8.getCols() - 1; i++) {
        u8x8.drawString(i,2, "%");
        delay(50);
  } 

  u8x8.drawString(2, 5, "Press Button");
  u8x8.drawString(4, 6, "To Start");


}


void readRange() {
    const unsigned char startCommand[] = {0xD, 0xA, 0x4F, 0x4E, 0xD, 0xA};
    const unsigned char stopCommand[] = {0xD, 0xA, 0x4F, 0x46, 0x46, 0xD, 0xA};

    /* Start Command*/
    Rangefinder.write(startCommand, 6);
    u8x8.drawString(1, 3, "Range Finder Active");

    Serial.print("Data Read: "); 

    for(int i = 0; i < 7; i++) {
      Serial.print((char)Rangefinder.read());
    }
    Serial.println(); 

    // u8x8.drawString()
    Rangefinder.write(stopCommand, 7);
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
        char c = Rangefinder.read();

        if(c == NULL) {
          u8x8.drawString(1,3,"NULL VAL");
          return;
        }

        Serial.print(c);
      }
      Serial.println(); 

      // u8x8.drawString()
      Rangefinder.write(stopCommand, 7);

      lastDebounceTime = millis();
    }
  }

float calcFallOff(float range, float velocity) {

  const float varG = 41.697;
  const float varN = 0.5;
  const long varFzero = 3230;

  float varFm = (float)varFzero - (float)(0.75 + 0.00006 * range) * (float)(varN * range);

  // Denominator First
  float recipRange = 1.00/range;
  float recipMultiply = 1.00/(varFm);

  float denominator = recipRange - recipMultiply;

  // Numerator
  float numerator = varG/float(velocity);

  // Final 

  float varD = (float)(numerator / denominator);
  varD *= varD;
  varD /= 12;

  return varD;
}

float calcAngle(float drop, float distance) {

  float dropAngleRads = atan2f(drop, distance);

  float dropAngleDegs = (dropAngleRads * 4068) / 71;

  return dropAngleDegs;
}