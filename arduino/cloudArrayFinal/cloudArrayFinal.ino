#include <SoftPWM.h>

int led [] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A3, A4, A5};
int amountOfLeds = 13;
int maxPWM = 35;

int pulseInput = A0, pulseOutput = A1;
int lcdShutter = A2;
int rxLed = 2, txLed = 3;

int delayMultiplier = 1;
int transmitDelay = 0;
int stepDelay = 150;
unsigned long nextStep = millis() + stepDelay;

int intensity = 20, fadeIn = 0, fadeOut = 4000, probability = 40;

int lcdDelay = 3000;
boolean lcdShut = false, lcdActive = true;

int burstDuration = 0;
unsigned long burstEnd = millis() + burstDuration;
boolean bursting = false, autoBursting = true;
int autoBurstFrequency = 10000;
int oldProbability = probability;
boolean directionalRunning = false;

unsigned long nextShutter = millis() + lcdDelay;
int id = 10;

int program = 2;
#define REST 0
#define STARS 1
#define KNIGHTRIDER 2
#define SOLID 3
#define BLINKSOLID 4
#define VU 5
#define MIDDLEVU 6
#define DIRECTIONAL 7
#define RANDOMBITS 8

boolean feedback = false;

// program specific globals
int vuValue = 0;
int knightRiderLed = 0, knightRiderDirection = 1;
int blinkStep = 0;

void setup() {
  SoftPWMBegin();
  for (int i = 0; i < amountOfLeds; i++) {
    pinMode(led[i], OUTPUT);
    SoftPWMSet(led[i], 0);
  }
  SoftPWMSetFadeTime(ALL, fadeIn, fadeOut);

  pinMode(lcdShutter, OUTPUT);
  pinMode(pulseOutput, OUTPUT);
  pinMode(rxLed, OUTPUT);
  pinMode(txLed, OUTPUT);

  Serial.begin(115200);

  digitalWrite(rxLed, HIGH);
  digitalWrite(txLed, HIGH);

  for (int i = 0; i < amountOfLeds; i++) {
    digitalWrite(led[i], HIGH);
    delay(50);
    digitalWrite(led[i], LOW);
  }

  digitalWrite(rxLed, LOW);
  digitalWrite(txLed, LOW);

  Serial.flush();
}

void loop() {

  if (Serial.available() > 2) {

    digitalWrite(rxLed, HIGH);

    byte startByte = Serial.read();
    if (startByte == 255) {
      
      // startbyte 255, one value
      byte command, value;
      command = Serial.read();
      value = Serial.read();
      parseIncomingSerial(command, value);
      
      delay(transmitDelay * delayMultiplier);
      sendData(command, value);
      
    } else if (startByte == 254) {
      
      // startbyte 254, two values
      byte command, value1, value2;
      command = Serial.read();
      value1 = Serial.read();
      value2 = Serial.read();
      parseIncomingSerial(command, value1, value2);
      
    } else {
      // doesn't make sense, just pass it along anyways
      digitalWrite(txLed, HIGH);
      Serial.write(startByte);
      digitalWrite(txLed, LOW);
    }

    digitalWrite(rxLed, LOW);
  }

  if (millis() > nextStep) {

    switch (program) {
      case REST:
        // do nothing
        break;
      case KNIGHTRIDER:
        knightRider();
        break;
      case STARS:
        stars();
        break;
      case SOLID:
        solid();
        break;
      case BLINKSOLID:
        blinkSolid();
        break;
      case VU:
        vu();
        break;
      case MIDDLEVU:
        middleVu();
        break;
      case DIRECTIONAL:
        directional();
        break;
      case RANDOMBITS:
        randomBits();
        break;
    }

    nextStep = millis() + stepDelay;

  }

  if (bursting && millis() > burstEnd) {
    bursting = false;
    probability = oldProbability;
  }

  if (autoBursting && millis() > burstEnd + autoBurstFrequency) {
    createBurst();
  }

  if (lcdActive && millis() > nextShutter) {
    lcdShut = !lcdShut;
    digitalWrite(lcdShutter, lcdShut);
    nextShutter = millis() + lcdDelay;
  }

}

void solid() {
  int pwmVal = map(intensity, 0, 100, 0, maxPWM);
  pwmVal = constrain(pwmVal, 0, maxPWM);
  for (int i = 0; i < amountOfLeds; i++) {
    SoftPWMSet(led[i], pwmVal);
  }
}

void blinkSolid() {
  blinkStep++;
  blinkStep = blinkStep % 100;
  if (blinkStep % intensity == 0) {
    for (int i = 0; i < amountOfLeds; i++) {
      SoftPWMSet(led[i], maxPWM);
    }
  }
  else {
    for (int i = 0; i < amountOfLeds; i++) {
      SoftPWMSet(led[i], 0);
    }
  }

}
void randomBits() {
  int chance = map(probability, 0, 100, 100, 0);
  int r = random(chance);
  if (r == 0) {
    for (int i = 0; i < amountOfLeds; i++) {
      int on = random(100);
      if (on < intensity) {
        SoftPWMSet(led[i], maxPWM);
      } else {

        SoftPWMSet(led[i], 0);
      }
    }
  }
}

void stars() {
  int chance = map(probability, 0, 100, 100, 0);
  int r = random(chance);
  if (r == 0) {
    int l = random(amountOfLeds);
    SoftPWMSet(led[l], maxPWM);
    delay(5);
    SoftPWMSet(led[l], 0);
  }
}

void middleVu() {
  int chance = map(probability, 0, 100, 100, 0);
  int r = random(chance);
  if (r == 0) {
    int v = random(7);
    for (int i = 0; i < 7; i++) {
      if (i < v) {
        SoftPWMSet(led[6 - i], maxPWM);
        SoftPWMSet(led[6 + i], maxPWM);
      } else {
        SoftPWMSet(led[6 - i], 0);
        SoftPWMSet(led[6 + i], 0);
      }
    }
  }
}

void vu() {
  int chance = map(probability, 0, 100, 100, 0);
  int r = random(chance);
  if (r == 0) {
    int v = random(14);
    for (int i = 0; i < amountOfLeds; i++) {
      if (i < v) {
        SoftPWMSet(led[i], maxPWM);
      } else {
        SoftPWMSet(led[i], 0);
      }
    }
  }
}

void directional() {
  if (!directionalRunning) {

    int chance = map(probability, 0, 100, 100, 0);
    int r = random(chance);
    if (r == 0) {
      int dir = random(2);
      if (dir == 0) {
        knightRiderDirection = -1;
        knightRiderLed = amountOfLeds;
      } else {
        knightRiderDirection = 1;
        knightRiderLed = 0;
      }
      directionalRunning = true;
    }

  }
  if (directionalRunning) {

    for (int i = 0; i < amountOfLeds; i++) {
      if (i == knightRiderLed) {
        SoftPWMSet(led[i], maxPWM);
      } else {
        SoftPWMSet(led[i], 0);
      }
    }

    knightRiderLed += knightRiderDirection;

    if (knightRiderLed < -1 || knightRiderLed > amountOfLeds + 1) {
      directionalRunning = false;
      for (int i = 0; i < amountOfLeds; i++) {
        SoftPWMSet(led[i], 0);
      }
    }
  }
}

void knightRider() {
  if (knightRiderDirection == 1) {
    knightRiderLed ++;
    if (knightRiderLed > amountOfLeds) {
      knightRiderDirection = -1;
      knightRiderLed = amountOfLeds - 1;
    }
  }
  if (knightRiderDirection == -1) {
    knightRiderLed --;
    if (knightRiderLed < 0) {
      knightRiderDirection = 1;
      knightRiderLed = 1;
    }
  }
  for (int i = 0; i < amountOfLeds; i++) {
    if (i == knightRiderLed) {
      SoftPWMSet(led[i], maxPWM);
    } else {
      SoftPWMSet(led[i], 0);
    }
  }
}

void parseIncomingSerial(char command, byte value1, byte value2) {
  if (command == 's') {
    digitalWrite(txLed, HIGH);
    stepDelay = value1;
    Serial.print("step delay is ");
    Serial.println(stepDelay);
    int offset = value2;
    offset = offset - 100;
    Serial.print("offset is ");
    Serial.println(offset);
    int newStepDelay = stepDelay + offset;
    byte newValue1 = constrain(newStepDelay, 0, 250);
    Serial.println(newStepDelay);
    sendDoubleData('s', newValue1, value2);
    delay(5);
    digitalWrite(txLed, LOW);
  }
}

void parseIncomingSerial(char command, byte value) {
  if (command == 'T') {
    transmitDelay = value;
    if (feedback) Serial.print("transmitDelay set to ");
    if (feedback) Serial.println(int(value));
  }
  if (command == 'M') {
    delayMultiplier = value;
    if (feedback) Serial.print("delayMultiplier set to ");
    if (feedback) Serial.println(byte(value));
  }
  if (command == 'P') {
    program = value;
    if (feedback) Serial.println(byte(value));
  }
  if (command == 'S') {
    stepDelay = value;
  }
  if (command == 's') {
    stepDelay = map(id, 0, 100, value/10, value);
  }
  if (command == 'V') {
    intensity = constrain(value, 0, 100);
  }
  if (command == 'L') {
    if (value == 0) {
      lcdActive = false;
      digitalWrite(lcdShutter, LOW);
    } else if (value == 250) {
      lcdActive = false;
      digitalWrite(lcdShutter, HIGH);
    } else {
      lcdActive = true;
      lcdDelay = map(value, 1, 249, 0, 16000);
      nextShutter = millis() + lcdDelay;
    }
  }
  if (command == 'I') {
    fadeIn = map(value, 0, 250, 0, 4000);
    SoftPWMSetFadeTime(ALL, fadeIn, fadeOut);
  }
  if (command == 'O') {
    fadeOut = map(value, 0, 250, 0, 4000);
    SoftPWMSetFadeTime(ALL, fadeIn, fadeOut);
  }
  if (command == 'R') {
    randomSeed(value);
    id = value;
    value++;
    value = value % 250;
    sendData(byte('R'), value);
  }
  if (command == 'F') {
    delay(random(value * 100));
  }
  if (command == 'f') {
    delay(value * id);
  }
  if (command == 'C') {
    probability = constrain(value, 0, 100);
  }
  if (command == 'B') {
    createBurst(value);
  }
  if (command == 'b') {
    createRelativeBurst(value);
  }
  if (command == 'A') {
    if (value == 0) {
      autoBursting = false;
    } else {
      autoBursting = true;
      autoBurstFrequency = map(value, 1, 250, 1000, 60000);
    }
  }
}

void createBurst() {
  burstEnd = millis() + burstDuration;
  if (!bursting) {
    oldProbability = probability;
    probability = 100;
    bursting = true;
  }
}

void createBurst(byte dur) {
  burstDuration = map(dur, 0, 250, 0, 10000);
  burstEnd = millis() + burstDuration;
  if (!bursting) {
    oldProbability = probability;
    probability = 100;
    bursting = true;
  }
}

void createRelativeBurst(byte dur) {
  burstDuration = map(dur, 0, 250, 0, 10000);
  burstEnd = millis() + burstDuration;
  if (!bursting) {
    oldProbability = probability;
    probability = constrain(probability * 5, 0, 100);
    bursting = true;
  }
}

void sendData(byte command, byte value) {
  digitalWrite(txLed, HIGH);
  Serial.write(255);
  Serial.write(command);
  Serial.write(value);
  digitalWrite(txLed, LOW);
}

void sendDoubleData(byte command, byte value1, byte value2) {
  digitalWrite(txLed, HIGH);
  Serial.write(254);
  Serial.write(command);
  Serial.write(value1);
  Serial.write(value2);
  digitalWrite(txLed, LOW);
}
