import processing.serial.*;
import controlP5.*;

ControlP5 cp5;
String arduinoMega = "/dev/ttyACM4";
Serial arduino;

int delayBeforeStart, delayMultiplier, loopCount, program, stepDelay = 15;
int lcdDelay, intensity = 100;
boolean channel1 = true, channel2 = true, channel3 = true;

void setup() {
  size(600, 600);

  printArray(Serial.list());
  arduino = new Serial(this, arduinoMega, 115200);

  // robot controls
  cp5 = new ControlP5(this);
  cp5.addSlider("program").setRange(0, 6).setPosition(10, 30).setWidth(360).addCallback(  
    new CallbackListener() {
    public void controlEvent(CallbackEvent theEvent) {
      switch(theEvent.getAction()) {
        case(ControlP5.ACTION_RELEASED):
        case(ControlP5.ACTION_RELEASEDOUTSIDE):
        sendData('P',byte(program));
        break;
      }
    }
  }
  );
  cp5.addSlider("stepDelay").setRange(0, 250).setPosition(10, 40).setWidth(360).addCallback(  
    new CallbackListener() {
    public void controlEvent(CallbackEvent theEvent) {
      switch(theEvent.getAction()) {
        case(ControlP5.ACTION_RELEASED):
        case(ControlP5.ACTION_RELEASEDOUTSIDE):
        sendData('S',byte(stepDelay));
        break;
      }
    }
  }
  );
  cp5.addSlider("lcdDelay").setRange(0, 250).setPosition(10, 50).setWidth(360).addCallback(  
    new CallbackListener() {
    public void controlEvent(CallbackEvent theEvent) {
      switch(theEvent.getAction()) {
        case(ControlP5.ACTION_RELEASED):
        case(ControlP5.ACTION_RELEASEDOUTSIDE):
        sendData('L',byte(lcdDelay));
        break;
      }
    }
  }
  );
  cp5.addSlider("intensity").setRange(0, 250).setPosition(10, 60).setWidth(360).addCallback(  
    new CallbackListener() {
    public void controlEvent(CallbackEvent theEvent) {
      switch(theEvent.getAction()) {
        case(ControlP5.ACTION_RELEASED):
        case(ControlP5.ACTION_RELEASEDOUTSIDE):
        sendData('V',byte(intensity));
        break;
      }
    }
  }
  );
  cp5.addButton("send").setSize(70, 18).setPosition(10, height-60);
  cp5.addButton("pulse").setSize(70, 18).setPosition(10, height-40);
  cp5.addToggle("channel1").setSize(70,18).setPosition(10,height-100);
  cp5.addToggle("channel2").setSize(70,18).setPosition(85,height-100);
  cp5.addToggle("channel3").setSize(70,18).setPosition(160,height-100);
  
  // stylish
  cp5.setColorActive(color(225));
  colorMode(HSB, 255);
  cp5.setColorBackground(color(random(255), 100, 100));
  cp5.setColorForeground(color(random(255), 100, 150));
  colorMode(RGB);
}

void draw() {
  background(0);
}

void send() {
  println("send!");
}

void pulse() {
  arduino.write("GO");
}

void serialEvent(Serial arduino) {
  char incomingChar = arduino.readChar();
  print (incomingChar);
}

void sendData(char command, int val) {
  arduino.write(unsignedByte(255));
  arduino.write(command);
  arduino.write(unsignedByte(val));
}

void channel1(boolean theFlag) {
  sendData('C',int(theFlag));
}
void channel2(boolean theFlag) {
  sendData('C',2+int(theFlag));
}
void channel3(boolean theFlag) {
  sendData('C',4+int(theFlag));
}

byte unsignedByte( int val ) { return (byte)( val > 127 ? val - 256 : val ); }
