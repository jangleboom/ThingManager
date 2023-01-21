#include <Arduino.h>
#include <ThingManagerConfig.h>
#include <ThingManager.h>
#include <Button2.h>

#ifdef DEBUGGING
#include <TestsThingManager.h>
#endif

#define BUTTON_PIN 2
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
OledTable table(&display,4,1);
#define XPOS    0
#define YPOS    1
#define DELTAY  2
#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#endif

void setup_display(void);
void test_print_text(void);

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
  button.begin(BUTTON_PIN);
  button.setLongClickTime(1000);
  button.setClickHandler(btnHandler);
  button.setDoubleClickHandler(btnHandler);
  button.setLongClickHandler(btnHandler);

  setup_display();
  test_print_text();

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
  delay(3000);
  #endif
  //===============================================================================
  
  
  setupWiFi(&server);

  randomSeed(analogRead(0));
}

const unsigned long RECONNECT_INTERVAL = 30000;
unsigned long previousMillis = RECONNECT_INTERVAL * 2;

void loop() 
{
  #ifdef DEBUGGING
  aunit::TestRunner::run();
  #endif

  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every RECONNECT_INTERVAL seconds
  if (currentMillis - previousMillis > RECONNECT_INTERVAL) 
  {
    ThingManager::checkConnectionToWifiStation();
  #ifdef ESP32
    digitalWrite(LED_BUILTIN, ( (WiFi.getMode() == WIFI_AP) ? HIGH : LOW) );
  #else
    digitalWrite(LED_BUILTIN, ( (WiFi.getMode() == WIFI_AP) ? LOW : HIGH) );
  #endif
    DBG.printf("WiFi Mode: %i\n", WiFi.getMode());
    previousMillis = currentMillis;
  }

  button.loop();

#ifdef ESP8266
    MDNS.update();
#endif

}


void btnHandler(Button2& btn) 
{
   if (btn == button) 
   {
    switch ( btn.getType() ) 
    {
        case single_click:
            DBG.print("single ");
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

void setup_display() {
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

void test_print_text() 
{
  DBG.println(table.setText(0,0,"Temp:"));
  int dummyTemp = random(-20, 40);
  DBG.println (table.setText(1, 0, String(dummyTemp).c_str() ) );
  DBG.println(table.setText(2,0,"Hum:"));
  int dummyHum = random(0, 100);
  DBG.println(table.setText(3,0, String(dummyHum)));
}
