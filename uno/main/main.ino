#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <SoftwareSerial.h>

/* Pin Declaration */
const int buttonPin = 2;
const int rangefinderTx = 4;
const int rangefinderRx = 3;

const bool DEBUG = true;

/* Constructor */
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/13, /* data=*/11, /* cs=*/8, /* dc=*/A0, /* reset=*/9);
SoftwareSerial Rangefinder(/*RX*/ rangefinderRx, /*TX*/ rangefinderTx);

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


void setup(void) {
  u8x8.begin();

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButton, RISING);

  Serial.begin(115200);
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  Rangefinder.begin(115200);

  if (!DEBUG) {
    // Loading Screen
    displayStart();

    // Test Components
    displayLoad();
  }
}

void loop(void) {
  // char* tempBuff = buttonState;
  Serial.println(buttonState);
  while (buttonState == 0) {}

  readRange();
  delay(1000);
}

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

void displayLoad() {
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
  int readCounter = 0;
  /* Start Command*/
  if (Rangefinder.availableForWrite() < 7) { Serial.println("Waiting.."); }

  Rangefinder.write(turnOnCommand, 6);
  delay(400);

  Serial.println("Range Finder Start");
  Serial.print("Data Read: ");

  String displayString = "";
  int i = 0;

  while (Rangefinder.available() > 0) {
    char c = (char)Rangefinder.read();
    displayString += c;
    Serial.print(c);

    if (c == 'm') break;
    delay(100);
  }

  Serial.println("");

  delay(200);
  Rangefinder.write(turnOffCommand, 7);

  // Getting Final Distance
  float finalDistance = handleString(displayString);
  Serial.print("finalDistance = ");
  Serial.println(finalDistance, 2);

  // Display Final Distance
  char temp[7];
  String newString = String(finalDistance);
  newString.toCharArray(temp, 7);

  u8x8.clear();
  u8x8.drawString(3, 3, temp);
  delay(250);

  // Calculating Angle
  if(finalDistance != 0.0) {
      float dropAmmount = calcFallOff(finalDistance, DEFAULT_BULLET_SPEED);
      float finalAngle = calcAngle(dropAmmount, finalDistance);

      Serial.print("Drop Amount:");
      Serial.println(dropAmmount, 7); 

      Serial.print("Final Angle:");
      Serial.println(finalAngle, 7);

      displayCompensationPixel(dropAmmount, finalAngle, finalDistance);
  } else if(finalDistance == 0) {
    u8x8.drawString(1, 3, "No Value");
  }

  // Display Compensation


  buttonState--;
}

void handleButton() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonState++;
    lastDebounceTime = millis();
  }
}

void displayCompensationPixel(float drop, float angle, float range) {
  u8x8.drawString(1, 3, "Display Comp");
}

float handleString(String inputString) {

  String handler = inputString;
  // D=XX.Xm
  
  handler.replace("D", "");
  handler.replace("=", "");
  handler.replace("m", "");

  return handler.toFloat();
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