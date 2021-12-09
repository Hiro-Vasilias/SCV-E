
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <Servo.h>

const byte address[6] = "00001"; // address of the radio to receive the data on
RF24 radio(10, 11);
bool radioReady = false;
int mode = 1;                    // The default controller mode is set to 1. This can be changed by pressing other buttons on the remote.

String directionX = "CENTRE";    // Default analog stick position on the X axis
int directionXSpeed = 0;         // The speed of that direction. e.g a bit to the left will be speed 1. More to the left will increase the speed. Max speed is 4.
String directionY = "CENTRE";    // Default analog stick position on the Y axis
int directionYSpeed = 0;         // The speed of that direction. e.g a bit downwards will be speed 1. Fully downwards stick will increase the speed. Max speed is 4.

Servo craneservo;
Servo magnetropeservo;

bool magnet_state = false; //(FALSE IS OFF)

// Components output pins on the arduino Board
#define MAGNET_INPUT 26

#define LINIER_ACTUATOR_A 36
#define LINIER_ACTUATOR_PIN_1 30
#define LINIER_ACTUATOR_PIN_2 28

//#define MOTOR_RIGHT_TOP_A 13
#define MOTOR_RIGHT_TOP_1 37
#define MOTOR_RIGHT_TOP_2 35

#define MOTOR_RIGHT_BOTTOM_1 49
#define MOTOR_RIGHT_BOTTOM_2 45

#define MOTOR_LEFT_TOP_1 43
#define MOTOR_LEFT_TOP_2 47

#define MOTOR_LEFT_BOTTOM_1 39
#define MOTOR_LEFT_BOTTOM_2 41

int pos = 0;        // variable to store the crane servo position
int ropepos = 0;    // variable to store the magnet rope servo position

void setup() {
  // Initialise Serial first
  Serial.begin(9600);  // Baud rate

  /**
     The functions below allows to setup the components with their respective pins. It sets also the pins to their default state
  */
  setupRadio();
  setupMotors();
  setupActuator();
  setupServo();
  setupMagnet();

}

void setupServo() {

  // Setting up pins for the servos
  craneservo.attach(22);
  magnetropeservo.attach(24);
}

void setupRadio() {
  // Pin modes
  // Configure the Radio
  int failCount = 0;
  while (!radioReady && failCount < 10) {
    radioReady = radio.begin();
    if (!radioReady) {
      failCount++;
      Serial.print("Radio Initialisation Failed ");
      Serial.print(failCount);
      Serial.println(" times.");
    } else {
      Serial.println(" radio ready.");
    }
  }
  radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.startListening();              //Setting the radio in the receiving mode
}


void setupMotors()
{

  /**
     setup motors on their respective pins and set the initial values to off
  */

  pinMode(MOTOR_RIGHT_TOP_1, OUTPUT);
  pinMode(MOTOR_RIGHT_TOP_2, OUTPUT);
  digitalWrite(MOTOR_RIGHT_TOP_1, LOW);
  digitalWrite(MOTOR_RIGHT_TOP_2, LOW);


  pinMode(MOTOR_RIGHT_BOTTOM_1, OUTPUT);
  pinMode(MOTOR_RIGHT_BOTTOM_2, OUTPUT);
  digitalWrite(MOTOR_RIGHT_BOTTOM_1, LOW);
  digitalWrite(MOTOR_RIGHT_BOTTOM_2, LOW);

  pinMode(MOTOR_LEFT_TOP_1, OUTPUT);
  pinMode(MOTOR_LEFT_TOP_2, OUTPUT);
  digitalWrite(MOTOR_LEFT_TOP_1, LOW);
  digitalWrite(MOTOR_LEFT_TOP_2, LOW);

  pinMode(MOTOR_LEFT_BOTTOM_1, OUTPUT);
  pinMode(MOTOR_LEFT_BOTTOM_2, OUTPUT);
  digitalWrite(MOTOR_LEFT_BOTTOM_1, LOW);
  digitalWrite(MOTOR_LEFT_BOTTOM_2, LOW);

}

void setupActuator()
{
  /**
    setup the actuator on the correct pins
  */

  pinMode(LINIER_ACTUATOR_A, OUTPUT);
  pinMode(LINIER_ACTUATOR_PIN_1, OUTPUT);
  pinMode(LINIER_ACTUATOR_PIN_2, OUTPUT);
  digitalWrite(LINIER_ACTUATOR_PIN_1, LOW);
  digitalWrite(LINIER_ACTUATOR_PIN_2, LOW);
}

void setupMagnet()
{
  pinMode(MAGNET_INPUT, OUTPUT);
}

void loop()
{
  readCommandsFromRadio();
  delay(5);

  // For debugging persos to understand what is being received from the controller
  printControllerValues();


  // Allow only one mode at a time because of a single analog stick. mode will be set depending on what is received from the controller

  if (mode == 1) {
    controlBuggy();
  } else if (mode == 2) {
    controlCraneArm();
  } else if (mode == 3) {
    controlCraneHeight();
  } else if (mode == 4) {
    controlCraneRope();
  } else if (mode == 5) {
    toggleMagnet();
  }

}

void toggleMagnet() {

   // If magenet_state = false (off) turn it on. Else turn it off
    if (magnet_state = false) {
      magnet_state = true;
      digitalWrite(MAGNET_INPUT, HIGH);
    } else {
      magent_state = false;
      digitalWrite(MAGNET_INPUT, LOW);
    }
}
void controlCraneArm() {

  if (directionY == "UP") {

    // Telling the servo to go up a bit
    for (int x = 0; x < 10; x++) {
      pos = pos + 1;
      craneservo.write(pos);
      Serial.println("Crane Servo Up");
      Serial.println(pos);
      delay(20);
    }
  } else if (directionY == "DOWN") {

     // Telling the crane servo to go down

    for (int y = 10; y > 0; y--) {
      pos = pos - 1;
      craneservo.write(pos);
      Serial.println("Crane Servo Down");
      Serial.println(pos);
      delay(20);
    }
  }
}

void controlCraneRope() {

      // Telling the servo to go up a bit

  if (directionY == "UP") {
    for (int x = 0; x < 10; x++) {
      ropepos = ropepos + 1;
      magnetropeservo.write(ropepos);
      Serial.println("UP");
      Serial.println(ropepos);
      delay(20);
    }
  } else if (directionY == "DOWN") {

      // Telling the crane servo to go down

    for (int y = 10; y > 0; y--) {
      ropepos = ropepos - 1;
      magnetropeservo.write(ropepos);
      delay(20);
      Serial.println("Down");
      Serial.println(ropepos);
    }
  }
}

void controlCraneHeight() {

  if (directionY == "UP") {
    pinMode(LINIER_ACTUATOR_A, HIGH);

    digitalWrite(LINIER_ACTUATOR_PIN_1, HIGH);
    digitalWrite(LINIER_ACTUATOR_PIN_2, LOW);

    delay(20);
  } else if (directionY == "DOWN") {
    pinMode(LINIER_ACTUATOR_A, HIGH);

    digitalWrite(LINIER_ACTUATOR_PIN_1, LOW);
    digitalWrite(LINIER_ACTUATOR_PIN_2, HIGH);

    delay(20);
  } else {
    pinMode(LINIER_ACTUATOR_A, LOW);

    digitalWrite(LINIER_ACTUATOR_PIN_1, LOW);
    digitalWrite(LINIER_ACTUATOR_PIN_2, LOW);
  }

}


void controlBuggy() {

  /**
   * The movement of the buggy works like as a tank. e.g to go right we go forwards with the left motors and backwards with the right ones.
   */
  if (directionX == "LEFT") {
    forwardRightMotors();
    backwardsLeftMotors();
    delay(20);
  } else if (directionX == "RIGHT") {

    backwardsRightMotors();
    forwardLeftMotors();

    delay(20);
  } else if (directionY == "UP") {
    forwardRightMotors();
    forwardLeftMotors();
    delay(20);
  } else if (directionY == "DOWN") {
    backwardsRightMotors();
    backwardsLeftMotors();

    delay(20);
  } else {

    /**
     * if no direction is give we put it to a stop
     */
    digitalWrite(MOTOR_RIGHT_TOP_1, LOW);
    digitalWrite(MOTOR_RIGHT_TOP_2, LOW);
    digitalWrite(MOTOR_RIGHT_BOTTOM_1, LOW);
    digitalWrite(MOTOR_RIGHT_BOTTOM_2, LOW);
    digitalWrite(MOTOR_LEFT_TOP_1, LOW);
    digitalWrite(MOTOR_LEFT_TOP_2, LOW);
    digitalWrite(MOTOR_LEFT_BOTTOM_1, LOW);
    digitalWrite(MOTOR_LEFT_BOTTOM_2, LOW);
  }

}

void forwardRightMotors() {

  digitalWrite(MOTOR_RIGHT_TOP_1, LOW);
  digitalWrite(MOTOR_RIGHT_TOP_2, HIGH);
  digitalWrite(MOTOR_RIGHT_BOTTOM_1, LOW);
  digitalWrite(MOTOR_RIGHT_BOTTOM_2, HIGH);

}

void forwardLeftMotors() {

  digitalWrite(MOTOR_LEFT_TOP_1, LOW);
  digitalWrite(MOTOR_LEFT_TOP_2, HIGH);
  digitalWrite(MOTOR_LEFT_BOTTOM_1, LOW);
  digitalWrite(MOTOR_LEFT_BOTTOM_2, HIGH);

}


void backwardsRightMotors() {

  digitalWrite(MOTOR_RIGHT_TOP_1, HIGH);
  digitalWrite(MOTOR_RIGHT_TOP_2, LOW);
  digitalWrite(MOTOR_RIGHT_BOTTOM_1, HIGH);
  digitalWrite(MOTOR_RIGHT_BOTTOM_2, LOW);

}

void backwardsLeftMotors() {

  digitalWrite(MOTOR_LEFT_TOP_1, HIGH);
  digitalWrite(MOTOR_LEFT_TOP_2, LOW);
  digitalWrite(MOTOR_LEFT_BOTTOM_1, HIGH);
  digitalWrite(MOTOR_LEFT_BOTTOM_2, LOW);

}

void readCommandsFromRadio() {
  if (radio.available())              //Looking for the data.
  {
    char text[32] = "";                 //Saving the incoming data
    radio.read(&text, sizeof(text));
    String stringCommand = String(text);

    /**
     * Only set the mode if the command start with an M. 
     * The controller send the command in the following format M=1X=1Y=1
     * 
     * M can be from 1 to 5 which stand for MODE
     * X = the potensiometer value from the analog stick. Values can be 1-9 (4 = CENTER)
     * Y = the potensiometer value from the analog stick. Values can be 1-9 (4 = CENTER)
     */
    if (stringCommand.charAt(0) == 'M') {  

      // MODE
      setMode(stringCommand.charAt(2) - '0'); // Read the THIRD charactar of the command.  "- '0'" is to convert the charactar into an integer
      setDirectionAndSpeedX((stringCommand.charAt(5) - '0')); // Read the 6TH charactar of the command.  "- '0'" is to convert the charactar into an integer
      setDirectionAndSpeedY(stringCommand.charAt(8) - '0'); //Read the 9TH
    }
  }
}

void setMode(int mode) {
  mode = mode;
}

void setDirectionAndSpeedX(int x) {

  /**
   *  The analog sticks potentio meters send a value from 1-9 and. 4 is the centre value. 
   *  Set the speed and the direction based on the value
   */
  
  if (x > 4) {
    directionX = "LEFT";
    directionXSpeed = map(x, 4, 9, 0, 4);
  } else if (x < 4)  {
    directionX = "RIGHT";
    directionXSpeed = map(x, 4, 0, 0, 4);
  } else {
    directionX = "CENTRE";
    directionXSpeed = 0;
  }

  delay(20);
}

void setDirectionAndSpeedY(int y) {

    /**
   *  The analog sticks potentio meters send a value from 1-9 and. 4 is the centre value. 
   *  Set the speed and the direction based on the value
   */
   
  if (y > 4) {
    directionY = "UP";
    directionYSpeed = map(y, 4, 9, 0, 4);
  } else if (y < 4)  {
    directionY = "DOWN";
    directionYSpeed = map(y, 4, 0, 0, 4);
  } else {
    directionY = "CENTRE";
    directionYSpeed = 0;
  }
  delay(20);
}
void printControllerValues() {
  Serial.println("MODE: " + String(mode) + " Direction: " + directionY + directionX + " XSpeed: " + directionXSpeed + " YSpeed: " + directionYSpeed);
}