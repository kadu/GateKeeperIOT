#include <main.h>

void setup() {
  button_setup();
  wifi_setup();
}

void loop() {
  button_loop();
  wifi_loop();
}
