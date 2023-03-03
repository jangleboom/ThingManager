#include <Arduino.h>
#include <ThingManagerConfig.h>
#include <ThingManager.h>

#ifdef DEBUGGING
#include <TestsThingManager.h>
#endif

using namespace ThingManager;
AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];



// void displayWiFiState(void);

void setup() 
{ //===============================================================================
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  delay(1000);
  DBG.println(F("Serial setup done."));
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

  // wipeLittleFSFiles();  // Use this for deleting all data without formatting
  
  #ifdef DEBUGGING
  listFiles();
  delay(1000);
  #endif
  //===============================================================================
  setupWiFi(&server);
  if (WiFi.getMode() == WIFI_AP)
  {
    // displayWiFiState();
    int devNum = WiFi.softAPgetStationNum();
    int prevDevNum = devNum;
    
    while (true)
    {
      if (devNum != prevDevNum) 
      {
// #ifdef SSD1306_ATTACHED
//         displayWiFiState();
// #endif
        DBG.printf("Devices connected: %i", devNum);
        prevDevNum = devNum;
      }
      devNum = WiFi.softAPgetStationNum();

// #ifdef EXTERNAL_BUTTON_ATTACHED
//       button.loop();
// #endif
      yield();
    }
  }

  // // Waiting here for WiFi connection
  // table.clear();
  // table.setText(0, 0, "hotspot");
  // table.setText(1, 0, "search");
  // table.setText(2, 0, "...");
  // while (! checkConnectionToWifiStation() ) blinkOneTime(1000, false);
  // displayWiFiState();



  //===============================================================================
  
  // Further setup, e. g. Sensor readings, FreeRTOS tasks ect. pp.

 
  //===============================================================================


}

const unsigned long RECONNECT_INTERVAL = 10000;
unsigned long previousMillis = RECONNECT_INTERVAL * 2;

void loop() 
{
#ifdef DEBUGGING
  // aunit::TestRunner::run();
#endif
// #ifdef EXTERNAL_BUTTON_ATTACHED
//   button.loop();
// #endif


  // If WiFi is down, try reconnecting every RECONNECT_INTERVAL seconds
  static bool connected;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > RECONNECT_INTERVAL) 
  {
    previousMillis = currentMillis;
    connected = checkConnectionToWifiStation();
  }
}

// void buttonHandler(Button2& btn) 
// {
//    if (btn == button) 
//    {
//     switch ( btn.getType() ) 
//     {
//         case single_click:
//             DBG.println(F("single click"));
// #ifdef SSD1306_ATTACHED
//             table.clear();
//             displayDummyData();
// #endif
            
//             break;
//         case double_click:
//             DBG.println(F("double click"));
//             // table.clear();
//             // displayWiFiState();
//             break;
//         case triple_click:
//             DBG.println(F("triple click"));
//             break;
//         case long_click:
//             DBG.println(F("long click"));
//             // Set AP-Mode:
//             writeFile(LittleFS, getPath(PARAM_WIFI_MODE).c_str(), "WIFI_AP_MODE");
//             // wipeLittleFSFiles();
//             // table.clear();
//             // table.setText(0, 0, "Reboot" );
//             // delay(1000);
//             // table.setText(1, 0,"3");
//             // delay(500);
//             // table.setText(1, 0,"3 2");
//             // delay(500);
//             // table.setText(1, 0,"3 2 1");
//             // delay(500);
//             // table.setText(2, 0,"GO!");
//             delay(1000);
//             ESP.restart();
//             break;
//         case empty:
//             DBG.print("empty ");
//             break;
//     }
//    }
// }

// void setupButton()
// {
//   pinMode(BUTTON_PIN, INPUT_PULLUP);
//   button.begin(BUTTON_PIN);
//   button.setDebounceTime(40);
//   button.setDoubleClickTime(500);
//   button.setLongClickTime(1000);
//   button.setClickHandler(buttonHandler);
//   button.setDoubleClickHandler(buttonHandler);
//   button.setLongClickHandler(buttonHandler);
// }


// void blinkOneTime(int blinkTime, bool shouldNotBlock=false)
// {
//   static int ledState = LOW;  // ledState used to set the LED
//   unsigned long currentMillis = millis();
//   static unsigned long previousMillis = 0;

// #if defined ESP32 
//   ledState = (ledState == HIGH) ? LOW : HIGH;
//   digitalWrite(LED_BUILTIN, ledState);
//   shouldNotBlock ? vTaskDelay(blinkTime) : delay(blinkTime);
//  #elif ESP8266
//   if (shouldNotBlock)
//   {
//     if (currentMillis - previousMillis >= (unsigned long) blinkTime) 
//     {
//     // save the last time you blinked the LED
//     previousMillis = currentMillis;
//     ledState = (ledState == HIGH) ? LOW : HIGH;
//     digitalWrite(LED_BUILTIN, ledState);
//     }
//   } 
//   else
//   {
//     delay(blinkTime);
//     ledState = (ledState == HIGH) ? LOW : HIGH;
//     digitalWrite(LED_BUILTIN, ledState);
//   }
// #endif
// }






// void displayWiFiState()
// {
//   WiFiMode_t state = WiFi.getMode();
//   String ssid = "no ssid";
//   String devCon = "dev: ";
//   IPAddress ip(0, 0, 0, 0);
//   uint8_t devNum = 0;
//   String ipStr;
//   table.clear();
//   switch (state)
//   {
//     case WIFI_AP:
//       table.setText(0, 0, "WIFI_AP");
//       ssid = WiFi.softAPSSID();
//       devNum = WiFi.softAPgetStationNum();
//       devCon.concat(devNum);
//       table.setText(1, 0, ssid.substring(0, 8) );
//       table.setText(2, 0, devCon);
//       break;
//     case WIFI_STA:
//       table.setText(0, 0, "WIFI_STA");
//       ssid = WiFi.SSID();
//       table.setText(1, 0, ssid.substring(0, 8) );
//       break;
//     // case WIFI_AP_STA:
//     //   table.setText(0, 0, "WIFI_AP_STA");
//     //   break;
//     case WIFI_OFF:
//       table.setText(1, 0, "WIFI_OFF");
//       break;
      
//     default:
//       break;
//   }
// }



