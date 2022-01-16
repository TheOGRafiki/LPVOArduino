#ifndef Rangefinder_h
#define Rangefinder_h

#include <Arduino.h>


class Rangefinder {
    public:
        Rangefinder(int baudrate);
        int getData();
};

#endif
