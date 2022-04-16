#include <main.h>

void setup() {
  Serial.begin(115200);
  leds_setup();
  button_setup();
  wifi_setup();
  FastLED.setBrightness(50);
  leds[0] = CRGB::Blue;
  FastLED.show();
}

void loop() {
  button_loop();
  wifi_loop();
}
