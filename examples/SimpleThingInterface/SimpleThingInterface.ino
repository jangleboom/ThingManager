#include <Arduino.h>
#include <ThingManagerConfig.h>
#include <ThingManager.h>
#include <Button2.h>

#ifdef DEBUGGING
#include <TestsThingManager.h>
#endif

#define BUTTON_PIN 0
Button2 button;
using namespace ThingManager;
static AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];

void btnHandler(Button2& btn);

// Display for ESP32 (you have to add)

// Mini Display for ESP8266
#ifdef ESP8266
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306_mini.h"
#include "OledTable.h"
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
OledTable table(&display, 4, 2);
#define XPOS    0
#define YPOS    1
#define DELTAY  2
#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#endif

void setupDisplay(void);
void setupButton(void);
void printTestDisplay(void);
void blinkOneTime(int blinkTime, bool shouldNotBlock);

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

  setupDisplay();
  setupButton();
  printTestDisplay();
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
  delay(1000);
  #endif
  //===============================================================================
  bool connected = setupWiFi(&server);
  // Waiting here for WiFi connection
  while (! connected)
  {
    unsigned long currentMillis = millis();
    static unsigned long previousMillis = 0;

    if (currentMillis - previousMillis >= (unsigned long) 5000) 
    {
      previousMillis = currentMillis;
      connected = checkConnectionToWifiStation();
      DBG.printf("Wifi ready: %s\n", connected ? "yes" : "no");
    }

    blinkOneTime(1000, true);
    button.loop();
#ifdef ESP8266
    yield(); // Reset WDT on ESP8266!
#endif
  }
  //===============================================================================
  
  // Further setup, e. g. FreeRTOS tasks

  // ...

  //===============================================================================
  // Show setup finished - Turn off led
#ifdef ESP32
  digitalWrite(LED_BUILTIN, LOW);
#else
  digitalWrite(LED_BUILTIN, HIGH);
#endif

}

const unsigned long RECONNECT_INTERVAL = 10000;
unsigned long previousMillis = RECONNECT_INTERVAL * 2;

void loop() 
{
  #ifdef DEBUGGING
  aunit::TestRunner::run();
  #endif
  static bool connected;
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every RECONNECT_INTERVAL seconds
  if (currentMillis - previousMillis > RECONNECT_INTERVAL) 
  {
    previousMillis = currentMillis;
    connected = ThingManager::checkConnectionToWifiStation();
    DBG.printf("WiFi Mode: %s\n", getWiFiModeStr(WiFi.getMode()).c_str());
  }

#ifdef ESP8266
    MDNS.update();
#endif 
  // Show connection state on state led
 if ( ! connected ) 
 {
  blinkOneTime(1000, true);
 }
 else
 {
  // Turn off led
  #ifdef ESP32
  digitalWrite(LED_BUILTIN, LOW);
  #else
  digitalWrite(LED_BUILTIN, HIGH);
  #endif
 }
 
  button.loop();
}


void btnHandler(Button2& btn) 
{
   if (btn == button) 
   {
    switch ( btn.getType() ) 
    {
        case single_click:
            DBG.print("single ");
            printTestDisplay();
            break;
        case double_click:
            DBG.print("double ");
            break;
        case triple_click:
            DBG.print("triple ");
            break;
        case long_click:
            DBG.print("long ");
            // Start AP:
            // wipeLittleFSFiles();
            // delay(3000);
            // ESP.restart();
            break;
        case empty:
            DBG.print("empty ");
            break;
    }
    DBG.println("click");
   }
}

void setupButton()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  button.begin(BUTTON_PIN);
  button.setDebounceTime(40);
  button.setDoubleClickTime(500);
  button.setLongClickTime(1000);
  button.setClickHandler(btnHandler);
  button.setDoubleClickHandler(btnHandler);
  button.setLongClickHandler(btnHandler);
}

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

void printTestDisplay() 
{
  int dummyTemp = random(-20, 40);
  int dummyHum = random(0, 100);

  table.clear();
  table.setText(0, 0, "Temp:" );
  table.setText(0, 1, String(dummyTemp));
  table.setText(2, 0,"Hum: ");
  table.setText(2, 1, String(dummyHum) );

  DBG.printf("Temp: %d\n", dummyTemp);
  DBG.printf("Hum: %d\n", dummyHum);
  
}


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