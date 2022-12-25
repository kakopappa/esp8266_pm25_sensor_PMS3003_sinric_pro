#include "Indicator.h"

void Indicator::setIndicator(int pm25) {
  if(pm25 == 0) { 
    // PURPLE (bad reading)
    pixels.setPixelColor(0, pixels.Color(255,0,255));
  }
  else if(pm25 < 41) {
    // GREEN  
    pixels.setPixelColor(0, pixels.Color(0,255,0));
  } else if(pm25 <47) {
    // YELLOW
    pixels.setPixelColor(0, pixels.Color(255,255,0));
  }  else if(pm25 <48) {
    // ORANGE
    pixels.setPixelColor(0, pixels.Color(255,75,0));
  } else {
    // RED  
    pixels.setPixelColor(0, pixels.Color(255,0,0));
  }
  
  pixels.show();
}

void Indicator::begin() {
  Serial.printf("[Indicator::begin()]: Setup LED.. \r\n");  
  pixels.begin();
  pixels.setBrightness(50);  
}
