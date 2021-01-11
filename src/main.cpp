#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#else
  #include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <SimpleTimer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Ticker.h>
#include <Bounce2.h>
#include <FastLED.h>
#include "SinricPro.h"
#include "SinricProContactsensor.h"
#include "OneButton.h"

#define appName       "GateKeeper!"
#define ENABLE_DEBUG
#define CONFIG_BUTTON 4
#define GATE_SENSOR   13

#define NUM_LEDS 1
#define DATA_PIN 14

#define PAST_TIME(var, time)    (var + time < millis())
#define SENSORGATE_CLOSED 0
#define SENSORGATE_OPENED 1

/****************************************/
enum MACHINE_GATE_E {
  FSM_GATE_NONE,
  FSM_GATE_CONFIG,
  FSM_GATE_CLOSED,
  FSM_GATE_OPENED,
  FSM_GATE_OPENED_VERIFY,
  FSM_GATE_OPENED_NOTIFY,
  FSM_GATE_OPENED_NOTIFIED
};

enum MACHINE_APP_E {
  FSM_APP_NONE,
  FSM_APP_NORMAL,
  FSM_APP_SNOOZE,
  FSM_APP_SNOOZE_VERIFY,
  FSM_APP_WARNING
};

typedef enum MACHINE_GATE_E fsm_gate_t;
typedef enum MACHINE_APP_E fsm_app_t;

fsm_gate_t machine_gate;
fsm_app_t machine_app;

time_t last_time_gate;
time_t last_time_app;
/****************************************/

CRGB leds[NUM_LEDS];
volatile int watchdogCount;
volatile int StageMachineControl;
volatile bool isBtnSnoozedPressed;
unsigned long gateTimer;
unsigned long snoozeTimer;
char waitTimer[3];
char alexa_app_key[100];
char alexa_app_secret[100];
char alexa_sensor_id[100];
char telegramID_1[20];
char telegramID_2[20];
char telegramID_3[20];

bool myPowerState     = true;
bool shouldSaveConfig = false;
Bounce debouncerGate  = Bounce();

AsyncWebServer  server(80);
DNSServer       dns;
Ticker          tickerDOG;
SimpleTimer     timer;
OneButton       button(CONFIG_BUTTON, true, true);

void saveConfigCallback (void) {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback(AsyncWiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  // ticker.attach(0.2, ledVerdeTick);
}

void setupConfigPortal(bool resetConfiguration = false) {
  Serial.println("Setup Config Portal");
  // ticker.attach(0.6, ledVerdeTick);
  Serial.println("mounting FS...");

  if (LittleFS.begin()) {
    // LittleFS.format(); // Must use if change config.json
    Serial.println("mounted file system");
    if (LittleFS.exists("/config.json")) {
      Serial.println("reading config file");
      File configFile = LittleFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, configFile);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          return;
        }
        serializeJson(doc, Serial);
        Serial.println();

        strcpy(waitTimer, doc["waittimer"]);
        strcpy(alexa_app_key, doc["alexa_app_key"]);
        strcpy(alexa_app_secret, doc["alexa_app_secret"]);
        strcpy(alexa_sensor_id, doc["alexa_sensor_id"]);
        strcpy(telegramID_1, doc["telegramID_1"]);
        strcpy(telegramID_2, doc["telegramID_2"]);
        strcpy(telegramID_3, doc["telegramID_3"]);
      }
      configFile.close();
    }
  } else {
    Serial.println("failed to mount FS");
  }

  AsyncWiFiManagerParameter custom_alexa_app_key("alexa_app_key", "Sinric APP Key", alexa_app_key, 100);
  AsyncWiFiManagerParameter custom_alexa_app_secret("alexa_app_secret", "Sinric APP Secret", alexa_app_secret, 100);
  AsyncWiFiManagerParameter custom_alexa_sensor_id("alexa_sensor_id", "Sinric Sensor ID", alexa_sensor_id, 100);
  AsyncWiFiManagerParameter custom_waittimer("waittimer", "Wait Timer", waitTimer, 3);
  AsyncWiFiManagerParameter custom_telegramID_1("telegramID_1", "Telegram ID", telegramID_1, 20);
  AsyncWiFiManagerParameter custom_telegramID_2("telegramID_2", "Telegram ID", telegramID_2, 20);
  AsyncWiFiManagerParameter custom_telegramID_3("telegramID_3", "Telegram ID", telegramID_3, 20);

  AsyncWiFiManager wifiManager(&server,&dns);
  if(resetConfiguration) {
    wifiManager.resetSettings();
  }

  wifiManager.setTimeout(300);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_alexa_app_key);
  wifiManager.addParameter(&custom_alexa_app_secret);
  wifiManager.addParameter(&custom_alexa_sensor_id);
  wifiManager.addParameter(&custom_waittimer);
  wifiManager.addParameter(&custom_telegramID_1);
  wifiManager.addParameter(&custom_telegramID_2);
  wifiManager.addParameter(&custom_telegramID_3);

  if(!wifiManager.autoConnect("GateKeeperIOT")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset(); //reset and try again, or maybe put it to deep sleep
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  // ledStatus(_STATUS_WIFIOK);

  strcpy(waitTimer, custom_waittimer.getValue());
  strcpy(alexa_app_key, custom_alexa_app_key.getValue());
  strcpy(alexa_app_secret, custom_alexa_app_secret.getValue());
  strcpy(alexa_sensor_id, custom_alexa_sensor_id.getValue());
  strcpy(telegramID_1, custom_telegramID_1.getValue());
  strcpy(telegramID_2, custom_telegramID_2.getValue());
  strcpy(telegramID_3, custom_telegramID_3.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument doc(1024);
    doc["waittimer"]       = waitTimer;
    doc["alexa_app_key"]   = alexa_app_key;
    doc["alexa_app_secret"]= alexa_app_secret;
    doc["alexa_sensor_id"] = alexa_sensor_id;
    doc["telegramID_1"]    = telegramID_1;
    doc["telegramID_2"]    = telegramID_2;
    doc["telegramID_3"]    = telegramID_3;

    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    serializeJson(doc, Serial);
    serializeJson(doc, configFile);

    configFile.close();
    //end save
  }
}

void softReset(void) {
  Serial.println("RESET");
  ESP.reset();
}

void doubleclick(void) {
  setupConfigPortal(true);
  softReset();
};

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
  myPowerState = state;
  return true; // request handled properly
}

void setupSinricPro(void) {
  SinricProContactsensor& myContact = SinricPro[alexa_sensor_id];
  myContact.onPowerState(onPowerState);

  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(alexa_app_key, alexa_app_secret);
  SinricPro.isConnected();
}

void ISRwatchdog(void){
  watchdogCount++;
  if(watchdogCount == 5) {
    Serial.println();
    Serial.println("the watchdog bites!!");
    ESP.reset();
  }
}

void buttonSnoozedPress(void) {
  isBtnSnoozedPressed = true;
}

void setup() {
  Serial.begin(115200);
  for (size_t i = 0; i < 3; i++)
  {
    // pinMode(statusLED[i], OUTPUT);
    // digitalWrite(statusLED[i], LOW);
  }

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  // ledStatus(_STATUS_START);
  // leds[0] = CRGB::Blue;
  // FastLED.show();

  setupConfigPortal();

  button.attachClick(buttonSnoozedPress);
  button.attachDoubleClick(doubleclick);
  button.attachLongPressStop(softReset);

  pinMode(GATE_SENSOR, INPUT_PULLUP);
  debouncerGate.attach(GATE_SENSOR);
  debouncerGate.interval(10);

  Serial.printf("Gatekeeper time set to %i minutes\n", atoi(waitTimer));
  // timerNotification = timer.setInterval( atoi(waitTimer) * 60000 , repeatMe);
  // timerNotification = timer.setInterval( 30000 , repeatMe);
  // ledBlinkTimer = timer.setInterval(500, ledBlink);
  // timer.disable(timerNotification);
  // timer.disable(ledBlinkTimer);

  // ticker.detach();
  // digitalWrite(statusLED[0], HIGH);

  tickerDOG.attach(10, ISRwatchdog);
  setupSinricPro();
  // ledStatus(_STATUS_CLOSED);
}

void sendNotify(void) {
  // Manda a notificacao, para a alexa, para o Telegrama
  // zera variaveis referente a notificacao
}

void behaviorgate() {
  switch (machine_gate) {
    case FSM_GATE_OPENED:
      last_time_gate = millis();
      Serial.println("last_time_gate ");
      Serial.println(last_time_gate);
      Serial.println("FSM_GATE_OPENED");
      break;

    case FSM_GATE_CLOSED:
      /* mandar noti fechado se app = WARNING */
      /* resetar estado do app */
      Serial.println("FSM_GATE_CLOSED");
      break;

    case FSM_GATE_OPENED_NOTIFY:
      last_time_gate = millis();
      Serial.println("FSM_GATE_OPENED_NOTIFY");
      /* notificar */
      break;
  }
}

void behaviorapp() {
  switch (machine_app) {
    case FSM_APP_SNOOZE:
      last_time_app = millis();
      isBtnSnoozedPressed = false;
      Serial.println("FSM_APP_SNOOZE");
      break;

    case FSM_APP_SNOOZE_VERIFY:
      Serial.println("FSM_APP_SNOOZE_VERIFY");
      break;

    case FSM_APP_WARNING:
      Serial.println("FSM_APP_WARNING");
      break;

    default:
      Serial.println("DEFAULT APP");
      break;
  }
}

void loop() {
  int gateSensor = debouncerGate.read();
  Serial.println(gateSensor);
  watchdogCount = 0;

  SinricPro.handle();
  debouncerGate.update();
  button.tick();
  timer.run();

  if (machine_gate == FSM_GATE_NONE) {
    machine_gate = FSM_GATE_CONFIG;
  }
  else if (machine_gate == FSM_GATE_CONFIG && gateSensor == SENSORGATE_CLOSED/* portao fechado */) {
    machine_gate = FSM_GATE_CLOSED;
  }
  else if (machine_gate == FSM_GATE_OPENED && gateSensor == SENSORGATE_CLOSED/* portao fechado */) {
    machine_gate = FSM_GATE_CLOSED;
  }
  else if (machine_gate == FSM_GATE_CONFIG && gateSensor == SENSORGATE_OPENED/* portao aberto */) {
    machine_gate = FSM_GATE_OPENED;
  }
  else if (machine_gate == FSM_GATE_OPENED_VERIFY && gateSensor == SENSORGATE_CLOSED/* portao fechado */) {
    machine_gate = FSM_GATE_CLOSED;
  }
  else if (machine_gate == FSM_GATE_CLOSED && gateSensor == SENSORGATE_OPENED/* portao aberto */) {
    machine_gate = FSM_GATE_OPENED;
  }
  else if (machine_gate == FSM_GATE_OPENED) {
    machine_gate == FSM_GATE_OPENED_VERIFY;
  }
  else if (machine_gate == FSM_GATE_OPENED_VERIFY && machine_app != FSM_APP_SNOOZE_VERIFY && PAST_TIME(last_time_gate, 10000)) {  // acertar o tempo
    machine_gate = FSM_GATE_OPENED_NOTIFY;
  }
  else if (machine_gate == FSM_GATE_OPENED_NOTIFY) {
    machine_gate = FSM_GATE_OPENED_NOTIFIED;
  }
  else if (machine_gate == FSM_GATE_OPENED_NOTIFIED && PAST_TIME(last_time_gate, 10000)) {  // acertar o tempo
    machine_gate = FSM_GATE_OPENED_NOTIFY;
  }
  else if (machine_gate == FSM_GATE_OPENED_NOTIFIED && gateSensor == SENSORGATE_CLOSED /* portao fechado */) {
    machine_gate = FSM_GATE_CLOSED;
  }

  behaviorgate();

  if(machine_app == FSM_APP_NONE) {
    machine_app = FSM_APP_NORMAL;
  }
  else if (machine_app == FSM_APP_NORMAL && machine_gate == FSM_GATE_OPENED_NOTIFY) {
    machine_app = FSM_APP_WARNING;
  }
  else if (machine_app == FSM_APP_WARNING && machine_gate == FSM_GATE_CLOSED) {
    machine_app = FSM_APP_NORMAL;
  }
  else if (machine_app == FSM_APP_NORMAL && isBtnSnoozedPressed/*apertou botao de soneca */ ) {
    machine_app = FSM_APP_SNOOZE;
  }
  else if (machine_app == FSM_APP_SNOOZE) {
    machine_app = FSM_APP_SNOOZE_VERIFY;
  }
  else if (machine_app == FSM_APP_SNOOZE_VERIFY && PAST_TIME(last_time_app, 10000)) { // acertar o tempo
    machine_app = FSM_APP_NORMAL;
  }

  behaviorapp();

  delay(3000);
}


