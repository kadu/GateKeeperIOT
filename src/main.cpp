#include <main.h>

void setup() {
  Serial.begin(115200);
  leds_setup();
  button_setup();
  wifi_setup();
}

void loop() {
  button_loop();
  wifi_loop();
  leds_show();
}
