/*
  ESP32 Desktop Buddy — animated eyes on 0.96" SSD1306 OLED
  ------------------------------------------------------------
  Wiring (standard ESP32 dev board, CH340/CP2102 USB-serial):
    OLED VCC -> 3.3V
    OLED GND -> GND
    OLED SDA -> GPIO 21
    OLED SCL -> GPIO 22

  Libraries needed (Library Manager):
    - Adafruit GFX Library
    - Adafruit SSD1306

  If the screen stays blank, try changing SCREEN_ADDRESS below
  to 0x3D (some 0.96" modules use that instead of 0x3C).

  Behavior: picks a random face expression, holds it for a random
  duration, blink-transitions to the next one, forever. No sound.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- config ----------
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- eye geometry ----------
const int eyeW = 34;
const int eyeH = 34;
const int eyeGap = 14;
const int cornerRadius = 12;
int leftX, rightX, eyeY;

// ---------- expressions ----------
enum Expression {
  NORMAL, HAPPY, SLEEPY, SURPRISED, ANGRY, WINK, LOVE, DIZZY, SUSPICIOUS,
  EXPR_COUNT
};

Expression currentExpr = NORMAL;
unsigned long exprStartTime = 0;
unsigned long exprDuration = 2000;

// pupil drift (used by NORMAL)
float pupilOffsetX = 0, pupilOffsetY = 0;
int targetPupilX = 0, targetPupilY = 0;
unsigned long lastDriftUpdate = 0;

// idle blink (used by NORMAL)
unsigned long lastBlinkTime = 0;
unsigned long nextBlinkInterval = 3000;

// ---------- setup ----------
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for (;;) delay(1000);
  }

  display.clearDisplay();
  display.display();
  randomSeed(analogRead(34)); // any floating ADC pin works for entropy

  leftX  = SCREEN_WIDTH / 2 - eyeGap / 2 - eyeW;
  rightX = SCREEN_WIDTH / 2 + eyeGap / 2;
  eyeY   = (SCREEN_HEIGHT - eyeH) / 2;

  exprStartTime = millis();
  exprDuration  = random(1500, 4000);
  nextBlinkInterval = random(2000, 5000);
}

// ---------- main loop ----------
void loop() {
  unsigned long now = millis();

  if (now - exprStartTime > exprDuration) {
    startTransition();
    now = millis();
  }

  display.clearDisplay();
  renderCurrentState(now);
  display.display();
  delay(20); // ~50fps
}

// ---------- helpers ----------
void drawThickLine(int x0, int y0, int x1, int y1, int thickness) {
  for (int t = -thickness / 2; t <= thickness / 2; t++) {
    display.drawLine(x0, y0 + t, x1, y1 + t, SSD1306_WHITE);
  }
}

int breathe(unsigned long now) {
  return (int)(sin(now / 600.0) * 2);
}

void drawEyePairSimple(int h) {
  if (h < 2) h = 2;
  int y = eyeY + (eyeH - h) / 2;
  int r = min(cornerRadius, h / 2);
  display.fillRoundRect(leftX, y, eyeW, h, r, SSD1306_WHITE);
  display.fillRoundRect(rightX, y, eyeW, h, r, SSD1306_WHITE);
}

// quick single blink, used only inside NORMAL idle state
void quickBlink() {
  for (int h = eyeH; h >= 4; h -= 6) {
    display.clearDisplay();
    drawEyePairSimple(h);
    display.display();
    delay(12);
  }
  for (int h = 4; h <= eyeH; h += 6) {
    display.clearDisplay();
    drawEyePairSimple(h);
    display.display();
    delay(12);
  }
}

void resetExpressionState() {
  pupilOffsetX = 0; pupilOffsetY = 0;
  targetPupilX = 0; targetPupilY = 0;
  lastDriftUpdate = millis();
  lastBlinkTime = millis();
  nextBlinkInterval = random(2000, 5000);
}

// blink-closed, switch face, blink-open — hides the "cut" between expressions
void startTransition() {
  for (int h = eyeH; h >= 2; h -= 4) {
    display.clearDisplay();
    drawEyePairSimple(h);
    display.display();
    delay(15);
  }

  Expression newExpr;
  do {
    newExpr = (Expression)random(0, EXPR_COUNT);
  } while (newExpr == currentExpr);
  currentExpr = newExpr;
  resetExpressionState();

  for (int h = 2; h <= eyeH; h += 4) {
    display.clearDisplay();
    drawEyePairSimple(h);
    display.display();
    delay(15);
  }

  exprStartTime = millis();
  exprDuration  = random(1500, 4500);
}

void renderCurrentState(unsigned long now) {
  switch (currentExpr) {
    case NORMAL:     renderNormal(now);     break;
    case HAPPY:      renderHappy(now);      break;
    case SLEEPY:     renderSleepy(now);     break;
    case SURPRISED:  renderSurprised(now);  break;
    case ANGRY:      renderAngry(now);      break;
    case WINK:       renderWink(now);       break;
    case LOVE:       renderLove(now);       break;
    case DIZZY:      renderDizzy(now);      break;
    case SUSPICIOUS: renderSuspicious(now); break;
    default: break;
  }
}

// ---------- expressions ----------

void renderNormal(unsigned long now) {
  if (now - lastDriftUpdate > 1200) {
    targetPupilX = random(-8, 9);
    targetPupilY = random(-5, 6);
    lastDriftUpdate = now;
  }
  pupilOffsetX += (targetPupilX - pupilOffsetX) * 0.15;
  pupilOffsetY += (targetPupilY - pupilOffsetY) * 0.15;

  if (now - lastBlinkTime > nextBlinkInterval) {
    quickBlink();
    lastBlinkTime = millis();
    nextBlinkInterval = random(2500, 6000);
    return; // this frame already drawn+displayed inside quickBlink
  }

  int by = breathe(now);
  display.fillRoundRect(leftX, eyeY + by, eyeW, eyeH, cornerRadius, SSD1306_WHITE);
  display.fillRoundRect(rightX, eyeY + by, eyeW, eyeH, cornerRadius, SSD1306_WHITE);
  display.fillRoundRect(leftX + eyeW / 2 - 5 + (int)pupilOffsetX,
                         eyeY + eyeH / 2 - 5 + (int)pupilOffsetY + by, 10, 10, 3, SSD1306_BLACK);
  display.fillRoundRect(rightX + eyeW / 2 - 5 + (int)pupilOffsetX,
                         eyeY + eyeH / 2 - 5 + (int)pupilOffsetY + by, 10, 10, 3, SSD1306_BLACK);
}

void renderHappy(unsigned long now) {
  drawThickLine(leftX, eyeY + eyeH - 6, leftX + eyeW / 2, eyeY + 8, 4);
  drawThickLine(leftX + eyeW / 2, eyeY + 8, leftX + eyeW, eyeY + eyeH - 6, 4);
  drawThickLine(rightX, eyeY + eyeH - 6, rightX + eyeW / 2, eyeY + 8, 4);
  drawThickLine(rightX + eyeW / 2, eyeY + 8, rightX + eyeW, eyeY + eyeH - 6, 4);
}

void renderSleepy(unsigned long now) {
  int h = 8;
  int y = eyeY + (eyeH - h) / 2;
  display.fillRoundRect(leftX, y, eyeW, h, 3, SSD1306_WHITE);
  display.fillRoundRect(rightX, y, eyeW, h, 3, SSD1306_WHITE);

  for (int i = 0; i < 3; i++) {
    unsigned long cycle = (now + i * 700UL) % 2100UL;
    float progress = cycle / 2100.0;
    if (progress < 0.85) {
      int zx = 92 + i * 8;
      int zy = 50 - (int)(progress * 35);
      display.setTextSize(1 + (i == 2 ? 1 : 0));
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(zx, zy);
      display.print("Z");
    }
  }
}

void renderSurprised(unsigned long now) {
  int r = 20;
  int cxL = leftX + eyeW / 2, cxR = rightX + eyeW / 2, cy = eyeY + eyeH / 2;
  display.fillCircle(cxL, cy, r, SSD1306_WHITE);
  display.fillCircle(cxR, cy, r, SSD1306_WHITE);
  display.fillCircle(cxL, cy, 5, SSD1306_BLACK);
  display.fillCircle(cxR, cy, 5, SSD1306_BLACK);
}

void renderAngry(unsigned long now) {
  int h = eyeH - 10;
  int y = eyeY + 10;
  display.fillRoundRect(leftX, y, eyeW, h, 6, SSD1306_WHITE);
  display.fillRoundRect(rightX, y, eyeW, h, 6, SSD1306_WHITE);
  drawThickLine(leftX - 2, eyeY - 2, leftX + eyeW + 4, eyeY + 10, 3);
  drawThickLine(rightX + eyeW + 2, eyeY - 2, rightX - 4, eyeY + 10, 3);
}

void renderWink(unsigned long now) {
  display.fillRoundRect(leftX, eyeY, eyeW, eyeH, cornerRadius, SSD1306_WHITE);
  display.fillRoundRect(leftX + eyeW / 2 - 5, eyeY + eyeH / 2 - 5, 10, 10, 3, SSD1306_BLACK);
  display.fillRoundRect(rightX, eyeY + eyeH / 2 - 3, eyeW, 6, 3, SSD1306_WHITE);
}

void drawHeart(int cx, int cy, float size) {
  int r = (int)(size * 0.35);
  display.fillCircle(cx - r, cy - r / 2, r, SSD1306_WHITE);
  display.fillCircle(cx + r, cy - r / 2, r, SSD1306_WHITE);
  display.fillTriangle(cx - r * 2, cy - r / 4, cx + r * 2, cy - r / 4,
                        cx, cy + (int)(size * 0.6), SSD1306_WHITE);
}

void renderLove(unsigned long now) {
  float pulse = sin(now / 300.0) * 3;
  drawHeart(leftX + eyeW / 2, eyeY + eyeH / 2, 18 + pulse);
  drawHeart(rightX + eyeW / 2, eyeY + eyeH / 2, 18 + pulse);
}

void drawXEye(int cx, int cy, int r, float angleOffset) {
  for (int k = 0; k < 2; k++) {
    float a = (k == 0 ? 0.785 : -0.785) + angleOffset;
    int x0 = cx - (int)(cos(a) * r);
    int y0 = cy - (int)(sin(a) * r);
    int x1 = cx + (int)(cos(a) * r);
    int y1 = cy + (int)(sin(a) * r);
    drawThickLine(x0, y0, x1, y1, 3);
  }
}

void renderDizzy(unsigned long now) {
  float wobble = sin(now / 200.0) * 0.4;
  drawXEye(leftX + eyeW / 2, eyeY + eyeH / 2, 14, wobble);
  drawXEye(rightX + eyeW / 2, eyeY + eyeH / 2, 14, wobble);
}

void renderSuspicious(unsigned long now) {
  int h = 14;
  int y = eyeY + (eyeH - h) / 2 + 6;
  display.fillRoundRect(leftX, y, eyeW, h, 5, SSD1306_WHITE);
  display.fillRoundRect(rightX, y, eyeW, h, 5, SSD1306_WHITE);
  int shift = (int)(sin(now / 900.0) * 6);
  display.fillRoundRect(leftX + eyeW / 2 - 5 + shift, y + h / 2 - 4, 8, 8, 2, SSD1306_BLACK);
  display.fillRoundRect(rightX + eyeW / 2 - 5 + shift, y + h / 2 - 4, 8, 8, 2, SSD1306_BLACK);
}
