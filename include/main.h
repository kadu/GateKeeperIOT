#pragma once

#include <Arduino.h>
#include <wificontrol.h>
#include <buttons.h>
#include <leds.h>
#include <OneButton.h>
#include <Fsm.h>
#include <ESPAsyncWebServer.h>

#define CONFIG_PIN 4
#define GATE_PIN 13

OneButton btnConfig(CONFIG_PIN, true, true);
OneButton btnPortao(GATE_PIN, true, true);

unsigned long gateOpenMillis = 0;
unsigned long _notify_interval = 15000;

void btnSnooze();
void btnSnooze_dblClick();
void gateSensorClick();
void doubleclick2();
void gateSensorCloseEvent();
void gateSensorOpenEvent();
void button_setup();
void button_loop();

//Events
#define BUTTON_EVENT_OPEN  0
#define EVENT_NOTIFY  1
#define BUTTON_EVENT_CLOSE 2
#define EVENT_NOTIFY_OPEN 3
#define BUTTON_EVENT_CLOSE_NOTIFY 4
#define BUTTON_EVENT_CLOSE_NOTIFY_CLOSE 5
#define BUTTON_EVENT_SNOOZE 6
#define BUTTON_EVENT_SNOOZE_EXIT 7

int gateSensorState = 0;
bool hasNotified = false;
bool isSnoozed = false;

// headers
void gateOpen_enter();
void gateOpen_exit();
void gateOpen_on();

void gateClose();
void notify();
void notify_close();
void snooze();

State state_gateOpen(&gateOpen_enter, &gateOpen_on, &gateOpen_exit);
State state_gateClose(&gateClose, NULL, NULL);
State state_Notify(NULL, &notify, NULL);
State state_CloseNotify(NULL, &notify_close, NULL);
State state_Snooze(NULL, &snooze, NULL);

Fsm fsm(&state_gateOpen);