/*********************************************************
  This is a library for the MPR121 12-channel Capacitive touch sensor

  added ESP32 sending OSC messages by artdanion

  Designed specifically to work with the MPR121 Breakout in the Adafruit shop
  ----> https://www.adafruit.com/products/

  These sensors use I2C communicate, at least 2 pins are required
  to interface

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
**********************************************************/

#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <WiFi.h>
#include <OSCBundle.h>
#include <WiFiUdp.h>

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

char ssid[] = "xxxxxx";    // your network SSID (name)
char pass[] = "xxxxxx";    // your network password

WiFiUDP Udp;                                  // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 0, 114);      // remote IP of your computer
const unsigned int outPort = 9999;            // remote port to receive OSC

const unsigned int localPort = 8888;

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void setup() {
  Serial.begin(115200);

  delay(100);

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif
}

void loop() {
  // Get the currently touched pads
  currtouched = cap.touched();

  OSCBundle bndl;

  for (uint8_t i = 0; i < 12; i++) {
    // if it *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(1);
      bndl.add("/ESP_Touch/touched").add(i);
      Udp.beginPacket(outIp, outPort);
      bndl.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      bndl.empty();
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(0);
      bndl.add("/ESP_Touch/released").add(i);
      Udp.beginPacket(outIp, outPort);
      bndl.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      bndl.empty();
    }
  }
  // reset our state
  lasttouched = currtouched;

  return;
}
