#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

// OLED Display Object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Wi-Fi Credentials from Router Settings
const char* ssid     = "life";
const char* password = "hhhh5533";

// NTP Server Settings (UTC+6 for Bangladesh)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 21600; 
const int   daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Display connecting message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 15);
  display.println("Connecting Wi-Fi...");
  display.println(ssid);
  display.display();

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  // Show error on screen if connection fails
  if (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Connection Failed!");
    display.println("Check router power");
    display.display();
    return;
  }

  // Sync time
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("Connected!");
  display.println("Syncing Time...");
  display.display();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(1500);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  // Convert 24h hour to 12h hour & determine AM/PM
  int hour12 = timeinfo.tm_hour % 12;
  if (hour12 == 0) hour12 = 12; // 00:00 becomes 12 AM, 12:00 becomes 12 PM
  const char* ampm = (timeinfo.tm_hour >= 12) ? "PM" : "AM";

  // Draw Clock Interface
  display.clearDisplay();

  // Title Header
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 2);
  display.println(F("REAL-TIME CLOCK"));

  // Line separator
  display.drawFastHLine(0, 14, 128, SSD1306_WHITE);

  // Display Time (hh:mm:ss)
  display.setTextSize(2);
  display.setCursor(8, 22);
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", hour12, timeinfo.tm_min, timeinfo.tm_sec);
  display.print(timeStr);

  // Display AM/PM
  display.setTextSize(1);
  display.setCursor(106, 28);
  display.print(ampm);

  // Display Date (DD-MM-YYYY)
  display.setCursor(28, 48);
  char dateStr[11];
  sprintf(dateStr, "%02d-%02d-%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  display.print(dateStr);

  display.display();
  delay(1000);
}