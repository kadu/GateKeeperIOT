#include <OneButton.h>

#define BUTTON_PIN 4

OneButton button(BUTTON_PIN, true, true);

void button_click() {
  Serial.println("x1");
}

void button_doubleClick() {
  Serial.println("x2");
}

void button_setup() {
  button.attachClick(button_click);
  button.attachDoubleClick(button_doubleClick);
}

void button_loop() {
  button.tick();
}
