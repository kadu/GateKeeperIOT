#include <OneButton.h>

#define CONFIG_PIN 4
#define GATE_PIN 13

OneButton btnConfig(CONFIG_PIN, true, true);
OneButton btnPortao(GATE_PIN, true, true);

void click1() {
  Serial.println("Button 1 click.");
}

void doubleclick1() {
  Serial.println("Button 1 doubleclick.");
}

void click2() {
  Serial.println("Button 2 click.");
}

void doubleclick2() {
  Serial.println("Button 2 doubleclick.");
}

void button_setup() {
  btnConfig.attachClick(click1);
  btnConfig.attachDoubleClick(doubleclick1);
  btnPortao.attachClick(click2);
  btnPortao.attachDoubleClick(doubleclick2);
}

void button_loop() {
  btnConfig.tick();
  btnPortao.tick();
}
