#include <SPI.h>
#include <RF24.h>

/**
 *  Controller mode button pins
 */

#define button1 2
#define button2 3
#define button3 4
#define button4 5
#define button5 6

/**
 * Radio setup pins and address to send the data to
 */
const byte address[6] = "00001";
RF24 radio(9, 10);
bool radioReady = false;
int currentMode = 1;

/**
 * setup analogstick/joystick pins
 */
#define joyX A0
#define joyY A1

int xValue, yValue;

void setup() {
 
  // Initialise Serial first
  Serial.begin(9600);  // Baud rate
  // Pin modes
  // Configure the Radio
  setupButtons();
  setupRadio();
}

void setupButtons() {
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
}

void setupRadio() {
  int failCount = 0;
  while (!radioReady && failCount < 10) {
    radioReady = radio.begin();
    if (!radioReady) {
      failCount++;
      Serial.print("Radio Initialisation Failed ");
      Serial.print(failCount);
      Serial.println(" times.");
    }
  }
  if (!radioReady) {
    Serial.println("Radio could not be initialised!");
    return;
  } else {
    Serial.println("Radio Ready!");
  }
  radio.setPALevel(RF24_PA_MIN); // Maximum Range
  radio.openWritingPipe(address); // For Outgoing Data
  radio.stopListening();
}

void loop() {

  if (digitalRead(button1)) {
    currentMode = 1;
    Serial.println("Button 1 PRESSED");
  } else if (digitalRead(button2)) {
    currentMode = 2;
    Serial.println("Button 2 PRESSED");
  } else if (digitalRead(button3)) {
    currentMode = 3;
    Serial.println("Button 3 PRESSED");
  } else if (digitalRead(button4)) {
    currentMode = 4;
    Serial.println("Button 4 PRESSED");
  } else if (digitalRead(button5)) {
    currentMode = 5;
    Serial.println("Button 5 PRESSED");
  }

/**
 * Read the value out from the joysticks and create a command for the buggy in the following format:
 * M=1X=1Y=1
 * 
 * M = 1-5
 * X = 1-9 (4 = center)
 * Y = 1-9 (4 = center)
 */

  xValue = analogRead(joyX);
  yValue = analogRead(joyY);

  int x1 = map(xValue, 0, 1023, 0, 9);  // 0 is our minimum speed and 9 is the maximam
  int y1 = map(yValue, 0, 1023, 0, 9);  

  String command = "M=" + String(currentMode) + "X=" + String(x1) + "Y=" + String(y1);
  Serial.println(command);
  int command_len = command.length() + 1;
  char command_array[command_len];
  command.toCharArray(command_array, command_len);
  radio.write(&command_array, command_len);
}