#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

//Switch sensing


bool swLargeSurfaceValue = false; 
bool swLargeSurfaceNextValue = false; 
unsigned long swLargeSurfaceValueChangeTime = 0; 


bool swSmallSurfaceValue = false; 
bool swSmallSurfaceNextValue = false; 
unsigned long swSmallSurfaceValueChangeTime = 0; 

int swStableValue = -1; 

int swLargeSurfacePin =  A0; 
int swSmallSurfacePin = 	A2; 
int swLowValPin =      A1; 
int swValueStableTime = 200; 

int swLowTreshold = 980; 
int swHighTreshold = 1000; 



//Neopixel
int npPin = A3; 
const int npNum = 6;  
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(npNum, npPin, NEO_GRB + NEO_KHZ800);
const uint32_t npSharpColor = pixels.Color(200,200,200); 
const uint32_t npWarmColor  = pixels.Color(160,130,20); 


void setup() {
  //Switch sensing

  pinMode(swLargeSurfacePin, INPUT);
  pinMode(swSmallSurfacePin, INPUT);

  //Set pullup resistors to pins
  digitalWrite(swLargeSurfacePin, HIGH);
  digitalWrite(swSmallSurfacePin, HIGH);

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
  /*
  Serial.print("Large surface pin: "); 
  Serial.print( analogRead(swLargeSurfacePin)); 
  
  Serial.print(" - Small surface pin: "); 
  Serial.println( analogRead(swSmallSurfacePin)); 
  */
  

  updateColor(); 

  //Read value changes
  if (!swLargeSurfaceNextValue && analogRead(swLargeSurfacePin) < swLowTreshold){
    swLargeSurfaceNextValue = true; 
    swLargeSurfaceValueChangeTime = millis(); 

  }else if (swLargeSurfaceNextValue && analogRead(swLargeSurfacePin) > swHighTreshold){
    swLargeSurfaceNextValue = false; 
    swLargeSurfaceValueChangeTime = millis(); 
  }


  if (swLargeSurfaceValue != swLargeSurfaceNextValue && (millis() - swLargeSurfaceValueChangeTime > swValueStableTime)){
    swLargeSurfaceValue = swLargeSurfaceNextValue; 


    if (swLargeSurfaceValue){
      swStableValue = 2; 
      setColor(npWarmColor); //Warm light
      Serial.println("Warm light"); 
    }else{
      if (!swSmallSurfaceValue){
        swStableValue  = 0; 
        setColor(0,0,0); //Off
        Serial.println("Off"); 
      }else{
        swStableValue  = 1; 
        setColor(npSharpColor); //
        Serial.println("Sharp light"); 
      } 
    }
  }


  //Read value changes
  if (!swSmallSurfaceNextValue && analogRead(swSmallSurfacePin) < swLowTreshold){
    swSmallSurfaceNextValue = true; 
    swSmallSurfaceValueChangeTime = millis(); 

  }else if (swSmallSurfaceNextValue && analogRead(swSmallSurfacePin) > swHighTreshold){
    swSmallSurfaceNextValue = false; 
    swSmallSurfaceValueChangeTime = millis(); 
  }

  if (swSmallSurfaceValue != swSmallSurfaceNextValue && (millis() - swSmallSurfaceValueChangeTime > swValueStableTime)){
    swSmallSurfaceValue = swSmallSurfaceNextValue; 

    Serial.print("Small stable: "); 
    Serial.println(swSmallSurfaceValue); 

    if (swSmallSurfaceValue){
      swStableValue = 1; 
      setColor(npSharpColor); //Sharp light
      Serial.println("Sharp light"); 
    }else{
      if (!swLargeSurfaceValue){
        swStableValue  = 0; 
        setColor(0,0,0); //Off
        Serial.println("Off"); 
      }else{
        swStableValue = 2; 
        setColor(npWarmColor); //Warm light
        Serial.println("Warm light"); 
      }
    }
  }

}

uint32_t currentColor, nextColor; 
float colorInc; 
unsigned long lastColorUpdate = millis();
const int colorUpdateInterval = 30; 
const int colorTransitionTime = 30; 
float colorTransitionSteps = (float)colorTransitionTime/colorUpdateInterval; 
int colorTransitionCurrentStep = 0; 

void setColor(uint32_t color){
  currentColor = pixels.getPixelColor(1); //Take the color of the first one
  nextColor = color; 

  colorInc = ((float)nextColor - currentColor)/colorTransitionSteps; 
  colorTransitionCurrentStep = 0; 
}

void setColor(uint8_t r,uint8_t g,uint8_t b){
  setColor(pixels.Color(r,g,b)); 

}

void updateColor(){

  if (millis() - lastColorUpdate > colorUpdateInterval){
    lastColorUpdate = millis();
    if (colorTransitionCurrentStep < colorTransitionSteps){
      colorTransitionCurrentStep++;
      currentColor+=colorInc; 

    }
  }

  for(int i=0;i<npNum;i++){
    pixels.setPixelColor(i, currentColor); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}

