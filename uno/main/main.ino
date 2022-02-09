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
volatile int buttonState = 0; /* False = Not Pressed  True = Pressed*/

/* Range Finder Commands */
const unsigned char turnOnCommand[] = {0xD, 0xA, 0x4F, 0x4E, 0xD, 0xA};
const unsigned char turnOffCommand[] = {0xD, 0xA, 0x4F, 0x46, 0x46 ,0xD, 0xA};

/* ------------------------------------- SETUP ------------------------------------- */
void setup(void) {
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  Rangefinder.begin(115200);
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  
  // Loading Screen
  displayCheckerBoard();

  // Test Components 
  displayLoad();

  // Attach Interupts
  attachInterrupt(digitalPinToInterrupt(buttonPin), changeState, RISING);
}

/* ------------------------------------- MAIN LOOP ------------------------------------- */
void loop(void) { 

  Serial.println(buttonState);
  while(buttonState == 0) {}

  readRange();
  delay(1000);
}

/* ------------------------------------- ISR FUNCTION ------------------------------------- */
void changeState(void)
{
  if (buttonState == 0)
  {
    buttonState++;
  }
  else
  {
    buttonState--;
  }
}

/*------------------------------------- START UP SEQUENCE ------------------------------------- */
void displayCheckerBoard(void) {
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

void displayLoad(void) {
  // If Checker Board Passes Screen is Operational no need for check
  u8x8.drawString(0, 0, "Screen: ");

  for(int i = 7; i < u8x8.getCols() - 1; i++) {
      u8x8.drawString(i,0, "%");
      delay(50);
  } 

  // Check if range is working
  
  u8x8.drawString(0, 1, "Range: ");
  
  for(int i = 7; i < u8x8.getCols() - 1; i++) {
        u8x8.drawString(i,1, "%");
        delay(50);
  } 
  // Wait for button press then continue
  while (digitalRead(buttonPin) != HIGH) {}

  u8x8.drawString(0, 2, "Button: ");
  
  for(int i = 7; i < u8x8.getCols() - 1; i++) {
        u8x8.drawString(i,2, "%");
        delay(50);
  } 

  u8x8.drawString(2, 5, "Press Button");
  u8x8.drawString(4, 6, "To Start");


}

/* -------------------------------------  READ RANGE ------------------------------------- */
void readRange(void) {
    /* Start Command*/
while (Rangefinder.availableForWrite() < 6)
{}

    Rangefinder.write(turnOnCommand, 6);
    
    Serial.println("Range Finder Start");
    Serial.print("Data Read: "); 

    while(Rangefinder.available() > 0) {
      char c = Rangefinder.read();
      Serial.print(c);
      if(c == "m") break;
    }
    Serial.println(); 

    while(Rangefinder.availableForWrite() < 7) {}
    Rangefinder.write(turnOffCommand, 7);
}

/* ------------------------------------- MATH EQUATIONS ------------------------------------- */
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