#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C // Common I2C address for SSD1306

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUZZER_PIN   23
#define TOUCH_PIN    4    // Touch pin (Touch0)
#define TOUCH_THRESHOLD 30 // Adjust if touch is too sensitive/insensitive

void setup() {
  Serial.begin(115200);
  
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 25);
  display.println(F("SYSTEM READY"));
  display.display();
  delay(1500);
}

void loop() {
  int touchVal = touchRead(TOUCH_PIN);

  // If touched, trigger ALARM MODE
  if (touchVal < TOUCH_THRESHOLD) {
    triggerAlarm();
  } else {
    // Normal Idle Mode: Dynamic Equalizer Animation
    drawEqualizer();
  }
}

// Visualizer Animation
void drawEqualizer() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.println(F("[ SYSTEM OK ]"));

  // Draw 8 animated sound bars
  for (int i = 0; i < 8; i++) {
    int barHeight = random(5, 45);
    int xPos = 10 + (i * 14);
    display.fillRect(xPos, 55 - barHeight, 10, barHeight, SSD1306_WHITE);
  }

  // Play a soft dynamic tick tone
  tone(BUZZER_PIN, random(800, 1200), 20);

  display.display();
  delay(80);
}

// Alarm & Visual Flash
void triggerAlarm() {
  // Sound alarm & flash screen
  for (int i = 0; i < 3; i++) {
    display.clearDisplay();
    display.fillRect(0, 0, 128, 64, SSD1306_WHITE); // Flash White
    display.display();
    
    // Siren high pitch
    tone(BUZZER_PIN, 2500, 100);
    delay(100);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(18, 24);
    display.println(F("! ALERT !"));
    display.display();
    
    // Siren low pitch
    tone(BUZZER_PIN, 1200, 100);
    delay(100);
  }
  noTone(BUZZER_PIN);
}