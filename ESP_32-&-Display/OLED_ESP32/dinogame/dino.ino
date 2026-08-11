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

// Game Variables
int dinoY = 44;          // Dino vertical position
int dinoVelocity = 0;    // Jump speed
bool isJumping = false;
const int gravity = 1;

int cactusX = 128;       // Obstacle position
int score = 0;
bool gameOver = false;

// Trigger jump command
volatile bool jumpRequested = false;

// HTML Web Page for Phone Controller
const char HTML_CONTROLLER[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>Dino Game Controller</title>
    <style>
        body {
            background-color: #121212;
            color: white;
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 0;
            padding: 20px;
            touch-action: manipulation;
        }
        h1 { color: #00bcd4; margin-bottom: 30px; }
        .jump-btn {
            background-color: #ff4081;
            color: white;
            border: none;
            width: 80vw;
            height: 80vw;
            max-width: 300px;
            max-height: 300px;
            border-radius: 50%;
            font-size: 32px;
            font-weight: bold;
            box-shadow: 0 10px 25px rgba(255, 64, 129, 0.4);
            cursor: pointer;
            outline: none;
            user-select: none;
        }
        .jump-btn:active {
            transform: scale(0.95);
            background-color: #e91e63;
        }
    </style>
</head>
<body>
    <h1>🦖 DINO CONTROLLER</h1>
    <button class="jump-btn" onmousedown="triggerJump()" ontouchstart="triggerJump()">JUMP!</button>

    <script>
        function triggerJump() {
            fetch('/jump', { method: 'POST' });
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", HTML_CONTROLLER);
}

void handleJump() {
  jumpRequested = true;
  server.send(200, "text/plain", "OK");
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

  // Web Server Routes
  server.on("/", handleRoot);
  server.on("/jump", HTTP_POST, handleJump);
  server.begin();

  // Show Controller IP Address on Screen
  display.clearDisplay();
  display.setCursor(0, 5);
  display.println("CONNECTED!");
  display.println("\nOpen Phone Browser:");
  display.setTextSize(1);
  display.println(WiFi.localIP());
  display.println("\nStarting Game...");
  display.display();

  delay(4000);
}

void loop() {
  server.handleClient(); // Handle HTTP requests from phone

  if (!gameOver) {
    // --- 1. JUMP LOGIC ---
    if (jumpRequested && !isJumping) {
      dinoVelocity = -7; // Push dino upward
      isJumping = true;
      jumpRequested = false;
    }
    jumpRequested = false;

    // Apply Gravity
    if (isJumping) {
      dinoY += dinoVelocity;
      dinoVelocity += gravity;

      if (dinoY >= 44) { // Floor level
        dinoY = 44;
        isJumping = false;
      }
    }

    // --- 2. CACTUS MOVEMENT ---
    cactusX -= 4; // Speed of incoming cactus
    if (cactusX < -8) {
      cactusX = 128; // Reset cactus to right side
      score++;
    }

    // --- 3. COLLISION DETECTION ---
    // Dino bounding box: X=10 to 18, Y=dinoY to dinoY+12
    // Cactus bounding box: X=cactusX to cactusX+6, Y=44 to 56
    if (cactusX >= 8 && cactusX <= 18 && dinoY >= 36) {
      gameOver = true;
    }

    // --- 4. RENDER GAME FRAME ---
    display.clearDisplay();

    // Draw Ground Line
    display.drawFastHLine(0, 56, 128, SSD1306_WHITE);

    // Draw Score
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(80, 0);
    display.print("Score:");
    display.print(score);

    // Draw Dino (Simple Pixel Box Representation)
    display.fillRect(10, dinoY, 8, 12, SSD1306_WHITE); // Body
    display.fillRect(14, dinoY - 3, 6, 5, SSD1306_WHITE); // Head

    // Draw Cactus
    display.fillRect(cactusX, 44, 6, 12, SSD1306_WHITE);

    display.display();
  } else {
    // --- GAME OVER SCREEN ---
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 15);
    display.println("GAME OVER");

    display.setTextSize(1);
    display.setCursor(25, 40);
    display.print("Final Score: ");
    display.print(score);

    display.setCursor(10, 54);
    display.print("Tap Phone to Restart");
    display.display();

    // Restart game when button on phone is pressed
    if (jumpRequested) {
      gameOver = false;
      score = 0;
      cactusX = 128;
      dinoY = 44;
      isJumping = false;
      jumpRequested = false;
    }
  }

  delay(30); // Frame rate controller (~33 FPS)
}