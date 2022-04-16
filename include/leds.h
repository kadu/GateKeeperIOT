#pragma once
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 3
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];

void leds_setup() { 
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); 
}