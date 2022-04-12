#include <Arduino.h>
#include <U8x8lib.h>
#include <SoftwareSerial.h>
#include <Wire.h>

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

U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/13, /* data=*/11, /* cs=*/8, /* dc=*/A0, /* reset=*/9);
Gun gunObj;

char inData[20];
int counter=0;

void setup() { 
  Serial.begin(9600);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
}

/*
0x01 - Request Data
0x02 - Set Gun Speed
0x03 - Set Gun Weight
0x04 - Parse Data Into Display 
*/

void loop() {
  handleSerial();
  delay(1000);

}
void handleSerial() {
  while(Serial.available() > 0) {
    char aChar = Serial.read();
    Serial.print(aChar);

    inData[counter] = aChar;
    counter++;
  }

  handleCommand(String(inData));
}
void receiveEvent(int howMany) {
    // Receive Data
    char inData[20];
    int counter = 0;

    while(Wire.available() > 0) {
      char aChar = Wire.read();
      inData[counter] = aChar;
      counter++;
    }
    inData[counter++] = '\n';

    u8x8.drawString(1, 3, inData);
    handleCommand(String(inData));
}

void handleCommand(String inputData) {
  if(inputData.length() == 0) return;
  String com = inputData.substring(0, 4);

  String setSpeed = "SETS";
  String setWeight = "SETW";
  String requestData = "REQD";
  String displayData = "DISP";
  
  if(com.equalsIgnoreCase(requestData))
  {
      // Send Data
      Serial.println("Case 1");
  }
  else if(com.equalsIgnoreCase("SETS")) 
  {
      // Set Speed
      gunObj.setBulletSpeed(inputData.substring(4).toFloat());
      Serial.println(gunObj.bulletSpeed);
      Serial.println("Case 2");
  }
  else if(com.equalsIgnoreCase("SETW"))
  {
      gunObj.setBulletWeight(inputData.substring(4).toFloat());
      Serial.println("Case 3");
  }
  else if(com.equalsIgnoreCase("DISP")) 
  {
      char* buff;
      inputData.substring(4).toCharArray(buff, 15);
      u8x8.drawString(1, 3, buff);
      Serial.println("Case 4");
  }
  
  inData[0] = '\0';
}



















