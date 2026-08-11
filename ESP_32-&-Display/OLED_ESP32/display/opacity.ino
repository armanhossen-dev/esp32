#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
}

void loop() {
  // -------------------------------------------------------------
  // DEMO 1: 4 Simulated Shade Levels (0%, 25%, 50%, 100% Brightness)
  // -------------------------------------------------------------
  display.clearDisplay();
  
  // Draw 4 vertical gradient blocks across the screen
  for (int y = 0; y < 48; y++) {
    for (int x = 0; x < 128; x++) {
      // Block 1 (0 - 31px): 0% - Pure Black
      if (x < 32) {
        // Leave pixel off
      } 
      // Block 2 (32 - 63px): 25% - Dark Blue (1 in 4 pixels)
      else if (x < 64) {
        if (x % 2 == 0 && y % 2 == 0) display.drawPixel(x, y, SSD1306_WHITE);
      } 
      // Block 3 (64 - 95px): 50% - Medium Blue (Checkerboard)
      else if (x < 96) {
        if ((x + y) % 2 == 0) display.drawPixel(x, y, SSD1306_WHITE);
      } 
      // Block 4 (96 - 127px): 100% - Solid Bright Blue
      else {
        display.drawPixel(x, y, SSD1306_WHITE);
      }
    }
  }

  // Draw labels under each block
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 52);   display.print("0%");
  display.setCursor(34, 52);  display.print("25%");
  display.setCursor(66, 52);  display.print("50%");
  display.setCursor(96, 52);  display.print("100%");

  display.display();
  delay(4000);

  // -------------------------------------------------------------
  // DEMO 2: Full Inverted Mode (Swaps Black & Blue)
  // -------------------------------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 28);
  display.println("INVERTED COLOR MODE");
  display.display();
  
  delay(1000);
  display.invertDisplay(true);  // Hardware Invert: Background becomes Blue, Text becomes Black
  delay(3000);
  display.invertDisplay(false); // Reset to Normal
}