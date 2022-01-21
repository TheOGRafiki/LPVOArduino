#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>

/* Constructor */
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 8, /* dc=*/ A0, /* reset=*/ 9);

/* u8x8.begin() is required and will sent the setup/init sequence to the display */
void setup(void)
{
  u8x8.begin();
}

void loop(void)
{
  char* outputRows = "Rows: " + u8x8.getCols();
  char* outputCols = "Cols: " + u8x8.getRows();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(4,4,outputCols);
  u8x8.drawString(0,0, outputRows);
  delay(1000);
}
