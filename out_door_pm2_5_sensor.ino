// ESP8266 only

//#define ENABLE_DEBUG
//#define SINRICPRO_NOSSL

#ifdef ENABLE_DEBUG
   #define DEBUG_ESP_PORT Serial
   #define NODEBUG_WEBSOCKETS
   #define NDEBUG
#endif 

#include <Arduino.h>
#include <ArduinoOTA.h>

#include <ESP8266WiFi.h> 
#include <ESP8266mDNS.h>

#include "SinricPro.h"
#include "SinricProAirQualitySensor.h"
#include "pms3003.h"
#include "Telnet.h"     // https://github.com/sivar2311/AsyncTelnet with ESP8266 modifications
#include "Indicator.h"

#define APP_KEY    ""
#define APP_SECRET ""
#define DEVICE_ID  ""

#define WIFI_SSID ""
#define WIFI_PASS ""

#define HOSTNAME  "OutsideAQS"

#define BAUD_RATE 115200
#define UPLOAD_INTERVAL  60000 * 3 // 3 mins
//#define PROD 1 

#ifdef PROD
  #define DEBUG_PRINTLN(x)  Telnet.println(x)
  #define DEBUG_PRINTF_P(fmt, ...) Telnet.printf_P((PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#else
  #define DEBUG_PRINTLN(x)  Serial.println (x)
  #define DEBUG_PRINTF_P(fmt, ...) Serial.printf_P((PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#endif

PMS3003 pms;
Indicator indicator;
SinricProAirQualitySensor& sensor = SinricPro[DEVICE_ID];

void sendPM25Level() {
  if (SinricPro.isConnected() == false) {
    DEBUG_PRINTF_P("Not connected to SinricPro. Bail..\r\n");
    return; 
  }

  // Send PM2.5 once a UPLOAD_INTERVAL.
  static unsigned long last_millis;
  unsigned long        current_millis = millis();
  if (last_millis && current_millis - last_millis < UPLOAD_INTERVAL) return;
  last_millis = current_millis;

  int pm10 = 0;
  int pm1 = 0;
  int pm2_5 = pms.get_pm25();

  DEBUG_PRINTF_P("PM 2.5 level: %u\r\n", pm2_5); 

  // set the LED indicator
  indicator.setIndicator(pm2_5);      
  
  if(pm2_5 > 0) {
    sensor.sendAirQualityEvent(pm1, pm2_5, pm10, "PERIODIC_POLL");  
  } else {
    DEBUG_PRINTF_P("Got an invalid PM2.5 reading: %u\r\n", pm2_5);
  }
}

void setupMDNS() {
  if (!MDNS.begin(HOSTNAME)) {
    Serial.println("[setupMDNS]: Error setting up MDNS responder!");
    return;
  }

  MDNS.addService("tcp", "telnet", 23);
  Serial.println("[setupMDNS]: mDNS responder started");
}

void setupTelnet() {
  Serial.printf("[setupLED]: Setup Telnet on port 23.. \r\n");
  Telnet.begin(23);
}

void setupOTA() { 
  Serial.printf("[setupOTA]: Setup OTA.. \r\n");
  
  ArduinoOTA.setHostname(HOSTNAME);  

  ArduinoOTA.onStart([]() {
    DEBUG_PRINTF_P("[setupOTA]: Start\r\n");
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_PRINTF_P("[setupOTA]: End\r\n");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_PRINTF_P("[setupOTA]: Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_PRINTF_P("[setupOTA]: Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.printf("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.printf("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.printf("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.printf("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.printf("End Failed");
  });
  ArduinoOTA.begin();
}

void setupLED() {
  Serial.printf("[setupLED]: Setup LED.. \r\n");
  indicator.begin();
}

void setupSinricPro() {
  Serial.printf("[setupSinricPro]: Setup SinricPro.. \r\n");
  SinricPro.onConnected([](){ Serial.printf("[setupSinricPro]: Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("[setupSinricPro]: Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
} 

void setupWiFi() {
  Serial.printf("[setupWiFi]: Connecting.. \r\n");
  WiFi.mode(WIFI_STA);

  #if defined(ARDUINO_ARCH_ESP8266)
    WiFi.forceSleepWake();
    delay(200);
  #endif
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("Connection Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSensor() {
  Serial.printf("[setupSinricPro]: Setup Sensors.. \r\n");
  pms.begin();
}
 
void setup() {
  Serial.begin(BAUD_RATE);
  while (!Serial) {};

  setupOTA();
  setupLED();
  setupSensor();
  setupWiFi();
  setupMDNS();
  setupTelnet();
  setupSinricPro();  
}

void loop(void) {
  MDNS.update();
  ArduinoOTA.handle();
  SinricPro.handle();  
  sendPM25Level();  
}                  
