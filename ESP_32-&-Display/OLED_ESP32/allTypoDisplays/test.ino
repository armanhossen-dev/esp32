#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Custom 16x16 Heart Bitmap Array
const uint8_t PROGMEM heart_bmp[] = {
  0x00, 0x00, 0x1C, 0x38, 0x3E, 0x7C, 0x7F, 0xFE, 
  0x7F, 0xFE, 0x7F, 0xFE, 0x3F, 0xFC, 0x1F, 0xF8, 
  0x0F, 0xF0, 0x07, 0xE0, 0x03, 0xC0, 0x01, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

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
  // FEATURE 1: Text Sizes & Formatting
  // -------------------------------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Size 1: Hello!");

  display.setTextSize(2);
  display.setCursor(0, 16);
  display.println("Size 2");

  display.setTextSize(3);
  display.setCursor(0, 38);
  display.println("Size 3");
  
  display.display();
  delay(2500);

  // -------------------------------------------------------------
  // FEATURE 2: Inverted Text & Highlighting
  // -------------------------------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("Normal Text Mode");

  // Inverted text (Black text over White background box)
  display.fillRect(8, 30, 112, 20, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.setCursor(14, 36);
  display.println("INVERTED HIGHLIGHT");

  display.display();
  delay(2500);

  // -------------------------------------------------------------
  // FEATURE 3: Geometry & Shapes (Lines, Rectangles, Circles, Triangles)
  // -------------------------------------------------------------
  display.clearDisplay();
  
  // Wireframe Rect & Filled Rect
  display.drawRect(0, 0, 30, 30, SSD1306_WHITE);
  display.fillRect(35, 0, 30, 30, SSD1306_WHITE);

  // Wireframe Circle & Filled Circle
  display.drawCircle(80, 15, 12, SSD1306_WHITE);
  display.fillCircle(110, 15, 12, SSD1306_WHITE);

  // Triangles
  display.drawTriangle(10, 60, 25, 38, 40, 60, SSD1306_WHITE);
  display.fillTriangle(60, 60, 75, 38, 90, 60, SSD1306_WHITE);

  // Rounded Box
  display.drawRoundRect(98, 38, 28, 24, 4, SSD1306_WHITE);

  display.display();
  delay(3000);

  // -------------------------------------------------------------
  // FEATURE 4: Dithering Gray Scale & Shading (Dot-Density)
  // -------------------------------------------------------------
  display.clearDisplay();
  
  // 25% Shading Block
  for (int y = 0; y < 40; y++) {
    for (int x = 0; x < 40; x++) {
      if (x % 2 == 0 && y % 2 == 0) display.drawPixel(x, y, SSD1306_WHITE);
    }
  }

  // 50% Checkerboard Shading Block
  for (int y = 0; y < 40; y++) {
    for (int x = 44; x < 84; x++) {
      if ((x + y) % 2 == 0) display.drawPixel(x, y, SSD1306_WHITE);
    }
  }

  // 100% Solid Block
  display.fillRect(88, 0, 40, 40, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(8, 48);   display.print("25%");
  display.setCursor(52, 48);  display.print("50%");
  display.setCursor(92, 48);  display.print("100%");

  display.display();
  delay(3000);

  // -------------------------------------------------------------
  // FEATURE 5: Custom Bitmaps & Icons
  // -------------------------------------------------------------
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(18, 0);
  display.println("BITMAP RENDERING");
  
  // Draw custom 16x16 bitmap at various positions
  display.drawBitmap(20, 24, heart_bmp, 16, 16, SSD1306_WHITE);
  display.drawBitmap(56, 24, heart_bmp, 16, 16, SSD1306_WHITE);
  display.drawBitmap(92, 24, heart_bmp, 16, 16, SSD1306_WHITE);

  display.display();
  delay(2500);

  // -------------------------------------------------------------
  // FEATURE 6: Real-time UI Progress Bar Animation
  // -------------------------------------------------------------
  for (int progress = 0; progress <= 100; progress += 5) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(25, 10);
    display.println("SYSTEM LOADING");

    // Outer Progress Border
    display.drawRoundRect(10, 30, 108, 16, 4, SSD1306_WHITE);

    // Inner Loading Bar
    int barWidth = map(progress, 0, 100, 0, 102);
    display.fillRoundRect(13, 33, barWidth, 10, 2, SSD1306_WHITE);

    display.setCursor(50, 50);
    display.print(progress);
    display.print("%");

    display.display();
    delay(100);
  }
  delay(1000);

  // -------------------------------------------------------------
  // FEATURE 7: Hardware Text Scrolling & Screen Inversion
  // -------------------------------------------------------------
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(8, 24);
  display.println("SCROLL DEMO");
  display.display();

  // Hardware Smooth Right Scroll
  display.startscrollright(0x00, 0x0F);
  delay(2500);
  display.stopscroll();

  // Hardware Full Display Inversion (Flips all lit & dark pixels)
  display.invertDisplay(true);
  delay(1500);
  display.invertDisplay(false);
  delay(1000);
}