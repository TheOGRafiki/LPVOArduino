#include <Arduino.h>
#include <LiquidCrystal.h>
using namespace std;


int rs, en, d4, d5, d6, d7;
int rows, cols;

LiquidCrystal myDisplay(rs, en, d4, d5, d6, d7);

void setDisplay(int rowsIn, int colsIn) {
    rows = rowsIn;
    cols = colsIn;
}

void configureDisplay() {
    myDisplay.begin(rows, cols); // To start the display

    myDisplay.noDisplay(); // Turn off while Configuring

    myDisplay.clear(); // Clearing anything on the display

    myDisplay.noCursor(); // We Do Not want a blinking Cursor

    myDisplay.display(); // Turn on when finishing config

    myDisplay.print("Config Complete"); // Testing
    myDisplay.clear(); // Reseting
}

void write(char data) {
    myDisplay.print(data);
}

