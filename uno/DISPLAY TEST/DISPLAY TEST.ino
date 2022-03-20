#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(/* clock=*/13, /* data=*/11, /* cs=*/8, /* dc=*/A0, /* reset=*/9);


void setup() {
  // put your setup code here, to run once:
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

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

void loop() {
  displayStart();
}
