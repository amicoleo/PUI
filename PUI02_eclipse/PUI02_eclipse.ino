#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

//Switch sensing
int swInputPin = 	   A1; 
int swLowValPin =      A0; 
bool swValue = false; 
bool swNextValue = false; 
int swValueStableTime = 200; 
unsigned long swValueChangeTime = 0; 
int swLowTreshold = 400; 
int swHighTreshold = 500; 


//Neopixel
int npPin = A3; 
const int npNum = 6;  
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(npNum, npPin, NEO_GRB + NEO_KHZ800);


void setup() {
  //Switch sensing
  
  pinMode(swInputPin, INPUT);
  //Set pullup resistors to pins
  digitalWrite(swInputPin, HIGH);
  //Low Val pin
  pinMode(swLowValPin, OUTPUT);
  digitalWrite(swLowValPin, LOW);
  
   //Neopixel
  pixels.begin(); // This initializes the NeoPixel library.

  //Debug
  Serial.begin(9600); 

}

void loop() {

  //Debug
  //Serial.println(analogRead(swInputPin)); 

  updateColor(); 

  //Read value changes
  if (!swNextValue && analogRead(swInputPin) < swLowTreshold){
    swNextValue = true; 
    swValueChangeTime = millis();
  } else if (swNextValue && analogRead(swInputPin) > swHighTreshold){
    swNextValue = false; 
    swValueChangeTime = millis();
  }

  //If there's a new value, and it's stable enough - change lamp state
  if (swValue != swNextValue && (millis() - swValueChangeTime > swValueStableTime)){
    if (swNextValue){
        //fadeOn
        setColor(200,200,150); 
    }else{
      //fadeOff
      setColor(0,0,0); 
    }
    swValue = swNextValue; 
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

