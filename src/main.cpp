#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#else
  #include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <PushbulletAPI.hpp>
#include <SimpleTimer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <Ticker.h>
#include <Bounce2.h>
#include "SinricPro.h"
#include "SinricProContactsensor.h"
#include "OneButton.h"

#define appName       "GateKeeper!"
#define ENABLE_DEBUG
#define CONFIG_BUTTON D2
#define GATE_SENSOR   D1

AsyncWebServer  server(80);
DNSServer       dns;
SimpleTimer     timer;
int             timerNotification;
Ticker          ticker;
Ticker          tickerDOG;
Ticker          tickerLedBranco;
volatile int    watchdogCount;
Bounce debouncer             = Bounce();
int statusLED[3]             = {D5, D4, D3};
bool sendClosedMessage       = false;
bool myPowerState            = true;
bool shouldSaveConfig        = false;
bool snoozedNotifications    = false;
unsigned long snoozedTimeout = 0;
int countOpenGarage          = 0;

char waitTimer[3];
char email_1[100];
char email_2[100];
char email_3[100];
char email_4[100];
char email_5[100];
char alexa_app_key[100];
char alexa_app_secret[100];
char alexa_sensor_id[100];
char pb_key[100];
char pb_fingerprint[100];

PushbulletAPI *pb;
OneButton button(CONFIG_BUTTON, true, true);

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void ledVerdeTick() {
  int state = digitalRead(statusLED[0]);  // get the current state of GPIO1 pin
  digitalWrite(statusLED[0], !state);     // set pin to the opposite state
}

void ledBrancoTick() {
  int state = digitalRead(statusLED[2]);  // get the current state of GPIO1 pin
  digitalWrite(statusLED[2], !state);     // set pin to the opposite state
}

void configModeCallback(AsyncWiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, ledVerdeTick);
}

void setupConfigPortal(bool resetConfiguration = false) {
  Serial.println("Setup Config Portal");
  ticker.attach(0.6, ledVerdeTick);
  Serial.println("mounting FS...");

  if (LittleFS.begin()) {
    //LittleFS.format(); // Must use if change config.json
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
        strcpy(email_1, doc["email_1"]);
        strcpy(email_2, doc["email_2"]);
        strcpy(email_3, doc["email_3"]);
        strcpy(email_4, doc["email_4"]);
        strcpy(email_5, doc["email_5"]);
        strcpy(pb_key, doc["pb_key"]);
        strcpy(pb_fingerprint, doc["pb_fingerprint"]);
        strcpy(alexa_app_key, doc["alexa_app_key"]);
        strcpy(alexa_app_secret, doc["alexa_app_secret"]);
        strcpy(alexa_sensor_id, doc["alexa_sensor_id"]);
      }
      configFile.close();
    }
  } else {
    Serial.println("failed to mount FS");
  }

  AsyncWiFiManagerParameter custom_pb_key("pb_key", "Pushbullet Key", pb_key, 100);
  AsyncWiFiManagerParameter custom_pb_fingerprint("pb_fingerprint", "Pushbullet Fingerprint", pb_fingerprint, 100);
  AsyncWiFiManagerParameter custom_alexa_app_key("alexa_app_key", "Sinric APP Key", alexa_app_key, 100);
  AsyncWiFiManagerParameter custom_alexa_app_secret("alexa_app_secret", "Sinric APP Secret", alexa_app_secret, 100);
  AsyncWiFiManagerParameter custom_alexa_sensor_id("alexa_sensor_id", "Sinric Sensor ID", alexa_sensor_id, 100);
  AsyncWiFiManagerParameter custom_waittimer("waittimer", "Wait Timer", waitTimer, 3);
  AsyncWiFiManagerParameter custom_email_1("email_1", "Email", email_1, 100);
  AsyncWiFiManagerParameter custom_email_2("email_2", "Email (opcional)", email_2, 100);
  AsyncWiFiManagerParameter custom_email_3("email_3", "Email (opcional)", email_3, 100);
  AsyncWiFiManagerParameter custom_email_4("email_4", "Email (opcional)", email_4, 100);
  AsyncWiFiManagerParameter custom_email_5("email_5", "Email (opcional)", email_5, 100);

  AsyncWiFiManager wifiManager(&server,&dns);
  if(resetConfiguration) {
    wifiManager.resetSettings();
  }

  wifiManager.setTimeout(300);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_pb_key);
  wifiManager.addParameter(&custom_pb_fingerprint);

  wifiManager.addParameter(&custom_alexa_app_key);
  wifiManager.addParameter(&custom_alexa_app_secret);
  wifiManager.addParameter(&custom_alexa_sensor_id);

  wifiManager.addParameter(&custom_waittimer);
  wifiManager.addParameter(&custom_email_1);
  wifiManager.addParameter(&custom_email_2);
  wifiManager.addParameter(&custom_email_3);
  wifiManager.addParameter(&custom_email_4);
  wifiManager.addParameter(&custom_email_5);

  if(!wifiManager.autoConnect("GateKeeperIOT")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  strcpy(waitTimer, custom_waittimer.getValue());
  strcpy(email_1, custom_email_1.getValue());
  strcpy(email_2, custom_email_2.getValue());
  strcpy(email_3, custom_email_3.getValue());
  strcpy(email_4, custom_email_4.getValue());
  strcpy(email_5, custom_email_5.getValue());
  strcpy(pb_key, custom_pb_key.getValue());
  strcpy(pb_fingerprint, custom_pb_fingerprint.getValue());
  strcpy(alexa_app_key, custom_alexa_app_key.getValue());
  strcpy(alexa_app_secret, custom_alexa_app_secret.getValue());
  strcpy(alexa_sensor_id, custom_alexa_sensor_id.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument doc(1024);
    doc["waittimer"]       = waitTimer;
    doc["email_1"]         = email_1;
    doc["email_2"]         = email_2;
    doc["email_3"]         = email_3;
    doc["email_4"]         = email_4;
    doc["email_5"]         = email_5;
    doc["pb_key"]          = pb_key;
    doc["pb_fingerprint"]  = pb_fingerprint;
    doc["alexa_app_key"]   = alexa_app_key;
    doc["alexa_app_secret"]= alexa_app_secret;
    doc["alexa_sensor_id"] = alexa_sensor_id;

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

void softReset() {
  Serial.println("RESET");
  ESP.reset();
}

void doubleclick() {
  setupConfigPortal(true);
  softReset();
};

void snoozeNotification() {
  Serial.println("Snooze");
  snoozedNotifications = true;
  tickerLedBranco.attach(0.4, ledBrancoTick);
  snoozedTimeout = millis() + 1800000; // 30 mins
}

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
  myPowerState = state;
  return true; // request handled properly
}

void setupSinricPro() {
  SinricProContactsensor& myContact = SinricPro[alexa_sensor_id];
  myContact.onPowerState(onPowerState);

  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(alexa_app_key, alexa_app_secret);
  SinricPro.isConnected();
}

void sendNotification(String title, String message) {
  if(strlen(email_1) > 0) {
    pb->pushNotifcationEmail(title, message, email_1);
  }
  if(strlen(email_2) > 0) {
    pb->pushNotifcationEmail(title, message, email_2);
  }
  if(strlen(email_3) > 0) {
    pb->pushNotifcationEmail(title, message, email_3);
  }
  if(strlen(email_4) > 0) {
    pb->pushNotifcationEmail(title, message, email_4);
  }
  if(strlen(email_5) > 0) {
    pb->pushNotifcationEmail(title, message, email_5);
  }
}

void sendGarageClosedMessage() {
  if(!sendClosedMessage && countOpenGarage > 0) {
    sendNotification(appName, "O Portão foi fechado!");
    SinricProContactsensor &myContact = SinricPro[alexa_sensor_id];
    myContact.sendContactEvent(HIGH);
    delay(100);
  }
  sendClosedMessage = true;
  digitalWrite(statusLED[2], LOW);
  countOpenGarage = 0;
}

void repeatMe() {
  countOpenGarage++;
  int pastTime = atoi(waitTimer) * countOpenGarage;

  if(snoozedNotifications) {
    if(millis() > snoozedTimeout) {
      snoozedTimeout = 0;
      snoozedNotifications = false;
      tickerLedBranco.detach();
      digitalWrite(statusLED[2], LOW);
      sendNotification(appName, "Atenção ACORDEI e o portão ainda está aberto a " + String(pastTime) + " minutos!");
    }
  } else {
    sendNotification(appName, "Atenção o portão está aberto a " + String(pastTime) + " minutos!");
  }

  SinricProContactsensor &myContact = SinricPro[alexa_sensor_id];
  myContact.sendContactEvent(LOW);
  digitalWrite(statusLED[2], HIGH);
}

void ISRwatchdog(){
  watchdogCount++;
  if(watchdogCount == 5) {
    Serial.println();
    Serial.println("the watchdog bites!!");
    ESP.reset();
  }
}

void setup() {
  Serial.begin(115200);
  for (size_t i = 0; i < 3; i++)
  {
    pinMode(statusLED[i], OUTPUT);
    digitalWrite(statusLED[i], LOW);
  }

  setupConfigPortal();

  button.attachClick(snoozeNotification);
  button.attachDoubleClick(doubleclick);
  button.attachLongPressStop(softReset);

  pinMode(GATE_SENSOR, INPUT_PULLUP);
  debouncer.attach(GATE_SENSOR);
  debouncer.interval(10);

  Serial.printf("Gatekeeper time set to %i minutes\n", atoi(waitTimer));
  timerNotification = timer.setInterval( atoi(waitTimer) * 60000 , repeatMe);
  timer.disable(timerNotification);


  ticker.detach();
  digitalWrite(statusLED[0], HIGH);

  tickerDOG.attach(1, ISRwatchdog);
  setupSinricPro();
  pb = new PushbulletAPI(pb_key,  pb_fingerprint);
}

void loop() {
  timer.run();
  SinricPro.handle();
  debouncer.update();
  button.tick();

  watchdogCount = 0;
  int sensorValue = debouncer.read();

  digitalWrite(statusLED[1], sensorValue);
  if(!timer.isEnabled(timerNotification) && (millis() > 120000)) {
    sendGarageClosedMessage();
  }

  if(sensorValue == 1) { // Open
    if(!timer.isEnabled(timerNotification)) {
      timer.enable(timerNotification);
      sendClosedMessage = false;
    }
  } else {
    if(timer.isEnabled(timerNotification)) {
      timer.disable(timerNotification);
    }
  }
}
