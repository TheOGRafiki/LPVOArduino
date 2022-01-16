#include <Arduino.h>
#include "Rangefinder.h"
using namespace std;


Rangefinder::Rangefinder(int baudrate) {
    Serial.begin(baudrate);
}

int getData() {

    // Start Hex Code
    const unsigned char writeCommand[] = {0xD, 0xA, 0x4F, 0x4E, 0xD, 0xA};
    size_t byteSent = Serial.write(writeCommand[0]);

    while (Serial.available() > 0)
    {
        long incommingData = Serial.parseInt();
    }
}

