#pragma once
//#include <FastLED.h>
#include <WS2812FX.h>

#define NUM_LEDS 1
#define DATA_PIN 14
#define TIMER_MS 5000

WS2812FX ws2812fx = WS2812FX(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

void leds_setup() {
  ws2812fx.init();
  ws2812fx.setBrightness(255);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();
  ws2812fx.service();
}

void leds_show() {
  ws2812fx.service();
}

void leds_portao_aberto() {
  ws2812fx.setMode(FX_MODE_BREATH);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(RED);
  ws2812fx.service();
}

void leds_portao_fechado() {
  ws2812fx.setMode(FX_MODE_STROBE);
  ws2812fx.setSpeed(10688);
  ws2812fx.setColor(GREEN);
  ws2812fx.service();
}

void leds_portao_aberto_soneca() {
  ws2812fx.setMode(FX_MODE_BREATH);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(ws2812fx.Color(97,35,0));
  ws2812fx.service();
}

void leds_wifi_ok() {
  ws2812fx.setMode(FX_MODE_STROBE);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(BLUE);
  ws2812fx.service();
}

void leds_wifi_config() {
  ws2812fx.setMode(FX_MODE_FADE);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(ws2812fx.Color(252,247,0));
  ws2812fx.service();
}