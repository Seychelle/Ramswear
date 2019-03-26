#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;
// https://learn.adafruit.com/adafruit-drv2605-haptic-controller-breakout/arduino-code
// need library: Adafruit DRV2605

// This code will play two vibrations once on boot, then loop a different vibe
void setup() {
  Serial.begin(9600);
  Serial.println("DRV test");
  drv.begin();
  
  drv.selectLibrary(1);
  
  // I2C trigger by sending 'go' command 
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG); 

  uint8_t effect = 106; // This sets the vibration effect. 106 is what i chose to boot with

  drv.setWaveform(0, effect);  // play effect 
  drv.setWaveform(1, 0);       // end waveform

  // play the effect!
  drv.go();
  delay(1500); //wait a sec yo
  drv.go();
  delay(2000);
}

void loop() {

  uint8_t effect = 70; //I chose 70 to inform us when we hit our temp range limits

  drv.setWaveform(0, effect);  // play effect 
  drv.setWaveform(1, 0);       // end waveform

  drv.go();
}
