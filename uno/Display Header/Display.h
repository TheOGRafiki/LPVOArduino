#ifndef display_h
#define display_h

#include <string.h>
#include <Arduino.h>

class Diplay {
    public:
        void setDisplay(int rowsIn, int colsIn);
        void configureDisplay();
        void write(char data);

    private:
        int rs, en, d4, d5, d6, d7;
        int rows, cols;
};

#endif
