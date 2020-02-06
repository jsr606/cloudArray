int led [] = {50, 46, 42};
boolean active [] = {false, false, false};

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);
  pinMode(13, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(led[i], OUTPUT);
    digitalWrite(led[i], HIGH);
  }
}

void loop() {

  if (Serial.available()) {
    // read the incoming byte:
    byte incomingByte = Serial.read();

    if (incomingByte == 251) {
      delay(3);
      byte incomingByte2 = Serial.read();
      if (incomingByte2 == 0) active[0] = false;
      if (incomingByte2 == 1) active[0] = true;
      if (incomingByte2 == 2) active[1] = false;
      if (incomingByte2 == 3) active[1] = true;
      if (incomingByte2 == 4) active[2] = false;
      if (incomingByte2 == 5) active[2] = true;
      }

    // say what you got:
    Serial.write("I forwarded: ");
    Serial.write(incomingByte);
    Serial.write(" / ");
    Serial.write(char(incomingByte));
    Serial.write('\n');

    if (active[0]) {
      digitalWrite(led[0], HIGH);
      Serial1.write(incomingByte);
    }
    if (active[1]) {
      digitalWrite(led[1], HIGH);
      Serial2.write(incomingByte);
    }
    if (active[2]) {
      digitalWrite(led[2], HIGH);
      Serial3.write(incomingByte);
    }

  }
  delay(1);
  for (int i = 0; i < 3; i++) {
    digitalWrite(led[i], active[i]);
  }
}
