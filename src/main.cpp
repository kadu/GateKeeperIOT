#include <main.h>

void btnSnooze() {
  Serial.println("\n\nevento Snooze");
  fsm.trigger(BUTTON_EVENT_SNOOZE);
  isSnoozed = true;
  leds[0] = CRGB::Orange;
}

void btnSnooze_dblClick() {
  Serial.println("Chama funções de configuração");
}

void gateSensorClick() {
  Serial.println("Button 2 click.");
}

void gateSensorCloseEvent() {
  Serial.println("\n\nevento Portão fechado.");
  isSnoozed = false;

  if(hasNotified) {
    fsm.trigger(BUTTON_EVENT_CLOSE_NOTIFY);
  } else {
    fsm.trigger(BUTTON_EVENT_CLOSE);
  }
}

void gateSensorOpenEvent() {
  Serial.println("\n\nevento Portão aberto.");
  fsm.trigger(BUTTON_EVENT_OPEN);
}

void gateOpen_enter()
{
  Serial.println("Portão aberto (enter).");
  gateOpenMillis = millis();
  fauxmo.setState(device_id_gk , true , 200);
  if (!isSnoozed ) {
    leds[0] = CRGB::Red;
  } else {
    leds[0] = CRGB::DarkSalmon;
  }
}

void gateOpen_on() {
  if(gateOpenMillis > 0) {
    if(millis() - gateOpenMillis >= _notify_interval) {
        isSnoozed = false;
        String convert = CONFIG_NORMAL_ALERT_TIME;
        int interval = convert.toFloat();
      _notify_interval = interval * 60000;
      fsm.trigger(EVENT_NOTIFY);
    }
  }
}

void gateOpen_exit() {
  Serial.println("Portão aberto (exit).");
  gateOpenMillis = 0;
}

void gateClose()
{
  hasNotified = false;
  Serial.println("estado Portão fechado.");
  fauxmo.setState(device_id_gk , false , 1);

  leds[0] = CRGB::LightCoral;
}

void notify() {
  Serial.println("envia notificação");
  hasNotified = true;
  fsm.trigger(EVENT_NOTIFY_OPEN);
}

void snooze() {
  String convert = CONFIG_SNOOZE_TIME;
  int interval = convert.toInt();
  Serial.println("aciona o soneca");
  _notify_interval = interval * 60000;
  fsm.trigger(BUTTON_EVENT_SNOOZE_EXIT);
}

void notify_close() {
  Serial.println("envia notificação que fechou o portão");
  fsm.trigger(BUTTON_EVENT_CLOSE_NOTIFY_CLOSE);
}

void button_setup() {
  btnConfig.attachClick(btnSnooze);
  btnConfig.attachDoubleClick(btnSnooze_dblClick);
  btnPortao.attachClick(gateSensorClick);
  btnPortao.attachLongPressStart(gateSensorCloseEvent);
  btnPortao.attachLongPressStop(gateSensorOpenEvent);
}

void button_loop() {
  btnConfig.tick();
  btnPortao.tick();
}

void setupFSM() {
  fsm.add_transition(&state_gateClose,   &state_gateOpen,  BUTTON_EVENT_OPEN, NULL);
  fsm.add_transition(&state_gateOpen,    &state_gateClose, BUTTON_EVENT_CLOSE, NULL);
  fsm.add_transition(&state_gateOpen,    &state_Notify,    EVENT_NOTIFY, NULL);
  fsm.add_transition(&state_Notify,      &state_gateOpen,  EVENT_NOTIFY_OPEN, NULL);
  fsm.add_transition(&state_gateOpen,    &state_CloseNotify, BUTTON_EVENT_CLOSE_NOTIFY, NULL);
  fsm.add_transition(&state_CloseNotify, &state_gateClose, BUTTON_EVENT_CLOSE_NOTIFY_CLOSE, NULL);
  fsm.add_transition(&state_gateOpen,    &state_Snooze,    BUTTON_EVENT_SNOOZE, NULL);
  fsm.add_transition(&state_Snooze,      &state_gateOpen,  BUTTON_EVENT_SNOOZE_EXIT, NULL);

  Serial.println("Setup END");
}

void setup() {
  Serial.begin(115200);
  leds_setup();
  button_setup();
  wifi_setup();
  setupFSM();
  String convert = CONFIG_NORMAL_ALERT_TIME;
  int interval = convert.toFloat();
  _notify_interval = interval * 60000;
}

void loop() {
  button_loop();
  wifi_loop();
  leds_show();
  alexa_loop();
  fsm.run_machine();
}