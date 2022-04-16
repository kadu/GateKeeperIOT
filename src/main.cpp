#include <main.h>

void setup() {
  Serial.begin(115200);
  leds_setup();
  leds[0] = CRGB::Blue;
  FastLED.show();

  button_setup();
  wifi_setup();
  leds[0] = CRGB::Green;
}

void loop() {
  button_loop();
  wifi_loop();
  FastLED.show();
}
