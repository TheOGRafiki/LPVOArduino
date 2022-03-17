#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <SoftwareSerial.h>
#include <U8g2lib.h>

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

/* Pin Declaration */
const int buttonPin = 2;
const int rangefinderTx = 4;
const int rangefinderRx = 3;

const bool DEBUG = true;

/* Constructor */
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/13, /* data=*/11, /* cs=*/8, /* dc=*/A0, /* reset=*/9);
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 8, /* dc=*/ A0, /* reset=*/ 9);

SoftwareSerial Rangefinder(/*RX*/ rangefinderRx, /*TX*/ rangefinderTx);
Gun gunObj;

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


/* ------------------------------------- SETUP ------------------------------------- */
void setup(void) {
  // Start Display Connection
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Button Setup + Interupt   
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButton, RISING);

  // Error Logging 
  Serial.begin(115200);

  // Rangefinder UART connection
  Rangefinder.begin(115200);
}

void loop(void) {
  while (buttonState == 0) {} // Wait Until Button Has Been Pressed

  readRange();
  delay(1000);
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

  u8x8.drawString(2, 5, "Press Button");
  u8x8.drawString(4, 6, "To Start");
}

void readRange() {
  // Start Command
  if (Rangefinder.availableForWrite() < 7) { Serial.println("Waiting.."); }
  Rangefinder.write(turnOnCommand, 6);
  delay(400);
  
  // Error Logging
  Serial.println("Range Finder Start");
  Serial.print("Data Read: ");

  // Placeholder String for Scoping
  String displayString = "";

  while (Rangefinder.available() > 0) {
    char c = (char)Rangefinder.read();
    displayString += c;

    // Error Logging
    Serial.print(c);

    // Last character should be m but setup catch for errors
    if (c == 'm') break;
    delay(100);
  }

  // Error Logging  
  Serial.println("");

  // Stop Command
  Rangefinder.write(turnOffCommand, 7);
  delay(200);

  // Getting Final Distance
  float finalDistance = handleString(displayString);

  // Error Logging
  Serial.print("finalDistance = ");
  Serial.println(finalDistance, 2);

  // u8x8 needs Char Array not String
  char temp[7];
  String newString = String(finalDistance);
  newString.toCharArray(temp, 7);

  // Display String on Display    
  u8x8.clear();
  u8x8.drawString(3, 3, temp);
  delay(250);

  // Calculating Angle
  if(finalDistance != 0.0) {
<<<<<<< Updated upstream
    float dropAmmount = calcFallOff(finalDistance, DEFAULT_BULLET_SPEED);
    float finalAngle = calcAngle(dropAmmount, finalDistance);

    // Error Logging
    Serial.print("Drop Amount:");
    Serial.println(dropAmmount, 7); 

    // Error Logging
    Serial.print("Final Angle:");
    Serial.println(finalAngle, 7);

    // Final Display
    displayCompensationPixel(dropAmmount, finalAngle, finalDistance);
      
  } else if(finalDistance == 0) {   
    u8x8.drawString(1, 3, "No Value"); // Catch Error from Handle String
  }
// Reset Button State  
=======
      float dropAmmount = calcFallOff(finalDistance, gunObj.bulletSpeed);
      float finalAngle = calcAngle(dropAmmount, finalDistance);

      Serial.print("Drop Amount:");
      Serial.println(dropAmmount, 7); 

      Serial.print("Final Angle:");
      Serial.println(finalAngle, 7);

      displayCompensationPixel(dropAmmount, finalAngle, finalDistance);
  } else if(finalDistance == 0) {
    u8x8.drawString(1, 3, "No Value");
  }
>>>>>>> Stashed changes
  buttonState--;
}

void handleButton() {
  // Check for Button Debounce 
  if ((millis() - lastDebounceTime) > debounceDelay) {
<<<<<<< Updated upstream
    
    // Change button state
=======
    if(buttonState > 1) {
      buttonState--;
      return;
    }
>>>>>>> Stashed changes
    buttonState++;
    
    // Reset Debounce time    
    lastDebounceTime = millis();
  }
}

void displayCompensationPixel(float drop, float angle, float range) {
  u8x8.clearDisplay();
  unsigned char rectile[8] ={0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f};

  int centerRow = u8x8.getRows()/2;
  int centerCol = u8x8.getCols()/2;

  u8x8.drawTile(centerCol, centerRow, 1, rectile);

  int newPosition = calcNewPos(angle);

  u8x8.drawTile(centerCol, newPosition, 1, rectile);
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

int calcNewPos(float angle) {

  int bottomDistance = u8x8.getCols()/2;
  float fixedDistance = (tan(angle) * 180.0f/ PI) * bottomDistance; 

  return fixedDistance;
} 

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