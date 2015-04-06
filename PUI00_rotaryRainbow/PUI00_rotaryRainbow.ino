#include "mpr121.h"
#include "capThresholds.h"

#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

/*
NOTES: 

- the central pad needs a wire like for the cone

*/  


//Capacitive sensing
int capIrqPin = 10;  // Digital 2
boolean capTouchStates[12]; //to keep track of the previous touch states
int lastCapTouchIndex = -1; 
unsigned long lastCapTouchTime = 0; 
int capTouchStableTime = 200; 

//Neopixel
int npPin = A3; 
const int npNum = 6;  
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(npNum, npPin, NEO_GRB + NEO_KHZ800);


void setup(){
  //MPR121 CapSense
  pinMode(capIrqPin, INPUT);
  digitalWrite(capIrqPin, HIGH); //enable pullup resistor
  Serial.begin(9600);
  Wire.begin();
  mpr121_setup();

  //Neopixel
  pixels.begin(); // This initializes the NeoPixel library.
  
}

void loop(){
  readTouchInputs();
  updateColor(); 
  
  //Time based hysteresis
  if (lastCapTouchIndex > -1){ //There's some waiting
    if (millis()  - lastCapTouchTime > capTouchStableTime){    
      //Set color
      if (lastCapTouchIndex == 4){
        setColor(150,200,60); 
      }
      else if (lastCapTouchIndex == 3){
        setColor(00,200,150); 
      }
      else if (lastCapTouchIndex == 2){
        setColor(200,200,180); 
      }else if (lastCapTouchIndex == 1){
        setColor(150,0,150); 
      }else if (lastCapTouchIndex == 0){
        setColor(200,100,0); 
      }
      lastCapTouchIndex = -1; 
    }
  }
}


uint32_t currentColor, nextColor; 
float colorInc; 
unsigned long lastColorUpdate = millis();
const int colorUpdateInterval = 30; 
const int colorTransitionTime = 300; 
float colorTransitionSteps = (float)colorTransitionTime/colorUpdateInterval; 
int colorTransitionCurrentStep = 0; 

void setColor(uint8_t r,uint8_t g,uint8_t b){
  currentColor = pixels.getPixelColor(1); //Take the color of the first one
  nextColor = pixels.Color(r,g,b); 

  colorInc = ((float)nextColor - currentColor)/colorTransitionSteps; 
  colorTransitionCurrentStep = 0; 

}

void updateColor(){

  if (millis() - lastColorUpdate > colorUpdateInterval){
    lastColorUpdate = millis();
    if (colorTransitionCurrentStep < colorTransitionSteps){
      colorTransitionCurrentStep++;
      currentColor+=colorInc; 

      for(int i=0;i<npNum;i++){
        pixels.setPixelColor(i, currentColor); // Moderately bright green color.
        pixels.show(); // This sends the updated pixel color to the hardware.
      }
    }
  }
}


void readTouchInputs(){
  if(!checkInterrupt()){
    //read the touch state from the MPR121
    Wire.requestFrom(0x5A,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states


    
    
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
      
        if(capTouchStates[i] == 0){
          //pin i was just touched
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" was just touched");

          lastCapTouchIndex = i; 
          lastCapTouchTime = millis(); 
          

          

        
        }else if(capTouchStates[i] == 1){

        }  
      
        capTouchStates[i] = 1;      
      }else{
        if(capTouchStates[i] == 1){
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" is no longer being touched");
          
          if (i == lastCapTouchIndex){
            lastCapTouchIndex = -1; 
          }

          //pin i is no longer being touched
       }
        
        capTouchStates[i] = 0;
      }
    
    }
    
  }
}





void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, E00_TOU_THRESH);
  set_register(0x5A, ELE0_R, E00_REL_THRESH);
 
  set_register(0x5A, ELE1_T, E01_TOU_THRESH);
  set_register(0x5A, ELE1_R, E01_REL_THRESH);
  
  set_register(0x5A, ELE2_T, E02_TOU_THRESH);
  set_register(0x5A, ELE2_R, E02_REL_THRESH);
  
  set_register(0x5A, ELE3_T, E03_TOU_THRESH);
  set_register(0x5A, ELE3_R, E03_REL_THRESH);
  
  set_register(0x5A, ELE4_T, E04_TOU_THRESH);
  set_register(0x5A, ELE4_R, E04_REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*
  set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V
  
  //set_register(0x5A, ELE_CFG, 0x0C);
  */
  
}


boolean checkInterrupt(void){
  return digitalRead(capIrqPin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
