#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int buttonPowerPin = 2;   //to button non resistor side
const int buttonReadPin = 3;    // Pin connected to the button and resistor
const int buttonGroundPin = 4;  //To resistor only
const int solidStateRelayPin = 13;
// Define screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define I2C address
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const long countsPerSecond = 88235;
long loopCounter = 0;
int secondCounter = 0;
bool buttonPressed = false;  // Variable for reading the button status
int buttonPressedTimer = 0;
long buttonPressedResetTimer = 0;
const int buttonPressedMinimumCount = 1000;
int buttonReleasedTimer = 0;
int buttonReleasedMinimumCount = buttonPressedMinimumCount;
bool buttonHasBeenPressed = false;
bool resetJustHappened = false;
long pauseCounter = 0;
char minutesSecondsBuffer[6];
const int inductionOffTimeSeconds = 5;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  pinMode(buttonReadPin, INPUT);  // Set the button pin as an input
  pinMode(buttonPowerPin, OUTPUT);
  digitalWrite(buttonPowerPin, HIGH);
  pinMode(solidStateRelayPin, OUTPUT);  // Set the button pin as an output
  digitalWrite(solidStateRelayPin, HIGH);
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
  displayTime();
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
    resetJustHappened = true;
    displayTime();
  }

  if ((buttonReleasedTimer > buttonReleasedMinimumCount) && !buttonPressed) {
    if (buttonHasBeenPressed) {
      if (!resetJustHappened) {
        buttonCycled();
      } else {
        resetJustHappened = false;
      }
      buttonHasBeenPressed = false;
    }
    buttonReleasedTimer = 0;
  }
  if (pauseCounter > 0) {
    pauseCounter--;
  } else {
    pauseCounter = 0;
    loopCounter++;

    if ((secondCounter > 0)) {
      secondCounter--;
      if (secondCounter == 0) {
        digitalWrite(solidStateRelayPin, LOW);
        delay(inductionOffTimeSeconds * 1000);
        digitalWrite(solidStateRelayPin, HIGH);
      }
    }


    if (loopCounter > countsPerSecond) {
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
