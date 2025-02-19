#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>
#include "Gesture.h"
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

int changed = 2;
// DFPlayer
HardwareSerial mySerial(1);  // Use hardware serial
DFRobotDFPlayerMini myDFPlayer;

// Servo motors
Servo myservo;
Servo myservoTwo;
Servo myservoThree;
Servo myservoFour;
Servo myservoFive;
Servo myservoSix;
Servo servoAntenna1;
Servo servoAntenna2;

// Servo blackServo;

// Pin to control the servo
const int servo_pin = 5; // Right shoulder1
const int servo_pin_two = 18; // Left shoulder1
const int servo_pin_thr = 4; // Right shoulder2
const int servo_pin4 = 13; // Right elbow
const int servo_pin5 = 12; // Left shoulder2
const int servo_pin6 = 19; // Left elbow
const int servo_pina1 = 25; // Antenna
const int servo_pina2 = 14; // Antenna

// const int servo_pin_bla = 22;

int val;
int react;

// Stepper motor pins
const int step_pin = 33;
const int direction_pin = 27;

const int STEPS_PER_TURN = 200;

const int delay_between_step_microsec = 5000;

// Touch sensor pin
// const int Sensor = 26; 
const int Sensor = 26; // digital input

int valSensor;
bool touchState = false;
bool motorState = false;
bool gestureState = false;

#define GES_REACTION_TIME  500
#define GES_QUIT_TIME   1000

paj7620 Gesture;

//LED display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//music index
int music = 1;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  // Initialize I2C for gesture sensor
  Wire.begin();
  // paj7620Init();

  pinMode(step_pin, OUTPUT);
  pinMode(direction_pin, OUTPUT);

  myservo.attach(servo_pin, 500, 2500);
  myservoTwo.attach(servo_pin_two, 500, 2500);
  myservoThree.attach(servo_pin_thr, 500, 2500);
  myservoFour.attach(servo_pin4, 500, 2500);
  myservoFive.attach(servo_pin5, 500, 2500);
  myservoSix.attach(servo_pin6, 500, 2500);
  servoAntenna1.attach(servo_pina1, 500, 2500);
  servoAntenna2.attach(servo_pina2, 500, 2500);

  // Initial angle
  myservoTwo.write(150);
  myservoFive.write(180);
  myservoSix.write(180); 

  myservo.write(30);
  myservoThree.write(0);
  myservoFour.write(0); 

  servoAntenna1.write(90);
  servoAntenna2.write(90);
  

  // blackServo.attach(servo_pin_bla, 500, 2500);

  pinMode(Sensor, INPUT);

  if (!myDFPlayer.begin(mySerial)) {  
    Serial.println("Unable to begin:");
    Serial.println("1. Please recheck the connection!");
    Serial.println("2. Please insert the SD card or USB!");
    while (true);
  }
  Serial.println("DFPlayer Mini online.");

  myDFPlayer.setTimeOut(500);                       // Set serial communication timeout
  myDFPlayer.volume(30);                            // Set volume (0~30)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);                // Set EQ to normal
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);  // Set device to USB


  while(!Serial) {
      delay(100);
  }
  Serial.println("\nPAJ7620U2 TEST DEMO: Recognize 15 gestures.");
  
  if(Gesture.init()) {
      Serial.println("PAJ7620U2 initialization failed");
  } else {
      Serial.println("PAJ7620U2 initialization success");
  }
  Serial.println("Please input your gestures:\n");

}



using namespace std;

int chooseRandomNumber(int input) {
    // Ensure the input is within the range of 1-6
    if (input < 1 || input > 11) {
        cerr << "Error: Input must be between 1 and 6." << endl;
        return -1;
    }

    vector<int> options; // To store the possible choices based on the input

    // Populate the options based on the input number
    if (input == 1 || input == 3) {
        options = {5, 7, 9, 11}; // Options when input is 1 or 2
    } else if (input == 5 || input == 7) {
        options = {1, 3, 9, 11}; // Options when input is 3 or 4
    } else if (input == 9 || input == 11) {
        options = {1, 3, 5, 7}; // Options when input is 5 or 6
    }

    // Choose a random number from the options
    srand(time(0)); // Seed for random number generation
    int randomIndex = rand() % options.size();
    return options[randomIndex];
}



void step(bool forward) {
  if (forward == true) {
    digitalWrite(direction_pin, HIGH);
  } else {
    digitalWrite(direction_pin, LOW);
  }

  digitalWrite(step_pin, HIGH);

  digitalWrite(step_pin, LOW);
}


void steps(int number_of_steps) {
  bool move_forward = true;

  if (number_of_steps >= 0) {
    move_forward = true;
  } else {
    move_forward = false;
    number_of_steps = -number_of_steps;
  }

  for (int i = 0; i < number_of_steps; i++) {
    step(move_forward);
    delayMicroseconds(delay_between_step_microsec);
  }
}

void setArmsForward() {
  // Left Arm
  myservoTwo.write(90);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  myservoSix.write(90);  // Left Elbow 180

  // Right Arm
  myservo.write(90);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  myservoFour.write(90);      // Right Elbow 0

  delay(1000);
  // note: fixed
}

void setArmsDown() {
  // Left Arm
  myservoTwo.write(180);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Arm
  myservo.write(0);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(1000);
  // note: fixed
}

void setArmsOut() {
  // Left Arm
  myservoSix.write(90);   // Left Shoulder1
  myservoThree.write(90); // Left Shoulder2
  myservoFour.write(0);   // Left Elbow

  // Right Arm
  myservoTwo.write(90);   // Right Shoulder1
  myservoFive.write(90);  // Right Shoulder2
  myservo.write(0);       // Right Elbow
  delay(1000);
}

void setSalute() {
  // Left Arm
  myservoTwo.write(150);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Arm
  myservo.write(140);   // Right Shoulder1 0
  myservoThree.write(40);    // Right Shoulder2 0
  delay(400);
  myservoFour.write(120);      // Right Elbow 0

  delay(1000);

  myservo.write(30);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(1000);
}



void setWave1() {
  // Left Arm wave
  myservoTwo.write(50);   // Left Shoulder1 180
  myservoFive.write(90);  // Left Shoulder2 180
  myservoSix.write(90);  // Left Elbow 180
  // Right
  myservo.write(30);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(500);

  myservoSix.write(45);  // Left Elbow
  delay(500);
  myservoSix.write(135);
  delay(500);
  myservoSix.write(45);  // Left Elbow
  delay(500);
  myservoSix.write(135);
  delay(500);
  myservoSix.write(45);  // Left Elbow
  delay(500);
  myservoSix.write(135);
  delay(500);
  myservoSix.write(45);  // Left Elbow
  delay(500);
  myservoSix.write(135);
  delay(500);

  // Left Arm
  myservoTwo.write(150);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180
  delay(1000);

  // note: fixed
}

void setWave2() {
  // Right Arm wave
  myservoTwo.write(150);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180
  // Left
  myservo.write(140);   // Right Shoulder1 0
  myservoThree.write(90);    // Right Shoulder2 0
  myservoFour.write(90);      // Right Elbow 0

  delay(500);

  myservoFour.write(135); // Left Elbow
  delay(500);
  myservoFour.write(45);  
  delay(500);
  myservoFour.write(135);
  delay(500);
  myservoFour.write(45);  // Left Elbow
  delay(500);
  myservoFour.write(135); // Left Elbow
  delay(500);
  myservoFour.write(45);  
  delay(500);
  myservoFour.write(135);
  delay(500);
  myservoFour.write(45);  // Left Elbow
  delay(500);

  // Left
  myservo.write(30);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0
  delay(1000);

  // note: fixed
}

void setWalk1(){
  // Left forward, right backward
  // Left Arm
  myservoTwo.write(90);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  
  // Right Arm
  myservoFour.write(0);      // Right Elbow 0
  myservo.write(0);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  delay(200);

  myservoSix.write(90);  // Left Elbow 180
  delay(800);

  // note: fixed
}

void setWalk2(){
  // Right forward, left backward
  // Left Arm
  myservoSix.write(180);  // Left Elbow 180
  myservoTwo.write(180);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  
  // Right Arm
  myservo.write(90);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  
  delay(200);
  myservoFour.write(90);      // Right Elbow 0
  delay(1000);

  // note: fixed
}


void setWalk() {
  setWalk1();
  setWalk2();

  // note: fixed
}


void setArmsUp() {
  // Left Arm
  myservoSix.write(90);    // Left Shoulder1
  myservoThree.write(45);  // Left Shoulder2
  myservoFour.write(90);   // Left Elbow

  // Right Arm
  myservoTwo.write(90);    // Right Shoulder1
  myservoFive.write(135);  // Right Shoulder2
  myservo.write(90);       // Right Elbow
  delay(1000);
}

void setBothArmsUpAndDown() {
  // Up
  // Left Arm
  myservoTwo.write(0);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Arm
  myservo.write(180);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(1000);

  // Down
  // Left Arm
  myservoTwo.write(180);   // Left Shoulder1 180
  myservoFive.write(180);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Arm
  myservo.write(0);   // Right Shoulder1 0
  myservoThree.write(0);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(1000);

  // note: fixed
}

void dance1() {
  // 1: Left Up
  myservoTwo.write(90);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(30);  // Left Elbow 180

  // Right Down
  myservo.write(0);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(1000);

  // Left Down
  myservoTwo.write(180);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Up
  myservo.write(90);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(150);      // Right Elbow 0

  delay(1000);

  // Left Up
  myservoTwo.write(90);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(30);  // Left Elbow 180

  // Right Down
  myservo.write(0);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(500);

  // Left Down
  myservoTwo.write(180);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Up
  myservo.write(90);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(150);      // Right Elbow 0

  delay(500);

  // Left Up
  myservoTwo.write(90);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(30);  // Left Elbow 180

  // Right Down
  myservo.write(0);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(1000);

  // 2: Left Down
  myservoTwo.write(180);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Up
  myservo.write(90);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(150);      // Right Elbow 0

  delay(1000);

  // Left Up
  myservoTwo.write(90);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(30);  // Left Elbow 180

  // Right Down
  myservo.write(0);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(1000);

  // Left Down
  myservoTwo.write(180);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Up
  myservo.write(90);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(150);      // Right Elbow 0

  delay(500);

  // Left Up
  myservoTwo.write(90);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(30);  // Left Elbow 180

  // Right Down
  myservo.write(0);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(0);      // Right Elbow 0

  delay(500);

  // Left Down
  myservoTwo.write(180);   // Left Shoulder1 180
  myservoFive.write(160);  // Left Shoulder2 180
  myservoSix.write(180);  // Left Elbow 180

  // Right Up
  myservo.write(90);   // Right Shoulder1 0
  myservoThree.write(20);    // Right Shoulder2 0
  myservoFour.write(150);      // Right Elbow 0

  delay(1000);

  // note: fixed
}

void setClap() {
  // Bring arms together for a clap
  for (int i = 0; i < 3; i++) {
    // Left Arm
    myservoSix.write(60);    // Left Shoulder1
    myservoThree.write(60);  // Left Shoulder2
    myservoFour.write(90);   // Left Elbow

    // Right Arm
    myservoTwo.write(120);   // Right Shoulder1
    myservoFive.write(120);  // Right Shoulder2
    myservo.write(90);       // Right Elbow
    delay(500);

    // Open arms
    myservoSix.write(0);    // Left Shoulder1
    myservoThree.write(0);  // Left Shoulder2
    myservoFour.write(0);   // Left Elbow

    myservoTwo.write(180);   // Right Shoulder1
    myservoFive.write(180);  // Right Shoulder2
    myservo.write(180);      // Right Elbow
    delay(500);
  }
}

void setPointForward() {
  // Left Arm pointing forward
  myservoSix.write(45);    // Left Shoulder1
  myservoThree.write(0);   // Left Shoulder2
  myservoFour.write(180);  // Left Elbow fully extended

  // Right Arm in rest position
  myservoTwo.write(180);   // Right Shoulder1
  myservoFive.write(180);  // Right Shoulder2
  myservo.write(180);      // Right Elbow
  delay(1000);
}

void setStretch() {
  // Both arms stretched upward
  myservoSix.write(0);    // Left Shoulder1
  myservoThree.write(0);  // Left Shoulder2
  myservoFour.write(0);   // Left Elbow

  myservoTwo.write(180);  // Right Shoulder1
  myservoFive.write(0);   // Right Shoulder2
  myservo.write(0);       // Right Elbow
  delay(1000);
}

void setRotateArms() {
  // Simulate a rotating motion with both arms
  for (int i = 0; i < 3; i++) {
    // Left Arm rotates
    myservoSix.write(90);    // Left Shoulder1
    myservoThree.write(45);  // Left Shoulder2
    myservoFour.write(90);   // Left Elbow

    // Right Arm rotates
    myservoTwo.write(90);    // Right Shoulder1
    myservoFive.write(135);  // Right Shoulder2
    myservo.write(90);       // Right Elbow
    delay(500);

    // Reset
    myservoSix.write(0);    // Left Shoulder1
    myservoThree.write(0);  // Left Shoulder2
    myservoFour.write(0);   // Left Elbow

    myservoTwo.write(180);   // Right Shoulder1
    myservoFive.write(180);  // Right Shoulder2
    myservo.write(180);      // Right Elbow
    delay(500);
  }
}

// void set

void set0() {
  // Initial angle
  // Left Arm
  myservoTwo.write(0);   // Left Shoulder1
  delay(1000);
  myservoTwo.write(180);  
  delay(2000);

  // myservoFive.write(0);  // Left Shoulder2
  myservoFive.write(0);  
  delay(1000);
  myservoFive.write(180);  
  delay(2000);

  // myservoSix.write(45);  // Left Elbow
  myservoSix.write(0);  
  delay(1000);
  myservoSix.write(180);   
  delay(2000);

  // // Right Arm
  // myservo.write(90);   // Right Shoulder1
  myservo.write(0);  
  delay(1000);
  myservo.write(180);   
  delay(2000);
  // myservoThree.write(0);   // Right Shoulder2
  myservoThree.write(0);  
  delay(1000);
  myservoThree.write(180);   
  delay(2000);
  // myservoFour.write(90);      // Right Elbow
  myservoFour.write(0);  
  delay(1000);
  myservoFour.write(180);   
  delay(2000);
}


void danceAllIWant() {
  // steps(50);
  // // delay(350);
  // delay(1000);
  // steps(-50);
  // delay(350);

  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(400);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(400);

  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(400);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(400);

  // **1. Intro (Slow swaying movements)**
  // Left arm sways up
  myservoTwo.write(90);   // Left Shoulder1
  myservoFive.write(160); // Left Shoulder2
  myservoSix.write(45);   // Left Elbow

  // Right arm sways down
  myservo.write(30);      // Right Shoulder1
  myservoThree.write(20); // Right Shoulder2
  myservoFour.write(90);  // Right Elbow
  delay(900);

  // Left arm sways down, Right arm sways up
  myservoTwo.write(180);   // Left Shoulder1
  myservoFive.write(160);  // Left Shoulder2
  myservoSix.write(180);   // Left Elbow
  myservo.write(90);       // Right Shoulder1
  myservoThree.write(30);  // Right Shoulder2
  myservoFour.write(150);  // Right Elbow
  delay(900);

servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(400);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(400);

  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(400);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(400);
  // **2. Verse (Playful alternation)**
  for (int i = 0; i < 2; i++) { // Loop for rhythmic alternation
    // Left up, Right down
    myservoTwo.write(90);
    myservoFive.write(160);
    myservoSix.write(30);
    myservo.write(30);
    myservoThree.write(20);
    myservoFour.write(0);
    delay(400);

    // Left down, Right up
    myservoTwo.write(180);
    myservoFive.write(160);
    myservoSix.write(180);
    myservo.write(90);
    myservoThree.write(30);
    myservoFour.write(150);
    delay(400);
  }

  // **3. Chorus (Energetic movements)**
  // Both arms up and waving
  for (int i = 0; i < 4; i++) {
    myservoTwo.write(90);   // Left Shoulder1
    myservoFive.write(160); // Left Shoulder2
    myservoSix.write(45);   // Left Elbow
    myservo.write(90);      // Right Shoulder1
    myservoThree.write(160); // Right Shoulder2
    myservoFour.write(45);  // Right Elbow
    delay(400);

    // Both arms outstretched
    myservoTwo.write(180);  // Left Shoulder1
    myservoFive.write(120); // Left Shoulder2
    myservoSix.write(0);    // Left Elbow
    myservo.write(0);       // Right Shoulder1
    myservoThree.write(120); // Right Shoulder2
    myservoFour.write(0);   // Right Elbow
    delay(400);
  }
  
  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(400);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(400);

  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(400);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(400);

  // **4. Bridge (Expressive wave-like motions)**
  for (int i = 0; i < 2; i++) {
    // Wave with left arm
    myservoTwo.write(90);
    myservoFive.write(160);
    myservoSix.write(45);
    delay(400);

    myservoSix.write(90);
    delay(400);

    // Wave with right arm
    myservo.write(90);
    myservoThree.write(160);
    myservoFour.write(45);
    delay(400);

    myservoFour.write(90);
    delay(400);
  }

  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(400);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(400);

  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(400);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(400);

  // **5. Finale (Big finish with both arms up)**
  myservoTwo.write(90);   // Left Shoulder1
  myservoFive.write(160); // Left Shoulder2
  myservoSix.write(30);   // Left Elbow
  myservo.write(90);      // Right Shoulder1
  myservoThree.write(160); // Right Shoulder2
  myservoFour.write(30);  // Right Elbow
  delay(900);

  // Arms down to rest position
  myservoTwo.write(180);   // Left Shoulder1
  myservoFive.write(160);  // Left Shoulder2
  myservoSix.write(180);   // Left Elbow
  myservo.write(30);       // Right Shoulder1
  myservoThree.write(20);  // Right Shoulder2
  myservoFour.write(180);  // Right Elbow
  delay(900);
}



void danceTryEverything() {
  steps(50);
  delay(45);
  steps(-50);
  delay(45);

  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(350);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(350);
  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(350);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(250);
  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(1200);

  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(350);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(350);
  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(350);

  servoAntenna1.write(80);
  servoAntenna2.write(80);

  delay(250);
  servoAntenna1.write(100);
  servoAntenna2.write(100);

  delay(250);

  // **1. Intro (Upbeat Swaying Movement)**
  // Left arm sways up, Right arm sways down
  myservoTwo.write(90);   // Left Shoulder1
  myservoFive.write(160); // Left Shoulder2
  myservoSix.write(45);   // Left Elbow

  myservo.write(30);      // Right Shoulder1
  myservoThree.write(20); // Right Shoulder2
  myservoFour.write(0);   // Right Elbow
  delay(1000);

  // Switch positions
  myservoTwo.write(180);  // Left Shoulder1
  myservoFive.write(140); // Left Shoulder2
  myservoSix.write(180);  // Left Elbow

  myservo.write(90);      // Right Shoulder1
  myservoThree.write(160); // Right Shoulder2
  myservoFour.write(45);  // Right Elbow
  delay(1000);


  // **2. Verse (Playful Alternation)**
  for (int i = 0; i < 3; i++) { // Loop for rhythmic alternation
    // Left arm punch up
    myservoTwo.write(60);
    myservoFive.write(160);
    myservoSix.write(30);

    // Right arm punch down
    myservo.write(0);
    myservoThree.write(20);
    myservoFour.write(180);
    delay(500);

    // Switch arms
    myservoTwo.write(180); // Left arm down
    myservoFive.write(160);
    myservoSix.write(180);

    myservo.write(90); // Right arm up
    myservoThree.write(160);
    myservoFour.write(30);
    delay(500);
  }
  servoAntenna1.write(100);
  servoAntenna2.write(80);

  delay(450);

  servoAntenna1.write(80);
  servoAntenna2.write(100);

  delay(450);
  servoAntenna1.write(100);
  servoAntenna2.write(80);

  delay(450);

  servoAntenna1.write(80);
  servoAntenna2.write(100);

  delay(450);

  // **3. Chorus (Energetic Wave Movements)**
  for (int i = 0; i < 4; i++) { // Loop for wave-like motion
    // Left arm waves up
    myservoTwo.write(90);
    myservoFive.write(140);
    myservoSix.write(45);
    delay(300);

    myservoSix.write(90);
    delay(300);

    // Right arm waves up
    myservo.write(90);
    myservoThree.write(160);
    myservoFour.write(45);
    delay(300);

    myservoFour.write(90);
    delay(300);
  }

  // **4. Bridge (Circular Movements)**
  for (int i = 0; i < 2; i++) {
    // Left arm circular motion
    myservoTwo.write(90);  // Arm forward
    myservoFive.write(160);
    myservoSix.write(45);
    delay(500);

    myservoTwo.write(140); // Arm backward
    myservoFive.write(140);
    myservoSix.write(90);
    delay(500);

    // Right arm circular motion
    myservo.write(60);  // Arm forward
    myservoThree.write(160);
    myservoFour.write(45);
    delay(500);

    myservo.write(90); // Arm backward
    myservoThree.write(140);
    myservoFour.write(90);
    delay(500);
  }

  // **5. Finale (Both Arms Up and Down)**
  for (int i = 0; i < 4; i++) {
    // Both arms up
    myservoTwo.write(90);   // Left Shoulder1
    myservoFive.write(160); // Left Shoulder2
    myservoSix.write(30);   // Left Elbow

    myservo.write(90);      // Right Shoulder1
    myservoThree.write(160); // Right Shoulder2
    myservoFour.write(30);  // Right Elbow
    delay(500);

    // Both arms down
    myservoTwo.write(180);  // Left Shoulder1
    myservoFive.write(140); // Left Shoulder2
    myservoSix.write(180);  // Left Elbow

    myservo.write(30);      // Right Shoulder1
    myservoThree.write(140); // Right Shoulder2
    myservoFour.write(180); // Right Elbow
    delay(500);
  }
}



void danceFeelTheLove() {
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1000);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1000);

  // **1. Intro (Gentle Swaying Movements)**
  // Start with both arms down, then lift slightly
  myservoTwo.write(180);   // Left Shoulder1
  myservoFive.write(180);  // Left Shoulder2
  myservoSix.write(180);   // Left Elbow

  myservo.write(0);        // Right Shoulder1
  myservoThree.write(0);   // Right Shoulder2
  myservoFour.write(0);    // Right Elbow
  delay(1500);

  // Lift both arms partially
  myservoTwo.write(90);    // Left Shoulder1
  myservoFive.write(160);  // Left Shoulder2
  myservoSix.write(90);    // Left Elbow

  myservo.write(90);       // Right Shoulder1
  myservoThree.write(40);  // Right Shoulder2
  myservoFour.write(60);   // Right Elbow
  delay(2000);

  // **2. Verse (Flowing Alternations)**
  for (int i = 0; i < 2; i++) { // Repeat twice for rhythmic flow
    // Left arm waves inward, Right arm sweeps outward
    myservoTwo.write(60);    // Left Shoulder1 (inward)
    myservoFive.write(140);  // Left Shoulder2
    myservoSix.write(45);    // Left Elbow

    myservo.write(120);      // Right Shoulder1 (outward)
    myservoThree.write(140); // Right Shoulder2
    myservoFour.write(30);   // Right Elbow
    delay(2000);

    // Reverse: Left arm sweeps outward, Right arm waves inward
    myservoTwo.write(120);   // Left Shoulder1 (outward)
    myservoFive.write(160);  // Left Shoulder2
    myservoSix.write(90);    // Left Elbow

    myservo.write(60);       // Right Shoulder1 (inward)
    myservoThree.write(120); // Right Shoulder2
    myservoFour.write(90);   // Right Elbow
    delay(2000);
  }

  // **3. Chorus (Dynamic Symmetry)**
  for (int i = 0; i < 2; i++) { // Repeat expressive gestures twice
    // Both arms open wide
    myservoTwo.write(0);      // Left Shoulder1
    myservoFive.write(140);   // Left Shoulder2
    myservoSix.write(60);     // Left Elbow

    myservo.write(180);       // Right Shoulder1
    myservoThree.write(40);   // Right Shoulder2
    myservoFour.write(30);    // Right Elbow
    delay(3000);

    // Both arms lower gently
    myservoTwo.write(180);    // Left Shoulder1
    myservoFive.write(160);   // Left Shoulder2
    myservoSix.write(180);    // Left Elbow

    myservo.write(0);         // Right Shoulder1
    myservoThree.write(0);    // Right Shoulder2
    myservoFour.write(0);     // Right Elbow
    delay(3000);
  }

  // **4. Bridge (Graceful Waves)**
  for (int i = 0; i < 2; i++) {
    // Left arm waves upward, Right arm lowers and rises
    myservoTwo.write(90);    // Left Shoulder1
    myservoFive.write(140);  // Left Shoulder2
    myservoSix.write(60);    // Left Elbow (wave up)
    delay(1500);

    myservoSix.write(90);    // Left Elbow (wave down)
    delay(1500);

    myservo.write(45);       // Right Shoulder1 (lower)
    myservoThree.write(20);  // Right Shoulder2
    myservoFour.write(90);   // Right Elbow
    delay(1500);

    myservo.write(90);       // Right Shoulder1 (rise)
    myservoThree.write(120); // Right Shoulder2
    myservoFour.write(30);   // Right Elbow
    delay(1500);
  }

  // **5. Finale (Big Closing Gesture)**
  // Both arms stretch outward fully, then return to rest
  myservoTwo.write(60);    // Left Shoulder1
  myservoFive.write(140);  // Left Shoulder2
  myservoSix.write(30);    // Left Elbow

  myservo.write(90);       // Right Shoulder1
  myservoThree.write(140); // Right Shoulder2
  myservoFour.write(30);   // Right Elbow
  delay(4000);

  // Rest position
  setBothArmsUpAndDown();
  delay(3000);
}


void danceCityOfStars() {
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1100);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1100);
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1100);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1100);


  // **1. Intro (Soft, Dreamy Opening Movements)**

  // Both arms gently lift outward
  myservoTwo.write(90);    // Left Shoulder1
  myservoFive.write(140);  // Left Shoulder2
  myservoSix.write(60);    // Left Elbow

  myservo.write(90);       // Right Shoulder1
  myservoThree.write(140); // Right Shoulder2
  myservoFour.write(60);   // Right Elbow
  delay(1500);

  // Right arm lowers slightly while Left arm sweeps inward
  myservoTwo.write(120);   // Left Shoulder1
  myservoFive.write(150);  // Left Shoulder2
  myservoSix.write(90);    // Left Elbow

  myservo.write(70);       // Right Shoulder1
  myservoThree.write(130); // Right Shoulder2
  myservoFour.write(90);   // Right Elbow
  delay(1200);

  // Both arms briefly return to rest
  setBothArmsUpAndDown();
  delay(1500);

  // **2. Verse (Flowing Alternations with Subtle Asymmetry)**

  for (int i = 0; i < 2; i++) {
    // Left arm lifts outward while Right arm sweeps inward
    myservoTwo.write(100);    // Left Shoulder1 (lifting outward)
    myservoFive.write(140);   // Left Shoulder2
    myservoSix.write(60);     // Left Elbow

    myservo.write(90);        // Right Shoulder1 (sweeping inward)
    myservoThree.write(140);  // Right Shoulder2
    myservoFour.write(60);    // Right Elbow
    delay(1500);

    // Reverse: Right arm lifts outward while Left arm lowers
    myservoTwo.write(150);    // Left Shoulder1 (lowering)
    myservoFive.write(160);   // Left Shoulder2
    myservoSix.write(120);    // Left Elbow

    myservo.write(100);       // Right Shoulder1 (lifting outward)
    myservoThree.write(130);  // Right Shoulder2
    myservoFour.write(60);    // Right Elbow
    delay(1500);
  }

  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1100);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1100);


  // **3. Chorus (Expressive, Dreamy Gestures)**

  for (int i = 0; i < 2; i++) {
    // Both arms lift together gracefully
    setArmsUp();
    delay(2000);

    // Left arm waves outward while Right arm lowers slightly
    myservoTwo.write(90);    // Left Shoulder1
    myservoFive.write(140);  // Left Shoulder2
    myservoSix.write(60);    // Left Elbow (wave outward)

    myservo.write(70);       // Right Shoulder1 (lowering)
    myservoThree.write(130); // Right Shoulder2
    myservoFour.write(90);   // Right Elbow
    delay(2000);
  }

  // **4. Bridge (New Synchronized Wave Movement)**
  for (int i = 0; i < 2; i++) {
    // Both arms raise to a horizontal level and wave simultaneously
    myservoTwo.write(90);    // Left Shoulder1 (horizontal)
    myservoFive.write(140);  // Left Shoulder2
    myservoSix.write(60);    // Left Elbow (wave start)

    myservo.write(90);       // Right Shoulder1 (horizontal)
    myservoThree.write(140); // Right Shoulder2
    myservoFour.write(60);   // Right Elbow (wave start)
    delay(1000);

    // Both elbows wave downward
    myservoSix.write(90);    // Left Elbow (wave down)
    myservoFour.write(90);   // Right Elbow (wave down)
    delay(1000);
  }

  // **5. Finale (Elegant and Balanced Closing Gesture)**

  // Both arms rise outward for the final expressive moment
  setArmsUp();
  delay(2500);

  // Left arm holds while Right arm gently waves
  myservoTwo.write(90);     // Left Shoulder1
  myservoFive.write(140);   // Left Shoulder2
  myservoSix.write(60);     // Left Elbow

  myservo.write(90);        // Right Shoulder1
  myservoThree.write(130);  // Right Shoulder2
  myservoFour.write(60);    // Right Elbow (wave upward)
  delay(1000);

  myservoFour.write(90);    // Right Elbow (wave downward)
  delay(1000);

  // Both arms return to rest for a serene ending
  setBothArmsUpAndDown();
  delay(2000);
}




void dancePuffTheMagicDragon() {
  // **1. Intro (Slow and Gentle Swaying)**
  // Left arm rises slightly, Right arm stays low
  myservoTwo.write(80);   // Left Shoulder1
  myservoFive.write(140);  // Left Shoulder2
  myservoSix.write(60);    // Left Elbow

  myservo.write(90);       // Right Shoulder1
  myservoThree.write(160); // Right Shoulder2
  myservoFour.write(180);  // Right Elbow
  delay(1000);

  // Right arm rises slightly, Left arm lowers
  myservoTwo.write(150);   // Left Shoulder1
  myservoFive.write(160);  // Left Shoulder2
  myservoSix.write(180);   // Left Elbow

  myservo.write(120);      // Right Shoulder1
  myservoThree.write(140); // Right Shoulder2
  myservoFour.write(60);   // Right Elbow
  delay(1000);

  // **2. Verse (Flowing Arm Movements)**
  for (int i = 0; i < 3; i++) { // Loop for gentle rhythm
    // Left arm sweeps out and up, Right arm sweeps in and down
    myservoTwo.write(100);
    myservoFive.write(140);
    myservoSix.write(60);

    myservo.write(90);
    myservoThree.write(160);
    myservoFour.write(120);
    delay(2000);

    // Reverse: Left arm sweeps in and down, Right arm sweeps out and up
    myservoTwo.write(150);
    myservoFive.write(160);
    myservoSix.write(180);

    myservo.write(120);
    myservoThree.write(140);
    myservoFour.write(60);
    delay(2000);
  }

  // **3. Chorus (Expressive and Wide Gestures)**
  for (int i = 0; i < 2; i++) { // Repeat for emotional emphasis
    // Both arms open outward
    myservoTwo.write(90);    // Left Shoulder1
    myservoFive.write(140);  // Left Shoulder2
    myservoSix.write(60);    // Left Elbow

    myservo.write(70);      // Right Shoulder1
    myservoThree.write(90); // Right Shoulder2
    myservoFour.write(60);   // Right Elbow
    delay(1500);

    // Both arms lower gracefully
    myservoTwo.write(150);   // Left Shoulder1
    myservoFive.write(100);  // Left Shoulder2
    myservoSix.write(180);   // Left Elbow

    myservo.write(120);      // Right Shoulder1
    myservoThree.write(160); // Right Shoulder2
    myservoFour.write(120);  // Right Elbow
    delay(1500);
  }

  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1500);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1500);
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1500);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1500);

  // **4. Bridge (Wave-Like Movements)**
  for (int i = 0; i < 2; i++) {
    // Left arm waves up and down
    myservoTwo.write(100);  // Arm up
    myservoFive.write(140);
    myservoSix.write(60);
    delay(1000);

    myservoTwo.write(150);  // Arm down
    myservoFive.write(160);
    myservoSix.write(180);
    delay(1000);

    // Right arm waves up and down
    myservo.write(60);  // Arm up
    myservoThree.write(140);
    myservoFour.write(60);
    delay(1000);

    myservo.write(120);  // Arm down
    myservoThree.write(160);
    myservoFour.write(120);
    delay(1000);
  }

  // **5. Finale (Wide Open Gesture and Rest)**
  // Both arms stretch outward, then slowly lower
  myservoTwo.write(90);    // Left Shoulder1
  myservoFive.write(100);  // Left Shoulder2
  myservoSix.write(60);    // Left Elbow

  myservo.write(80);      // Right Shoulder1
  myservoThree.write(100); // Right Shoulder2
  myservoFour.write(60);   // Right Elbow
  delay(3000);

  // Both arms lower gently to rest
  myservoTwo.write(150);   // Left Shoulder1
  myservoFive.write(160);  // Left Shoulder2
  myservoSix.write(180);   // Left Elbow

  myservo.write(120);      // Right Shoulder1
  myservoThree.write(160); // Right Shoulder2
  myservoFour.write(120);  // Right Elbow
  delay(3000);

  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1500);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1500);
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1500);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1500);
}



void danceDoYouWantToBuildASnowman() {
  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1000);
  
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1000);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1000);
  
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1000);
  // **1. Intro (Playful Knocking Movement)**
  for (int i = 0; i < 2; i++) {
    // Right arm mimics knocking
    myservo.write(90);       // Right Shoulder1
    myservoThree.write(140); // Right Shoulder2
    myservoFour.write(60);   // Right Elbow (up for knock)
    delay(500);

    myservoFour.write(120);   // Right Elbow (down for knock)
    delay(500);
  }

  // Left arm moves slightly up (as if waiting for a response)
  myservoTwo.write(120);    // Left Shoulder1
  myservoFive.write(140);   // Left Shoulder2
  myservoSix.write(90);     // Left Elbow
  delay(2000);

  // **2. Verse (Playful Alternations)**
  for (int i = 0; i < 3; i++) { // Loop for playful rhythm
    // Left arm sweeps up, Right arm sweeps down
    myservoTwo.write(90);
    myservoFive.write(140);
    myservoSix.write(60);

    myservo.write(60);
    myservoThree.write(160);
    myservoFour.write(120);
    delay(1500);

    // Reverse: Left arm sweeps down, Right arm sweeps up
    myservoTwo.write(150);
    myservoFive.write(160);
    myservoSix.write(180);

    myservo.write(120);
    myservoThree.write(140);
    myservoFour.write(60);
    delay(1500);
  }
  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1000);
  
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1000);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1000);
  
  servoAntenna1.write(120);
  servoAntenna2.write(60);


  // **3. Chorus (Both Arms Open and Close Gestures)**
  for (int i = 0; i < 2; i++) { // Repeat for emotional emphasis
    // Both arms open outward
    myservoTwo.write(90);    // Left Shoulder1
    myservoFive.write(140);  // Left Shoulder2
    myservoSix.write(60);    // Left Elbow

    myservo.write(100);      // Right Shoulder1
    myservoThree.write(140); // Right Shoulder2
    myservoFour.write(60);   // Right Elbow
    delay(1500);

    // Both arms close inward
    myservoTwo.write(120);   // Left Shoulder1
    myservoFive.write(160);  // Left Shoulder2
    myservoSix.write(120);   // Left Elbow

    myservo.write(90);       // Right Shoulder1
    myservoThree.write(160); // Right Shoulder2
    myservoFour.write(90);   // Right Elbow
    delay(1500);
  }
  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1000);
  
  servoAntenna1.write(120);
  servoAntenna2.write(60);

  delay(1000);

  servoAntenna1.write(60);
  servoAntenna2.write(120);

  delay(1000);
  
  servoAntenna1.write(120);
  servoAntenna2.write(60);


  // **4. Bridge (Gentle Swaying Movements)**
  for (int i = 0; i < 2; i++) {
    // Left arm sways upward
    myservoTwo.write(100);  // Arm up
    myservoFive.write(140);
    myservoSix.write(60);
    delay(1000);

    // Left arm sways downward
    myservoTwo.write(150);
    myservoFive.write(160);
    myservoSix.write(180);
    delay(1000);

    // Right arm sways upward
    myservo.write(90);  // Arm up
    myservoThree.write(140);
    myservoFour.write(60);
    delay(1000);

    // Right arm sways downward
    myservo.write(120);  // Arm down
    myservoThree.write(160);
    myservoFour.write(120);
    delay(1000);
  }

  // **5. Finale (Wide Open Gesture and Rest)**
  // Both arms stretch outward, then slowly lower
  myservoTwo.write(90);    // Left Shoulder1
  myservoFive.write(100);  // Left Shoulder2
  myservoSix.write(60);    // Left Elbow

  myservo.write(90);      // Right Shoulder1
  myservoThree.write(100); // Right Shoulder2
  myservoFour.write(60);   // Right Elbow
  delay(1500);

  // Both arms lower gently to rest
  myservoTwo.write(150);   // Left Shoulder1
  myservoFive.write(160);  // Left Shoulder2
  myservoSix.write(180);   // Left Elbow

  myservo.write(150);      // Right Shoulder1
  myservoThree.write(160); // Right Shoulder2
  myservoFour.write(120);  // Right Elbow
  delay(1500);
}





void (*danceRoutines[6])() = {
    danceAllIWant,        // Function 1
    danceTryEverything,   // Function 2
    danceFeelTheLove,     // Function 3
    danceCityOfStars,     // Function 4
    dancePuffTheMagicDragon, // Function 5
    danceDoYouWantToBuildASnowman // Function 6
};




void loop() {

  // valSensor = analogRead(Sensor);
  // int touchState = digitalRead(Sensor);
  int sensorReading = digitalRead(Sensor);

  if (sensorReading == HIGH) {
    touchState = true;
    motorState = true;
    gestureState = true;
  }


  // if(gestureState){
    paj7620_gesture_t result;
    if (Gesture.getResult(result)) {
        switch (result) {
        case RIGHT:
            Serial.println("Right");
            if(music == 11){
              music = 1;
            }else{
              music = music + 2;
            }
            Serial.println("change to music ");
            Serial.println(music);
            changed = 1;
            touchState = true;
            break;
        case LEFT:
            Serial.println("Left");
            if(music == 1){
              music = 11;
            }else{
              music = music - 2;
            }
            Serial.println("change to music ");
            Serial.println(music);
            changed = 1;
            touchState = true;
            break;
        case UP:
            Serial.println("Up");
            music = chooseRandomNumber(music);
            touchState = true;
            Serial.println("Randomly change to music ");
            Serial.println(music);
            changed = 1;
            break;
        case DOWN:
            Serial.println("Down");
            music = chooseRandomNumber(music);
            touchState = true;
            Serial.println("Randomly change to music ");
            Serial.println(music);
            changed = 1;
            break;
        
        default:
            break;
        }
    }
    delay(100);
  // }


  if(touchState){
    Serial.println("Playing a new song...");
    // music = 11;
    myDFPlayer.play(music);  // Play the first song
    touchState = false;
  }

  if (motorState) {

    // setArmsForward();
    // setArmsDown();

    // setSalute();
    // setWave1();
    // setWave2();
    // setWalk();
    // setArmsForward()
    // set0();
    // setBothArmsUpAndDown();
    // dance1();

    // Serial.print(changed);
    // if (changed == 1 || changed == 2){
      danceRoutines[(music + 1) / 2 - 1]();
      Serial.print(music);
      Serial.println("dance change to ");
      Serial.print((music + 1) / 2 - 1);
    //   changed = 0;

    

    // } else {
    //   danceRoutines[music]();
    // }
  }
    


}
