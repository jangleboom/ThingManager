#include <Arduino.h>
#include <ThingManagerConfig.h>
#include <ThingManager.h>
#include <Button2.h>

#ifdef DEBUGGING
#include <TestsThingManager.h>
#endif

#define BUTTON_PIN 2
Button2 button_1;
const unsigned long LONG_CLICK_INTERVAL_MS = 2000;
const unsigned long RECONNECT_INTERVAL_MS = 30000;
unsigned long previousMillis = RECONNECT_INTERVAL_MS * 2;
void btnHandler(Button2& btn);

using namespace ThingManager;
static AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];




void setup() 
{ //===============================================================================
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  delay(1000);
  DBG.println(F("Serial setup done."));
  #endif

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button_1.begin(BUTTON_PIN);
  button_1.setLongClickTime(LONG_CLICK_INTERVAL_MS);
  button_1.setClickHandler(btnHandler);
  button_1.setDoubleClickHandler(btnHandler);
  button_1.setLongClickHandler(btnHandler);
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
  delay(3000);
  #endif
  //===============================================================================
  
  
  setupWiFi(&server);
}


void loop() 
{
  #ifdef DEBUGGING
  aunit::TestRunner::run();
  #endif

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every RECONNECT_INTERVAL seconds
  if (currentMillis - previousMillis > RECONNECT_INTERVAL_MS) 
  {
    ThingManager::checkConnectionToWifiStation();
    digitalWrite(LED_BUILTIN, ( (WiFi.getMode() == WIFI_AP) ? LOW : HIGH) );
    previousMillis = currentMillis;
  }

  button_1.loop();
  // MDNS.update();
}


void btnHandler(Button2& btn) 
{
   if (btn == button_1) 
   {
    switch ( btn.getType() ) 
    {
        case single_click:
            break;
            DBG.print("single ");
        case double_click:
            DBG.print("double ");
            break;
        case triple_click:
            DBG.print("triple ");
            break;
        case long_click:
            DBG.print("long ");
            break;
        case empty:
            DBG.print("empty ");
            break;
    }
    DBG.println("click");
   }
}