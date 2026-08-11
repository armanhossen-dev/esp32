#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WebServer server(80);

// Wi-Fi Credentials
const char* ssid     = "life";
const char* password = "hhhh5533";

// Display State Variables
String customMessage = "Control me from phone!";
int progressVal = 50;
bool invertState = false;

// HTML Webpage with Modern Touch UI
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 OLED Phone Control</title>
    <style>
        * { box-sizing: border-box; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
        body { background: #0f172a; color: #f8fafc; padding: 20px; margin: 0; display: flex; justify-content: center; }
        .card { background: #1e293b; padding: 24px; border-radius: 16px; width: 100%; max-width: 400px; box-shadow: 0 10px 25px rgba(0,0,0,0.5); }
        h2 { color: #38bdf8; margin-top: 0; text-align: center; font-size: 22px; }
        label { font-size: 14px; color: #94a3b8; font-weight: 600; display: block; margin-top: 15px; margin-bottom: 5px; }
        input[type="text"] { width: 100%; padding: 12px; background: #0f172a; border: 1px solid #334155; color: white; border-radius: 8px; font-size: 16px; }
        input[type="range"] { width: 100%; margin: 10px 0; accent-color: #38bdf8; }
        .btn { width: 100%; padding: 12px; border: none; border-radius: 8px; font-size: 16px; font-weight: bold; cursor: pointer; margin-top: 10px; transition: 0.2s; }
        .btn-blue { background: #0284c7; color: white; }
        .btn-blue:active { background: #0369a1; }
        .btn-purple { background: #8b5cf6; color: white; }
        .btn-purple:active { background: #7c3aed; }
        .btn-orange { background: #f97316; color: white; }
        .btn-orange:active { background: #ea580c; }
        .row { display: flex; gap: 10px; }
    </style>
</head>
<body>
    <div class="card">
        <h2>📱 OLED Dashboard</h2>
        
        <form action="/setText" method="GET">
            <label>Send Message to Screen</label>
            <input type="text" name="msg" placeholder="Type text..." required maxlength="40">
            <button type="submit" class="btn btn-blue">Update Text</button>
        </form>

        <form action="/setProgress" method="GET">
            <label>Live Progress Bar (<span id="val">50</span>%)</label>
            <input type="range" name="val" min="0" max="100" value="50" oninput="document.getElementById('val').innerText = this.value">
            <button type="submit" class="btn btn-purple">Set Progress</button>
        </form>

        <label>Display Actions</label>
        <div class="row">
            <button onclick="location.href='/toggleInvert'" class="btn btn-orange">Toggle Invert</button>
            <button onclick="location.href='/flash'" class="btn btn-blue">Flash Screen</button>
        </div>
    </div>
</body>
</html>
)rawliteral";

// --- Function to Redraw OLED UI ---
void updateOLED() {
  display.clearDisplay();

  // Header Title
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 0);
  display.println("SMART CONTROLLER");
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  // Custom Message Body
  display.setCursor(0, 18);
  display.println(customMessage);

  // Progress Bar UI
  display.setCursor(0, 42);
  display.print("Level: ");
  display.print(progressVal);
  display.print("%");

  display.drawRoundRect(0, 52, 128, 12, 3, SSD1306_WHITE);
  int barWidth = map(progressVal, 0, 100, 0, 122);
  display.fillRoundRect(3, 55, barWidth, 6, 2, SSD1306_WHITE);

  display.display();
}

// --- Web Server Routes ---
void handleRoot() {
  server.send(200, "text/html", HTML_PAGE);
}

void handleSetText() {
  if (server.hasArg("msg")) {
    customMessage = server.arg("msg");
    updateOLED();
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSetProgress() {
  if (server.hasArg("val")) {
    progressVal = server.arg("val").toInt();
    updateOLED();
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleToggleInvert() {
  invertState = !invertState;
  display.invertDisplay(invertState);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleFlash() {
  for (int i = 0; i < 4; i++) {
    display.invertDisplay(true);
    delay(100);
    display.invertDisplay(false);
    delay(100);
  }
  display.invertDisplay(invertState);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Connect to Wi-Fi
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Connecting Wi-Fi...");
  display.println(ssid);
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Web Server Route Definitions
  server.on("/", handleRoot);
  server.on("/setText", handleSetText);
  server.on("/setProgress", handleSetProgress);
  server.on("/toggleInvert", handleToggleInvert);
  server.on("/flash", handleFlash);
  server.begin();

  // Show IP Address on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("CONNECTED!");
  display.println("\nOpen Phone Browser:");
  display.setTextSize(1);
  display.println(WiFi.localIP());
  display.display();

  delay(4000);
  updateOLED();
}

void loop() {
  server.handleClient(); // Keep listening for incoming commands from phone
}