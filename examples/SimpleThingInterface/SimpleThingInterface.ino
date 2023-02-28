#include <Arduino.h>
#include <ThingManagerConfig.h>
#include <ThingManager.h>

#ifdef DEBUGGING
#include <TestsThingManager.h>
#endif

using namespace ThingManager;
AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

// Comment this if you are not using a display
// #define SSD1306_ATTACHED
// #define EXTERNAL_BUTTON_ATTACHED

#ifdef SSD1306_ATTACHED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#ifdef ESP8266
// Mini Display for ESP8266
#include "Adafruit_SSD1306_mini.h" // Change here if you are not using the mini display
#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#endif
#ifdef ESP32
#include <Adafruit_SSD1306.h>
#endif
#include "OledTable.h"
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
OledTable table(&display, 3, 1);
#define XPOS    0
#define YPOS    1
#define DELTAY  2
#endif

#ifdef EXTERNAL_BUTTON_ATTACHED
#include <Button2.h>
#define BUTTON_PIN 0
Button2 button;
void setupButton(void);
void buttonHandler(Button2& btn);
#endif


#ifdef SSD1306_ATTACHED
void setupDisplay(void);
void displayDummyData(void);
void displayWiFiState(void);
#endif

#ifdef ARDUINO_M5STACK_Core2
#include <M5Core2.h>
#endif

void setup() 
{ //===============================================================================
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
#ifdef SSD1306_ATTACHED
  setupDisplay();
#endif
#ifdef EXTERNAL_BUTTON_ATTACHED
  setupButton();
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
#ifdef SSD1306_ATTACHED
    displayWiFiState();
#endif
    int devNum = WiFi.softAPgetStationNum();
    int prevDevNum = devNum;
    
    while (true)
    {
      if (devNum != prevDevNum) 
      {
#ifdef SSD1306_ATTACHED
        displayWiFiState();
#endif
        DBG.printf("Devices connected: %i", devNum);
        prevDevNum = devNum;
      }
      devNum = WiFi.softAPgetStationNum();
#ifndef ARDUINO_M5STACK_Core2
      blinkOneTime(500, true);
#endif
#ifdef EXTERNAL_BUTTON_ATTACHED
      button.loop();
#endif
      yield();
    }
  }

#ifdef SSD1306_ATTACHED
  // Waiting here for WiFi connection
  table.clear();
  table.setText(0, 0, "hotspot");
  table.setText(1, 0, "search");
  table.setText(2, 0, "...");
  while (! checkConnectionToWifiStation() ) blinkOneTime(1000, false);
  displayWiFiState();
#endif


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

#ifdef SSD1306_ATTACHED
  table.clear();
#endif

}

const unsigned long RECONNECT_INTERVAL = 10000;
unsigned long previousMillis = RECONNECT_INTERVAL * 2;

void loop() 
{
#ifdef DEBUGGING
  // aunit::TestRunner::run();
#endif
#ifdef EXTERNAL_BUTTON_ATTACHED
  button.loop();
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
#ifdef EXTERNAL_BUTTON_ATTACHED
void buttonHandler(Button2& btn) 
{
   if (btn == button) 
   {
    switch ( btn.getType() ) 
    {
        case single_click:
            DBG.println(F("single click"));
#ifdef SSD1306_ATTACHED
            table.clear();
            displayDummyData();
#endif
            
            break;
        case double_click:
            DBG.println(F("double click"));
#ifdef SSD1306_ATTACHED
            table.clear();
            displayWiFiState();
#endif
            break;
        case triple_click:
            DBG.println(F("triple click"));
            break;
        case long_click:
            DBG.println(F("long click"));
            // Set AP-Mode:
            writeFile(LittleFS, getPath(PARAM_WIFI_MODE).c_str(), "WIFI_AP_MODE");
            // wipeLittleFSFiles();
#ifdef SSD1306_ATTACHED
            table.clear();
            table.setText(0, 0, "Reboot" );
            delay(1000);
            table.setText(1, 0,"3");
            delay(500);
            table.setText(1, 0,"3 2");
            delay(500);
            table.setText(1, 0,"3 2 1");
            delay(500);
            table.setText(2, 0,"GO!");
            delay(1000);
#endif
            ESP.restart();
            break;
        case empty:
            DBG.print("empty ");
            break;
    }
   }
}

void setupButton()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button.begin(BUTTON_PIN);
  button.setDebounceTime(40);
  button.setDoubleClickTime(500);
  button.setLongClickTime(1000);
  button.setClickHandler(buttonHandler);
  button.setDoubleClickHandler(buttonHandler);
  button.setLongClickHandler(buttonHandler);
}
#endif

#ifndef ARDUINO_M5STACK_Core2
      void blinkOneTime(int blinkTime, bool shouldNotBlock = false)
{
  static int ledState = LOW;  // ledState used to set the LED
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

#if defined ESP32 || ! defined ARDUINO_M5STACK_Core2
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
#endif


#ifdef SSD1306_ATTACHED
void setupDisplay() 
{
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // Clear the buffer.
  display.clearDisplay();
  // init table
  table.init();
  table.clear();
  // table.drawLines();
  DBG.println();
 
  delay(2000);
}

void displayDummyData() 
{
  String tempStr = "Temp:";
  String humStr = "Hum:";
  int dummyTemp = random(-20, 40);
  int dummyHum = random(0, 100);
  tempStr.concat(dummyTemp);
  humStr.concat(dummyHum);
  table.clear();
  table.setText(0, 0, String("Dummy Data"));
  table.setText(1, 0, tempStr);
  table.setText(2, 0, humStr);

  DBG.printf("Temp: %d\n", dummyTemp);
  DBG.printf("Hum: %d\n",  dummyHum);
}

void displayWiFiState()
{
  WiFiMode_t state = WiFi.getMode();
  String ssid = "no ssid";
  String devCon = "dev: ";
  IPAddress ip(0, 0, 0, 0);
  uint8_t devNum = 0;
  String ipStr;
  table.clear();
  switch (state)
  {
    case WIFI_AP:
      table.setText(0, 0, "WIFI_AP");
      ssid = WiFi.softAPSSID();
      devNum = WiFi.softAPgetStationNum();
      devCon.concat(devNum);
      table.setText(1, 0, ssid.substring(0, 8) );
      table.setText(2, 0, devCon);
      break;
    case WIFI_STA:
      table.setText(0, 0, "WIFI_STA");
      ssid = WiFi.SSID();
      table.setText(1, 0, ssid.substring(0, 8) );
      break;
    // case WIFI_AP_STA:
    //   table.setText(0, 0, "WIFI_AP_STA");
    //   break;
    case WIFI_OFF:
      table.setText(1, 0, "WIFI_OFF");
      break;
      
    default:
      break;
  }
}
#endif


