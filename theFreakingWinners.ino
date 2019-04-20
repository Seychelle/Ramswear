#include <Adafruit_NeoPixel.h>
#include <Adafruit_AMG88xx.h>
#include "Adafruit_DRV2605.h"
#include <Wire.h>
#ifndef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN  3
#define PIN2 4
#define PIN3 5
#define PIN4 6
#define PIN0 2

//For switch
#define SWITCH 7

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 3
#define NUM16 16
#define A0 A0
#define A1 A1
#define A2 A2
#define A3 A3
//declare LEDS
Adafruit_NeoPixel hand = Adafruit_NeoPixel(NUM16, PIN0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ind = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel mid = Adafruit_NeoPixel(NUMPIXELS, PIN2, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN3, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel pink = Adafruit_NeoPixel(NUMPIXELS, PIN4, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel empty = Adafruit_NeoPixel(NUMPIXELS, 12, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel fingers[6] = {hand,ind,mid,ring,pink,empty};
//int delayval = 500; // delay for half a second

//declare thermal camera
Adafruit_AMG88xx amg;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

//for haptic motor
Adafruit_DRV2605 drv;
int temp = 0;
int dangerHot = 85; //water boils at 100, we can demonstrate that the glove detects hot temperature by having the IR's sense boiling water
int painfulHot = 67;
int okayHot = 49; 
int okayCold = 31;
int painfulCold = 13; 
int dangerCold = -5;   

float sensorVals [4] = {A0,A1,A2,A3};

void setup() {
  //Serial.begin(9600);
  //initialize motor library
  drv.begin();
  //intialize thermal camera
  amg.begin();
  
  //initialize NeoPixel library for each set of LEDs
  hand.begin();
  ind.begin(); 
  mid.begin();
  ring.begin();
  pink.begin();
}

void IRCam(float pixelFinal [16]){
    amg.readPixels(pixels);
    int len = sqrt(AMG88xx_PIXEL_ARRAY_SIZE);
    int row[len];
    int col[len];
    //int pixelFinal[len*2];
    int average= 0;
    int pF=0;
     for(int i = 1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
      if(i%2==0){
      //get pixels in first two rows, first two cols, average
       average =(pixels[i]+pixels[i-1]+pixels[i+len]+pixels[(i+len)-1])/4;
      //add average to pixelfinal
       pixelFinal[pF]=average;
       pF++;
      }
     }
}

void light(int color, int color2, int color3, Adafruit_NeoPixel finger, bool swit){
    if(swit == true){
    for(int i=0;i<NUMPIXELS;i++){
    ind.setPixelColor(i, ind.Color(color,color2,color3));
    ind.setBrightness(150);
    ind.show();
    mid.setPixelColor(i, mid.Color(color,color2,color3));
    mid.setBrightness(150);
    mid.show();
    ring.setPixelColor(i, ring.Color(color,color2,color3));
    ring.setBrightness(150);
    ring.show();
    pink.setPixelColor(i, pink.Color(color,color2,color3));
    pink.setBrightness(150);
    pink.show();
    }
    for(int i = 0; i<NUM16; i++){
    hand.setPixelColor(i, hand.Color(color,color2,color3));
    hand.setBrightness(150);
    hand.show();
    }
    }
}

void motor(){
  drv.setWaveform(0, 70);  // play effect 
  drv.setWaveform(1, 0);       // end waveform
  drv.go();
}

void getTempCol(float temp, Adafruit_NeoPixel given, bool swit){
//bool bli = false;
if(temp >= dangerHot){ //display RED - BLINK - VIBRATE
    //bli = true;
    light(255, 0, 0, given, swit); 
    motor();       
  }
if(temp < dangerHot && temp >= painfulHot){ // Orange
   light(255,map(temp,85,67,0,112),0, given, swit);
  }
if(temp < painfulHot && temp >= okayHot){ //Yellow 
  light(255, map(temp,67,49,112,190), 0, given, swit);
  }
if(temp < okayHot && temp > okayCold ){ // Green this covers the upper bound on safe, the 
  light(map(temp,49,31,255,0), map(temp,49,31,190,255), 0, given, swit);
  }
if(temp > painfulCold && temp <= okayCold ){ // Blue- green 
  light(0, 255, map(temp,31,13,0,120), given, swit); 
  }
if(temp > dangerCold && temp <= painfulCold ){ // light blue
  light(0, map(temp,13,-5,255,190), map(temp,13,-5,120,225), given, swit);
  }
if(temp <= dangerCold ){ // Dark Blue
  //bli = true;
  //light(0, 0, 225, given, swit);
  motor();
  } 
  delay(100);
}

float IRtemp(float sensorValue){
  //Serial.println(analogRead(A0));
  //delay(100);
  // read the input on analog pin 0:
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = (sensorValue * (5.0 / 1024.0)); //     / 300.4596;

if(voltage != 0 && voltage < 4.9){  
  voltage = (voltage/155)*1000;
  voltage -= 4.75;
  Serial.println(voltage, 6);
  


  float oneOverThree = (float)1 /(float)3; // This is to avoid integer division
  //Serial.println("1/3: ");
  //Serial.println(oneOverThree);
  float p0 = pow(((3.8462 * voltage) + 28.5228), 2)+ 342.9034;
  //Serial.println("p0: ");
  //Serial.println(p0);
  float p1 = pow(p0, 0.5);
  //Serial.println("p1: ");
  //Serial.println(p1);
  float p2 = p1 + (3.8462*voltage) + 28.5228;
  //Serial.println("p2: ");
  //Serial.println(p2);
  float temp1 = pow(p2, oneOverThree);
  //Serial.println("temp1: ");
  //Serial.println(temp1);

  
  float temp2 = p1 * (-1);
  //Serial.println("temp2: ");
  //Serial.println(temp2);
  float p3 = temp2 + (3.8462*voltage) + 28.5228;
  //Serial.println("p3: ");
  //Serial.println(p3);
  bool neg = false;
  if(p3 < 0){
    p3 = -p3;
    neg = true;
  }
  float p4 =  pow(p3, oneOverThree);
  if(neg = true)
    p4 = -p4;
  //Serial.println("p4: ");
  //Serial.println(p4);
  
  float tempF = ((33 *temp1) + (33*p4)) - 47.8462;
  int t = static_cast<int>(tempF);
  //Serial.println("tempF: ");
  //Serial.println(t);
  //delay(1000);
  return t;
  }
  return 0;
}
void getColor(float temp, int colorVals [3]){
if(temp >= dangerHot){ //display RED - BLINK - VIBRATE
    colorVals[0] = 255;
    colorVals[1] = 0;
    colorVals[2] = 0;
    motor();     
  }
if(temp < dangerHot && temp >= painfulHot){ // Orange
    colorVals[0] = 255;
    colorVals[1] = 122;
    colorVals[2] = 0;
  }
if(temp < painfulHot && temp >= okayHot){ //Yellow 
  colorVals[0] = 255;
    colorVals[1] = 190;
    colorVals[2] = 0;
  }
if(temp < okayHot && temp > okayCold ){ // Green this covers the upper bound on safe, the 
  colorVals[0] = 0;
    colorVals[1] = 255;
    colorVals[2] = 0;
  }
if(temp > painfulCold && temp <= okayCold ){ // Blue- green 
  colorVals[0] = 0;
    colorVals[1] = 255;
    colorVals[2] = 120;
  }
if(temp > dangerCold && temp <= painfulCold ){ // light blue
  colorVals[0] = 0;
    colorVals[1] = 225;
    colorVals[2] = 225;
  }
if(temp <= dangerCold ){ // Dark Blue
    colorVals[0] = 0;
    colorVals[1] = 0;
    colorVals[2] = 255;
    motor();
  } 
}
void loop() {
  //if switch is at 1, then do "extreme cold through extreme heat" mode (demo mode)
  if(digitalRead(SWITCH)==1){
    bool swit = true;
    //loop through all temps for glove
    if(temp > 90){
      temp = -10;
    }
   if(temp > -12){
    temp = temp + 1;
    }
    getTempCol(temp, fingers[5], swit);
  }
  //otherwise, do normal mode
  else{
   bool swit = false;
  //For finger LEDs
  //IR Camera Data
  //float data [16];
  //IRCam(data);

 amg.readPixels(pixels);
    int len = sqrt(AMG88xx_PIXEL_ARRAY_SIZE);
    int row[len];
    int col[len];
    int pixelFinal[len*2];
    int average= 0;
    int pF=0;
     for(int i = 1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
      if(i%2==0){
      //get pixels in first two rows, first two cols, average
       average =(pixels[i]+pixels[i-1]+pixels[i+len]+pixels[(i+len)-1])/4;
      //add average to pixelfinal
       pixelFinal[pF]=average;
       pF++;
      }
     }
     
    
  //for back of hand LEDS
  for(int i = 0; i<len*2; i++){
      int temp = pixelFinal[i];
      if(temp >= dangerHot){ //display RED - BLINK - VIBRATE
    hand.setPixelColor(i,hand.Color(255,0,0));
    hand.show();
    motor();     
  }
if(temp < dangerHot && temp >= painfulHot){ // Orange
    hand.setPixelColor(i,hand.Color(255,map(temp,85,67,0,112),0));
    hand.show();
  }
if(temp < painfulHot && temp >= okayHot){ //Yellow 
  hand.setPixelColor(i,hand.Color(255,map(temp,67,49,112,190),0));
  hand.show();
  }
if(temp < okayHot && temp >=okayCold ){ // Green this covers the upper bound on safe, the 
  hand.setPixelColor(i,hand.Color(map(temp,49,31,255,0), map(temp,49,31,190,255),0));
  hand.show();
  }
if(temp > painfulCold && temp <= okayCold ){ // Blue- green 
  hand.setPixelColor(i,hand.Color(0,255,map(temp,31,13,0,120)));
  hand.show();
  }
if(temp > dangerCold && temp <= painfulCold ){ // light blue
  hand.setPixelColor(i,hand.Color(0,map(temp,13,-5,255,190), map(temp,13,-5,120,225)));
  hand.show();
  }
if(temp <= dangerCold ){ // Dark Blue
    hand.setPixelColor(i,hand.Color(0,0,255));
    hand.show();
    motor();
  } 
      //hand.setPixelColor(i, hand.Color(colorVals[0], colorVals[1], colorVals[2]));
  }
//a0 = ind, a1 = mid, a2 = ring, a3 = pink
//run loop for each finger
for(int pin = 0; pin < 4; pin++){
 //get analog read for each finger 
 int temper;
 Adafruit_NeoPixel finger;
 if(pin ==0){
    temper = IRtemp(analogRead(A0));
    finger = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
    finger.begin();}
 if(pin ==1){
    temper = IRtemp(analogRead(A1));
    finger = Adafruit_NeoPixel(NUMPIXELS, PIN3, NEO_GRB + NEO_KHZ800);
    finger.begin();}
 if(pin ==2){
    temper = IRtemp(analogRead(A2));
    finger = Adafruit_NeoPixel(NUMPIXELS, PIN4, NEO_GRB + NEO_KHZ800);
    finger.begin();}
 if(pin ==3){
    temper = IRtemp(analogRead(A3));
    finger = Adafruit_NeoPixel(NUMPIXELS, PIN2, NEO_GRB + NEO_KHZ800);
    finger.begin();}
 for(int i = 0; i<3; i++){
      //get IR sensor data and light each finger according to IR sensor
       if(temper >= dangerHot){ //display RED - BLINK - VIBRATE
      finger.setPixelColor(i,finger.Color(255,0,0));
      finger.show();
    motor();     
  }
if(temper < dangerHot && temper >= painfulHot){ // Orange
   ind.setPixelColor(i,finger.Color(255,map(temp,85,67,0,112),0));
     ind.show();
  }
if(temper < painfulHot && temper >= okayHot){ //Yellow 
  finger.setPixelColor(i,finger.Color(225, map(temp,67,49,112,190),0));
        finger.show();
}
if(temper < okayHot && temper >=okayCold ){ // Green this covers the upper bound on safe, the 
  finger.setPixelColor(i,finger.Color(map(temp,49,31,255,0), map(temp,49,31,190,255),0));
  finger.show();
  }
if(temper > painfulCold && temper <= okayCold ){ // Blue- green 
  finger.setPixelColor(i,finger.Color(0,255,map(temp,31,13,0,120)));
        finger.show();
  }
if(temper > dangerCold && temper <= painfulCold ){ // light blue
  finger.setPixelColor(i,finger.Color(0,map(temp,13,-5,255,190), map(temp,13,-5,120,225)));
        finger.show();
  }
if(temper <= dangerCold ){ // Dark Blue
    finger.setPixelColor(i,finger.Color(0,0,255));
    finger.show();
    motor();
  } 
     // fingers[i].setPixelColor(i, fingers[i].Color(colors[0], colors[1], colors[2]));
     //delay(1000);
  }
}
delay(200);
  }
  }
