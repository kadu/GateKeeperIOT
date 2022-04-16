#pragma once
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 14

CRGB leds[NUM_LEDS];

void leds_setup() { 
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}