#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "QuickStats.h"


/* Pin Declaration */
const int buttonPin = 2;
const int rangefinderTx = 4;
const int rangefinderRx = 3;

const bool DEBUG = true;

class Gun {
  public: 
    float bulletSpeed = 2320;
    float bulletWeight = 21; // grams 

    Gun() {Serial.println("New Gun Created");}

    void setBulletSpeed(float newBulletSpeed) {
      bulletSpeed = newBulletSpeed;
    }

    void setBulletWeight(float newBulletWeight) {
      bulletWeight = newBulletWeight; 
    }
};

/* Constructor */
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/13, /* data=*/11, /* cs=*/8, /* dc=*/A0, /* reset=*/9);
SoftwareSerial Rangefinder(/*RX*/ rangefinderRx, /*TX*/ rangefinderTx);
Gun gunObj;
QuickStats stats;

/* Button Debounce Paramas*/
volatile long lastDebounceTime = 0;
volatile long debounceDelay = 50;

/* States of button and display */
volatile unsigned char buttonState = 0; /* False = Not Pressed  True = Pressed*/

/* Range Finder Commands */
const unsigned char turnOnCommand[] = { 0xD, 0xA, 0x4F, 0x4E, 0xD, 0xA };
const unsigned char turnOffCommand[] = { 0xD, 0xA, 0x4F, 0x46, 0x46, 0xD, 0xA };
const unsigned char slowCommand = 0x4D;

/* Bullet Params */
const float DEFAULT_BULLET_SPEED = 2320;
/* ------------------------------------- SETUP ------------------------------------- */
void setup(void) {
  // Start Display Connection
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Button Setup + Interupt   
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButton, RISING);

  // Error Logging 
  Serial.begin(115200);

  // Rangefinder UART connection
  Rangefinder.begin(115200);

  // Start Up Display
  displayLoad();
}

void loop(void) {
  while (buttonState == 0) {} // Wait Until Button Has Been Pressed

  readRange();
  delay(1000);
}

// On Receive 
void receiveEvent(int howMany) {
  int x = 0;
  char * string;

  while(1 < Wire.available()) {
    char c = Wire.read();
    string[x] = c;
    x++;
  }

  u8x8.drawString(1, 3, string);
}

// Display Functions --> NOT NEEDED FOR PROJECT TO WORK
void displayStart() {
  for (int i = 0; i < u8x8.getCols(); i++) {

    for (int j = 0; j < u8x8.getRows(); j++) {

      uint8_t tiles[16] = { 0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0, 1, 3, 7, 15, 31, 63, 127, 255 };
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

void displayLoad(void) {
  // If Checker Board Passes Screen is Operational no need for check
  u8x8.drawString(0, 0, "Screen: ");
  
  // If first Screen Passed then Green Light
  for (int i = 7; i < u8x8.getCols() - 1; i++) {
    u8x8.drawString(i, 0, "%");
    delay(50);
  }

  // Perform a read range and make sure you receive a number
  u8x8.drawString(0, 1, "Range: ");

  for (int i = 7; i < u8x8.getCols() - 1; i++) {
    u8x8.drawString(i, 1, "%");
    delay(50);
  }

  u8x8.drawString(0, 2, "Button: ");

  for (int i = 7; i < u8x8.getCols() - 1; i++) {
    u8x8.drawString(i, 2, "%");
    delay(50);
  }
  u8x8.drawString(0, 3, "USB: ");

   for (int i = 7; i < u8x8.getCols() - 1; i++) {
    u8x8.drawString(i, 3, "%");
    delay(50);
  }

  u8x8.drawString(2, 5, "Press Button");
  u8x8.drawString(4, 6, "To Start");
}

void readRange() {
  // Start Command
  Rangefinder.flush();
  if (Rangefinder.availableForWrite() < 7) { Serial.println("Waiting.."); }
  Rangefinder.write(turnOnCommand, 6);
  delay(500);
  Rangefinder.write(turnOffCommand, 7);

  // Error Logging
  Serial.println("Range Finder Start");
  Serial.print("Data Read: ");

  // Placeholder String for Scoping
  float values[10];
  int valuePos = 0;
  String allVals = "";

  while (Rangefinder.available() > 0) {
    char c = Rangefinder.read();
    allVals += c;
  }

  int startIndex = 0;
  int endIndex = 0;
  // Filtering bad data through "D" and "m"
  for (int i = 0; i < allVals.length(); i++) {
    if (allVals[i] == 'D' && allVals[i + 1] == '=') {
      startIndex = i;
    }

    if (allVals[i] == 'm') {
      endIndex = i;
    }

    if (startIndex != endIndex && valuePos < 10) {
      String out = allVals.substring(startIndex, endIndex + 1);
      float value = handleString(out);
      values[valuePos] = value;
      startIndex = endIndex = 0;
      valuePos++;
    }
  }
  float trueRange = stats.mode(values, values.length(), 0.0001);
  displayPix(trueRange);
// Reset Button State  
  buttonState--;
}
void displayPix(float trueRange) {

  // Only work based on 300, 400, 500 and 600
  // 300 yd = 1 pixel down
  // 400 yd = 2 px down
  // 500 yd = 4 px down
  // 600 yd = 6 px down
}

void handleButton() {
  // Check for Button Debounce 
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    // Change button state
    buttonState++;
    
    // Reset Debounce time    
    lastDebounceTime = millis();
  }
}

int calcNewPos(float angle) {

  int bottomDistance = u8x8.getCols()/2;
  float fixedDistance = (tan(angle) * 180.0f/ PI) * bottomDistance; 

  return fixedDistance;
} 

float handleString(String inputString) {
  
  // See if you can avoid using buffer
  String handler = inputString;
  // D=XX.Xm
  
  handler.replace("D", "");
  handler.replace("=", "");
  handler.replace("m", "");

  return handler.toFloat();
}
/* WE ARE USING A TABLE NOW INSTEAD
float calcFallOff(float range, float velocity) {

  const float varG = 41.697;
  const float varN = 0.5;
  const long varFzero = 3230;

  float varFm = (float)varFzero - (float)(0.75 + 0.00006 * range) * (float)(varN * range);

  // Denominator First
  float recipRange = 1.00 / range;
  float recipMultiply = 1.00 / (varFm);

  float denominator = recipRange - recipMultiply;

  // Numerator
  float numerator = varG / float(velocity);

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
*./