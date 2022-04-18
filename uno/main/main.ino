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
U8X8_SSD1306_128X64_VCOMH0_4W_SW_SPI u8x8(/* clock=*/13, /* data=*/11, /* cs=*/8, /* dc=*/A0, /* reset=*/9);
SoftwareSerial Rangefinder(/*RX*/ rangefinderRx, /*TX*/ rangefinderTx);
Gun gunObj;
QuickStats stats;

/* Button Debounce Paramas*/
volatile long lastDebounceTime = 0;
volatile long debounceDelay = 50;

/* States of button */
volatile int buttonState = 0; /* False = Not Pressed  True = Pressed*/

/* Range Finder Commands */
const unsigned char turnOnCommand[] = { 0xD, 0xA, 0x4F, 0x4E, 0xD, 0xA };
const unsigned char turnOffCommand[] = { 0xD, 0xA, 0x4F, 0x46, 0x46, 0xD, 0xA };
const unsigned char slowCommand = 0x4D;

/* Bullet Params */
const float DEFAULT_BULLET_SPEED = 2320;
bool toggleRange = true;

/* I2C Mobile Global Vars*/
String mobileBuffer = "";
char MESSAGE_START = '<';
char MESSAGE_END = '>';
char message[32] = "";
bool isGettingData = false;
bool hasNewData = false;
int index = 0;

// *******************************Setup and Main Loop**********************************
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
  while(buttonState != 0) {
      u8x8.clearDisplay();
      readRange();
  }

  if(hasNewData) {
    handleCommand(message);
    hasNewData = false;
    // Clear message
  }

  // Write to mobile
  if(mobileBuffer.length() != 0) {
    char * mobileCharArray;
    mobileBuffer.toCharArray(mobileCharArray, mobileBuffer.length(), 0);

    Wire.write(mobileCharArray);
    mobileBuffer = "";    
  }

  
  delay(1000);
}

// **************************** I2C Mobile Commnads **********************************
void receiveEvent(int howMany) {
  while(Wire.available() > 0) {
      char curr = (char) Wire.read();

      if(curr == MESSAGE_END) {
        isGettingData = false;
        hasNewData = true;
        index = 0;
      }

      if(isGettingData) {
        message[index] = curr;
        index++;
      }

      if(curr == MESSAGE_START) {
        isGettingData = true;
      }
  }
}

void handleCommand(String inputData) {
  if(inputData.length() == 0) return;
  String com = inputData.substring(0, 4);
  String val = inputData.substring(5);

  val.trim();
  com.trim();

  String setSpeed = "SETS";
  String setWeight = "SETW";
  String displayData = "DISP";
  String setContrast = "SCON";
  String toggleRange = "TRNG";
  String setCommand = "SRNG";
  String clearDisplay = "CLSD";
  
  if(com.equalsIgnoreCase("SETS")) 
  {
      // Set Speed
      gunObj.setBulletSpeed(val.toFloat());
  }
  else if(com.equalsIgnoreCase("SETW"))
  {
      gunObj.setBulletWeight(val.toFloat());
  }
  else if(com.equalsIgnoreCase("DISP")) 
  {
      char* buff;
      val.toCharArray(buff, 15);
      u8x8.drawString(1, 3, buff);
      delay(2000);
  } else if(com.equalsIgnoreCase("SCON")) {
    // Set Contrast
    u8x8.setContrast(val.toInt());
  } else if(com.equalsIgnoreCase("TRNG")) {
    toggleRange = !toggleRange;
  } else if(com.equalsIgnoreCase("SRNG")) {
    displayPix(val.toFloat());
  } else if(com.equalsIgnoreCase("CLSD")) {
    u8x8.clearDisplay();
  }
}
// ************************************************************************************
// ******************************* Start Up Functions *********************************
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
// ************************************************************************************
// ******************************* Rangefinder + Comp Pixel ***************************
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

  int i = 0;
  char allValsArray[50];

  while (Rangefinder.available() > 0) {

    if(i < 50) {
      char c = Rangefinder.read();
      allValsArray[i] = c;
      i++;
    }
  }
  
  int startIndex = 0;
  int endIndex = 0;
  String allVals = String(allValsArray);

  // Filtering bad data through "D" and "m"
  for (int i = 0; i < allVals.length(); i++) {
    if (allVals.charAt(i) == 'D') {
      startIndex = i;
    }

    if (allVals.charAt(i) == 'm') {
      endIndex = i;
    }

    if (startIndex != 0 && endIndex != 0 && startIndex < endIndex && valuePos < 10) {
      String out = allVals.substring(startIndex, endIndex);
      float value = handleString(out);

      if(value == 0.00) {
        startIndex = 0;
        endIndex = 0;
        continue;
      } else {
        values[valuePos] = value;
        startIndex = 0;
        endIndex = 0;
        valuePos++;
      }
    }
  }

  float trueRange = stats.mode(values, valuePos, 0.1);
  // fill buffer with trueRnage;
  mobileBuffer = String(trueRange);

  // Display Compensation Pixel
  displayPix(trueRange);

  // Reset Button State  
  buttonState = 0;
}
void displayPix(float trueRange) {

  // Only work based on 300, 400, 500 and 600
  // 300 yd = 1 pixel down
  // 400 yd = 2 px down
  // 500 yd = 4 px down
  // 600 yd = 6 px down

  unsigned char defaultRectileLeft[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};
  unsigned char defaultRectileRight[8] = {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  unsigned char oneDownRectile[8] = {0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00};
  unsigned char twoDownRectile[8] = {0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};
  unsigned char fourDownRectile[8] = {0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  trueRange = trueRange * 1.09361;

  if(trueRange < 300) {
    u8x8.drawTile(7, 3, 1, defaultRectileLeft);
    u8x8.drawTile(8, 3, 1, defaultRectileRight);
  } else if(trueRange >= 300 && trueRange < 400) {
    u8x8.drawTile(7, 3, 1, oneDownRectile);
  } else if(trueRange >= 400 && trueRange < 500) {
    u8x8.drawTile(7, 3, 1, twoDownRectile);
  } else if(trueRange >= 500 && trueRange < 600) {
    u8x8.drawTile(7, 3, 1, fourDownRectile);
  } 
  else {
    u8x8.drawString(1, 3, "Out of Range");
    delay(1000);
    u8x8.drawTile(7, 3, 1, defaultRectileLeft);
    u8x8.drawTile(7, 3, 1, defaultRectileRight);
  }

  if(toggleRange) {
    char * buff;
    String(trueRange).toCharArray(buff, 10, 0);
    u8x8.drawString(0, 0, buff);
  }
}
// ************************************************************************************
// ******************************* Handlers for Main Functions ************************
void handleButton() {
  // Check for Button Debounce 
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    // Change button state
    buttonState = 1;
    
    // Reset Debounce time    
    lastDebounceTime = millis();
  }
}

float handleString(String inputString) { 
  char buff[inputString.length()]; 
  inputString.toCharArray(buff, inputString.length());
  String tempBuf = "";

  for(int i = 0; i < inputString.length(); i++) {
    if(isDigit(buff[i]) || buff[i] == '.') {
      tempBuf += buff[i];
    }
  }

   return tempBuf.toFloat(); 
}