#include <Arduino.h>
#include "../../ThingManagerConfig.h"
#include "../../ThingManager.h"

#ifdef DEBUGGING
#include "../../TestsThingManager.h"
#endif

void blinkOneTime(int blinkTime, bool shouldNotBlock);

using namespace ThingManager;
AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

//===============================================================================

void setup() 
{ 
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  delay(1000);
  DBG.println(F("Serial setup done."));
  #endif

  pinMode(LED_BUILTIN, OUTPUT);
  // Show setup began - Turn on led
  #ifdef ESP32
  digitalWrite(LED_BUILTIN, HIGH);
  #else
  digitalWrite(LED_BUILTIN, LOW);
  #endif
  
  //===============================================================================
  // Initialize LittleFS
  // Use board_build.partitions in platformio.ini
  if (!setupLittleFS()) 
  {
    formatLittleFS();
    if (!setupLittleFS()) while (true) {}; // Here you should never hang
  }

  // Uncomment if you want to format (e. g after changing partition sizes)
  // (And dont forget to comment this again after one run ;)
  //formatLittleFS();

  //wipeLittleFSFiles();  // Use this for deleting all data without formatting
  
  #ifdef DEBUGGING
  listFiles();
  delay(1000); // Time to watch the serial output
  #endif
  //===============================================================================
  setupWiFi(&server);
  if (WiFi.getMode() == WIFI_AP)
  {
    int devNum = WiFi.softAPgetStationNum();
    int prevDevNum = devNum;
    
    while (true)
    {
      if (devNum != prevDevNum) 
      {
        prevDevNum = devNum;
      }
      devNum = WiFi.softAPgetStationNum();
      blinkOneTime(500, true);
      yield();
    }
  }

  #ifdef ESP8266
    MDNS.update();
  #endif 

  //===============================================================================
  
  // Further setup, e. g. Sensor readings, FreeRTOS tasks ect. pp.

 
  //===============================================================================
  // Show setup finished - Turn off led
#ifdef ESP32
  digitalWrite(LED_BUILTIN, LOW);
#else
  digitalWrite(LED_BUILTIN, HIGH);
#endif
}

//===============================================================================

const unsigned long RECONNECT_INTERVAL = 10000;
unsigned long previousMillis = RECONNECT_INTERVAL * 2;

void loop() 
{
#ifdef DEBUGGING
  aunit::TestRunner::run();
#endif


#ifdef ESP8266
    MDNS.update();
#endif 

  // If WiFi is down, try reconnecting every RECONNECT_INTERVAL seconds
  static bool connected;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > RECONNECT_INTERVAL) 
  {
    previousMillis = currentMillis;
    connected = checkConnectionToWifiStation();
    // Turn on/of led
#ifdef ESP32
    digitalWrite(LED_BUILTIN, connected ? LOW : HIGH);
#else
    digitalWrite(LED_BUILTIN, connected ? HIGH : LOW);
#endif
  }
}

//===============================================================================

void blinkOneTime(int blinkTime, bool shouldNotBlock = false)
{
  static int ledState = LOW;  // ledState used to set the LED
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

#ifdef ESP32
  ledState = (ledState == HIGH) ? LOW : HIGH;
  digitalWrite(LED_BUILTIN, ledState);
  shouldNotBlock ? vTaskDelay(blinkTime) : delay(blinkTime);
 #elif ESP8266
  if (shouldNotBlock)
  {
    if (currentMillis - previousMillis >= (unsigned long) blinkTime) 
    {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    ledState = (ledState == HIGH) ? LOW : HIGH;
    digitalWrite(LED_BUILTIN, ledState);
    }
  } 
  else
  {
    delay(blinkTime);
    ledState = (ledState == HIGH) ? LOW : HIGH;
    digitalWrite(LED_BUILTIN, ledState);
  }
#endif
}