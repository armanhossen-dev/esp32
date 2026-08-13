#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUZZER_PIN   23
#define TOUCH_PIN    4
// Adjust proximity sensitivity:
// Standard reading is ~70-80. Lower values trigger when your hand gets near.
#define PROXIMITY_THRESHOLD 45 

// --- Musical Note Definitions (in Hz) ---
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_A4S 466
#define NOTE_B4  494
#define NOTE_C5  523

// Melody array for Happy Birthday
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_A4S, NOTE_A4S, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4
};

// Note durations: 4 = quarter note, 8 = eighth note, etc.
int noteDurations[] = {
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 2
};

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;); // Loop forever if OLED fails
  }

  showIdleScreen();
}

void loop() {
  int sensorValue = touchRead(TOUCH_PIN);

  // Trigger when hand gets near (capacitance drops)
  if (sensorValue < PROXIMITY_THRESHOLD) {
    playHappyBirthday();
    showIdleScreen(); // Return to waiting screen after song finishes
  }
  
  delay(100);
}

void showIdleScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 20);
  display.println(F("WAVE HAND NEAR"));
  display.setCursor(25, 35);
  display.println(F("TO START! <3"));
  display.display();
}

void playHappyBirthday() {
  int totalNotes = sizeof(melody) / sizeof(melody[0]);

  for (int thisNote = 0; thisNote < totalNotes; thisNote++) {
    // Update OLED graphic per note
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 15);
    display.println(F("HAPPY"));
    display.setCursor(10, 35);
    display.println(F("BIRTHDAY!"));

    // Random decorative twinkling stars on screen
    for (int star = 0; star < 12; star++) {
      display.drawPixel(random(0, 128), random(0, 64), SSD1306_WHITE);
    }
    display.display();

    // Calculate note duration (tempo set to ~1000ms base)
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);

    // Pause between notes to distinguish them
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);
  }
}