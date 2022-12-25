#pragma once
#include <Adafruit_NeoPixel.h>

#define LED_PIN   2
#define NUMPIXELS 1

class Indicator {    
  public: 
    void setIndicator(int pm25);
    void begin();
    
  private:
     Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);
};
