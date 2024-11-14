#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int buttonPowerPin = 2;
const int buttonReadPin = 3;  // Pin connected to the button
const int buttonGroundPin = 4;

// Define screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define I2C address
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
long countsPerSecond = 150000;
long loopCounter = 0;
int secondCounter = 0;
bool buttonPressed = false;  // Variable for reading the button status
int buttonPressedTimer = 0;
long buttonPressedResetTimer = 0;
int buttonPressedMinimumCount = 1000;
int buttonReleasedTimer = 0;
int buttonReleasedMinimumCount = 1000;
bool buttonHasBeenPressed = false;
long pauseCounter = 0;
char minutesSecondsBuffer[6];

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  pinMode(buttonReadPin, INPUT);    // Set the button pin as an input
  pinMode(buttonPowerPin, OUTPUT);  // Set the button pin as an output
  digitalWrite(buttonPowerPin, HIGH);
  pinMode(buttonGroundPin, OUTPUT);  // Set the button pin as an output
  digitalWrite(buttonGroundPin, LOW);
  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Use 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Infinite loop if allocation fails
  }

  display.display();  // Display initialization




  // Display "Hello, world!" text
  display.setTextSize(4);               // Set text size (1 is default)
  display.setTextColor(SSD1306_WHITE);  // Set text color to white

  // Update the display with the text
  display.clearDisplay();
  display.display();
}

void loop() {

  buttonPressed = digitalRead(buttonReadPin);
  if (buttonPressed) {
    buttonPressedTimer++;
    buttonPressedResetTimer++;
    buttonReleasedTimer = 0;
  } else {
    buttonReleasedTimer++;
    buttonPressedTimer = 0;
  }

  if ((buttonPressedTimer > buttonPressedMinimumCount) && buttonPressed) {
    buttonHasBeenPressed = true;
    buttonPressedTimer = 0;
  }

  if ((buttonPressedResetTimer > countsPerSecond * 2) && buttonPressed) {
    secondCounter = 0;
    buttonPressedResetTimer = 0;
    buttonHasBeenPressed = false;
    displayTime();
  }

  if ((buttonReleasedTimer > buttonReleasedMinimumCount) && !buttonPressed) {
    if (buttonHasBeenPressed) {
      buttonCycled();
      buttonHasBeenPressed = false;
    }
    buttonReleasedTimer = 0;
  }
  if (pauseCounter) {
    pauseCounter--;
  } else {
    loopCounter++;
    if ((loopCounter > countsPerSecond) && (secondCounter > 0)) {
      secondCounter--;
      loopCounter = 0;
      displayTime();
    }
  }
}

void buttonCycled() {
  secondCounter += 60;
  secondCounter = secondCounter - (secondCounter % 60);
  if (secondCounter > 3600) {
    secondCounter = 3600;
  }
  displayTime();
  pauseCounter = countsPerSecond;
}

void displayTime() {
  display.clearDisplay();
  //debug();
  display.setCursor(0, 20);
  display.setTextSize(4);
  formatTime(secondCounter, minutesSecondsBuffer);
  display.println(minutesSecondsBuffer);
  display.display();
}

void debug() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(buttonPressedTimer);
  display.print(" ");
  display.print(buttonReleasedTimer);
  display.print(" ");
  display.print(buttonHasBeenPressed);
}
void formatTime(int totalSeconds, char *buffer) {
  int minutes = totalSeconds / 60;
  int seconds = totalSeconds % 60;
  sprintf(buffer, "%02d:%02d", minutes, seconds);
}
