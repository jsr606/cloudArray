int led [] = {50, 46, 42};
boolean active [] = {true, true, true};
int tickLed = 13;
unsigned long nextTick = millis();
int seconds = 0;
boolean tock = false, ticked = false;
int nextProgramChange = 1;
int morphingParameter = -1, startVal = -1, endVal = -1, morphLength = -1, morphProgression = -1, morphVal = -1, lastMorphVal = -1, peakDuration = -1;
boolean morphing = false, morphRiseAndFall = true, morphLooping = true;
int morphState = -1;
byte formation = 0b00000111;

unsigned int lastMorph;
int morphFreq = 40, lastFeedbackSec = -1;
int counterActionFreq = 10, counterAction = -1, counterLow = 0, counterHigh = 251;
int burstChance = -1;

int currentPreset = -1;

#define RISE 0
#define PEAK 1
#define FALL 2
#define DONE 3

int program = 4, lastProgram = -1, amountOfPrograms = 9;
#define REST 0
#define STARS 1
#define KNIGHTRIDER 2
#define SOLID 3
#define BLINKSOLID 4
#define VU 5
#define MIDDLEVU 6
#define DIRECTIONAL 7
#define RANDOMBITS 8

#define NONE 0
#define STEPDELAY 1
#define INTENSITY 2
#define PROBABILITY 3
#define LCDFREQUENCY 4

int slowness = 3;
boolean morphFeedback = false;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);
  for (int i = 0; i < 3; i++) {
    pinMode(led[i], OUTPUT);
    digitalWrite(led[i], active[i]);
  }
  pinMode(tickLed, OUTPUT);

  // random seed and id
  sendData('R', 0);
  nextProgramChange = 1;
  preset(7);

}

void loop() {
  tick();
  if (ticked) {
    Serial.print(seconds);
    Serial.print(".");
    if (seconds >= nextProgramChange * slowness) {
      int nextPreset = currentPreset;
      while (currentPreset == nextPreset) {
        nextPreset = random(10);
      }
      preset(nextPreset);
    }
    if ((seconds % (counterActionFreq * slowness)) == 0) {
      doCounterAction();
    }
  }
  if (morphing && lastMorph + morphFreq < millis()) {
    morphProgression++;
    morph();
    lastMorph = millis();
  }
  if (burstChance > 0) {
    int ran = random(10000);
    if (ran < burstChance) {
      //Serial.println("burst!");
      sendData('B', random(30));
    }
  }
  delay(20);
}

void doCounterAction() {
  Serial.print("counterAction time");
  formation = ~formation;
  if (counterAction == LCDFREQUENCY) {
    sendData('L', random(counterLow, counterHigh));
  }
  if (counterAction == INTENSITY) {
    sendData('V', random(counterLow, counterHigh));
  }
  formation = ~formation;

  Serial.println();
}

void rest() {
  Serial.print("resting");
  delay(random(500, 5000));
  Serial.println(".");
}

void preset(int _preset) {
  currentPreset = _preset;
  rest();
  morphing = false;
  Serial.print("change preset to ");
  Serial.println(currentPreset);

  int ran = random(3);
  switch (currentPreset) {
    case 0:
      Serial.println("directionals");
      formation = 0b00000111;
      sendData('I', 10);
      sendData('O', random(50, 250));
      sendData('S', random(150, 250));
      sendData('C', random(1, 5));
      programChange(DIRECTIONAL);
      newMorph(PROBABILITY, random(100), 0, random(200, 800), true, true);
      formation = 0b00000101;
      nextProgramChange = random(5, 7);
      counterActionFreq = random(3, 5);
      counterAction = STEPDELAY;
      counterHigh = 251;
      counterLow = 20;
      break;
    case 1:
      Serial.println("stars");
      formation = 0b00000111;
      sendData('L', random(100, 150));
      programChange(STARS);
      sendData('I', random(5));
      sendData('O', random(200, 251));
      sendData('S', random(5, 25));
      formation = 0b00000101;
      newMorph(STEPDELAY, random(30, 100), random(0, 15), random(200, 800), true, true);
      counterAction = STEPDELAY;
      counterHigh = 35;
      counterLow = 1;
      burstChance = random(100);
      nextProgramChange = random(4);
      break;
    case 2:
      Serial.println("totes random");
      if (ran == 0) formation = 0b00000111;
      if (ran == 1) formation = 0b00000101;
      if (ran == 2) formation = 0b00000010;
      programChange(random(0, amountOfPrograms));
      newMorph(random(4), random(0, 50), random(200, 250), random(200, 800), true, true);
      counterAction = random(1, 5);
      counterHigh = random(255);
      counterLow = 0;
      burstChance = random(100);
      nextProgramChange = random(4);
      break;
    case 3:
      Serial.println("hectic stuff");
      formation = 0b00000111;
      sendData('I', 0);
      sendData('O', random(1, 25));
      sendData('S', random(0, 10));
      nextProgramChange = random(2);
      break;
    case 4:
      Serial.println("rest");
      programChange(REST);
      nextProgramChange = random(1, 2);
      break;
    case 5:
      Serial.println("chill trails");
      formation = 0b00000111;
      sendData('I', random(5));
      sendData('O', random(10, 250));
      nextProgramChange = 0;
      break;
    case 6:
      Serial.println("LCD frequency");
      formation = 0b00000111;
      newMorph(LCDFREQUENCY, random(0, 251), random(30, 251), random(50, 200), false, false);
      nextProgramChange = 2;
      break;
    case 7:
      Serial.println("VU middle");
      formation = 0b00000010;
      sendData('I', 0);
      sendData('O', random(1, 25));
      sendData('S', random(0, 10));
      programChange(MIDDLEVU);
      newMorph(STEPDELAY, random(0, 30), random(30, 251), random(50, 200), true, true);
      nextProgramChange = random(1, 3);
      break;
    case 8:
      Serial.println("autoburst on");
      if (ran == 0) formation = 0b00000111;
      if (ran == 1) formation = 0b00000101;
      if (ran == 2) formation = 0b00000010;
      sendData('A', random(1, 251));
      break;
    case 9:
      Serial.println("autoburst off");
      formation = 0b00000111;
      sendData('A', 0);
      break;
    default:
      break;

  }
  Serial.print("new preset ");
  Serial.print(_preset);
  Serial.print(": ");
  printMorphSettings();
  currentPreset = _preset;
}

void printMorphSettings() {
  if (morphing) {
    Serial.print("morph");
    if (morphRiseAndFall) Serial.print (" riseAndFall");
    if (morphLooping) Serial.print(" looping");

    Serial.print(" ");
    Serial.print(startVal);
    Serial.print(" / ");
    Serial.print(endVal);
    Serial.print(" length: ");
    Serial.print(morphLength);

    switch (morphingParameter) {
      case (STEPDELAY):
        Serial.print(" stepDelay");
        break;
      case (INTENSITY):
        Serial.print(" intensity");
        break;
      case (PROBABILITY):
        Serial.print(" probability");
        break;
      case (LCDFREQUENCY):
        Serial.print(" LCD frequency");
        break;
    }
  }

  Serial.print(" ");
  for (int i = 0; i < 3; i++) {
    bool _f = bitRead(formation, i);
    if (_f) {
      Serial.write('1');
    } else {
      Serial.write('0');
    }
  }

  Serial.println();
  Serial.print("slowness ");
  Serial.print(slowness);
  Serial.print(" next program change ");
  Serial.print(nextProgramChange * slowness);
  Serial.print(" counterAction frequency ");
  Serial.println(counterActionFreq * slowness);

}

void newMorph(int _param, int _startVal, int _endVal, int _dur, boolean _loop, boolean _riseAndFall) {
  morphingParameter = _param;
  morphRiseAndFall = _riseAndFall;
  morphLooping = _loop;
  morphState = RISE;
  setupMorph (_startVal, _endVal, _dur);
}

void setupMorph(int _startVal, int _endVal, int dur) {
  startVal = _startVal;
  endVal = _endVal;
  morphLength = dur;
  morphProgression = 0;
  morph();
  morphing = true;
}

void morph() {
  switch (morphState) {
    case RISE:
      morphVal = map(morphProgression, 0, morphLength, startVal, endVal);
      if (morphProgression == morphLength) {
        morphState = PEAK;
        peakDuration = random(morphLength / 5, morphLength / 3);
        morphProgression = 0;
      }
      break;
    case PEAK:
      if (morphProgression == peakDuration) {
        if (morphRiseAndFall) {
          setupMorph(endVal, startVal, morphLength);
          morphState = FALL;
        } else {
          if (morphLooping) {
            setupMorph(startVal, endVal, morphLength);
            morphState = RISE;
          } else {
            morphing = false;
          }
        }
      }

      break;
    case FALL:
      morphVal = map(morphProgression, 0, morphLength, startVal, endVal);
      if (morphProgression == morphLength) {
        if (morphLooping) {
          setupMorph(endVal, startVal, morphLength);
          morphState = RISE;
        } else {
          morphing = false;
        }
      }
      break;
  }

  if (morphVal != lastMorphVal) {
    if (morphFeedback) {
      if (morphState == RISE) Serial.print("RISE ");
      if (morphState == PEAK) Serial.print("PEAK ");
      if (morphState == FALL) Serial.print("FALL ");

      Serial.print(morphProgression);
      Serial.print(" / ");
      Serial.print(morphLength);
      Serial.print(" val ");
      Serial.print(morphVal);
    }
    if (morphingParameter == STEPDELAY) {
      sendData('S', morphVal);
      if (morphFeedback) Serial.print(" stepDelay");
    }
    if (morphingParameter == INTENSITY) {
      sendData('V', morphVal);
      if (morphFeedback) Serial.print(" instensity");
    }
    if (morphingParameter == PROBABILITY) {
      sendData('C', morphVal);
      if (morphFeedback) Serial.print(" probability");
    }
    if (morphingParameter == LCDFREQUENCY) {
      sendData('L', morphVal);
      if (morphFeedback) Serial.print(" LCD frequency");
    }
    if (morphFeedback) Serial.println();

    if (morphState == PEAK) {
      if (morphFeedback) Serial.print("hold it for ");
      if (morphFeedback) Serial.println(peakDuration);
    }
    lastMorphVal = morphVal;
  }


}

void programChange(int p) {
  sendData('P', p);
  seconds = 0;
}

void programChange() {
  morphing = false;
  lastProgram = program;
  while (program == lastProgram) {
    program = random(amountOfPrograms);
  }
  sendData('P', program);
  seconds = 0;
}

void tick() {
  ticked = false;
  if (millis() > nextTick) {
    tock = !tock;
    digitalWrite(tickLed, tock);
    seconds ++;
    nextTick = millis() + 1000;
    ticked = true;
  }
}

void sendData(byte command, byte value) {

  /*
    if (morphFeedback) Serial.print("\t");
    for (int i = 0; i < 3; i++) {
    active[i] = bitRead(formation, i);
    if (active[i]) {
      if (morphFeedback) Serial.write('1');
    } else {
      if (morphFeedback) Serial.write('0');
    }
    }
  */

  for (int i = 0; i < 3; i++) {
    digitalWrite(led[i], active[i]);

    if (active[i]) {
      if (i == 0) {
        Serial1.write(255);
        Serial1.write(command);
        Serial1.write(value);
      } else if (i == 1) {
        Serial2.write(255);
        Serial2.write(command);
        Serial2.write(value);
      } else if (i == 2) {
        Serial3.write(255);
        Serial3.write(command);
        Serial3.write(value);
      }
    }
    delay(2);
    digitalWrite(led[i], LOW);
  }
}

void sendDoubleData(byte command, byte value1, byte value2) {

  Serial.print("\t");
  for (int i = 0; i < 3; i++) {
    active[i] = bitRead(formation, i);
    if (active[i]) {
      Serial.write('1');
    } else {
      Serial.write('0');
    }
  }

  for (int i = 0; i < 3; i++) {
    digitalWrite(led[i], active[i]);

    if (active[i]) {
      if (i == 0) {
        Serial1.write(254);
        Serial1.write(command);
        Serial1.write(value1);
        Serial1.write(value2);
      }
      if (i == 1) {
        Serial2.write(254);
        Serial2.write(command);
        Serial2.write(value1);
        Serial2.write(value2);
      }
      if (i == 2) {
        Serial3.write(254);
        Serial3.write(command);
        Serial3.write(value1);
        Serial3.write(value2);
      }
    }
    delay(2);
    digitalWrite(led[i], LOW);
  }
}
