/*
recives OSCBundles from MPR121 Sensor
 print addrPattern() to see ID
 change addrPattern() for right ID
 
 
 /ESP_Touch/touched gets the touched PIN
 /ESP_Touch/released gets the released PIN
 
 */

import oscP5.*;
import netP5.*;

OscP5 oscP5;

int touched = 0;
int released = 0;
int[] button = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int Port = 9999;
String devID = "ESP_Touch";

void setup() {
  size(900, 200, P3D);
  frameRate(30);
  //set this to the receiving port
  oscP5 = new OscP5(this, Port);
}


void draw() {
  background(0);

  for (int i = 0; i<12; i++) {
    if (button[i] == 1) {
      fill(150, 0, 0);
    } else
    {
      fill(0, 150, 0);
    }
    circle(i*80+50, 100, 60);
  }
}

// incoming osc message are forwarded to the oscEvent method. 
void oscEvent(OscMessage theOscMessage) {
  //println(theOscMessage.addrPattern());

  if (theOscMessage.addrPattern().equals("/"+devID+"/touched")) {
    touched = theOscMessage.get(0).intValue();
    button[touched]=1;
    println(touched);
  } else if (theOscMessage.addrPattern().equals("/"+devID+"/released")) {
    released = theOscMessage.get(0).intValue();
    button[released]=0;
  }
}
