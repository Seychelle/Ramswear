#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 1  // This many NeoPixels...
#define LED_PIN  1  // are connected to this DIGITAL pin #
#define NUMPIXELS = 1 
int Red = 255 ;
int Green = 255;
int Blue = 0 ;// Starting color will be yellow
int potPin = A2; 
int potval = 0 ; 
int temp = 25;  
int i = 0;
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN);
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();                  // Initialize all pixels to 'off'
//  pixels.begin(); //maybe this will int pixel lib to recg color wipe
}

void loop() {
potval = analogRead(potPin); // Well do an anlog read of a pot for now
Serial.print("potval:");
Serial.print(potval);
temp = map(potval, 3, 1023, -20, 100);
Serial.print("    Temp:");
Serial.print(temp);
  delay(200);


/*
if(temp<100) {  // we will use this to cycle through temps for now
temp = temp + 1;
}
if(temp>=100){
temp = -20;
}
*/
if (temp<=25) { // temp less then 25 so start yellow (255,255,0) and map towards blue (0,0,255)
  strip.setBrightness(50);
  Red = map(temp, -20,25,0,255);
  Green = map(temp, -20,25,0,255) ;
  Blue = map(temp, -20,25,255,0) ;
  strip.setPixelColor(0, Red, Green, Blue);
  strip.show();
  Serial.print("   Red:");
Serial.print(Red);
Serial.print("   Blue:");
Serial.print(Blue);
Serial.print("   Green:");
Serial.println(Green);
  //colorWipe(strip.Color(Red, Green, Blue), 50);
  //pixels.setPixelColor(i, pixels.Color(Red,Green,Blue)); 
}

if (temp>25) { // temp is more then 25 so itll start yellow and map towards Red
  strip.setBrightness(50);
  Red = 255 ; 
  Green = map(temp,25,100,255,0) ; // map from 255 to 0? this should work according to internet..
  Blue = 0 ; 
  strip.setPixelColor(0, Red, Green, Blue);
  strip.show();
    Serial.print("   Red:");
Serial.print(Red);
Serial.print("   Blue:");
Serial.print(Blue);
Serial.print("   Green:");
Serial.println(Green);
  //pixels.setPixelColor(i, pixels.Color(Red,Green,Blue)); // Moderately bright green color.

//  pixels.show(); // This sends the updated pixel color to the hardware.
  //colorWipe(strip.Color(Red, Green, Blue), 50); //whats the 50 for? intensity?
}
  
delay(2); // Pause 2 ms before repeating
  
}


  


 /* 
//    colorWipe(strip.Color(0, 0, 0), 50); // Animate off
//  } else {                                   // else sensor is dark
    colorWipe(strip.Color(255, 0, 255), 50);     // Animate purple

        colorWipe(strip.Color(Red, Green, Blue), 50);     // Animate purple
 // }
  delay(2); // Pause 2 ms before repeating
}

// Fill pixels one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}*/
