#include <Rangefinder.h>
#include <string.h>
#include <Display.h>

void setup() { 
  Rangefinder range = Rangefinder(115200);
  Display dp = Display(1, 16);

  range.test();
  dp.test();
}

void loop() {
  // put your main code here, to run repeatedly:

}

