/**
 * @file      main.cpp
 * @author    jangleboom
 * @brief     Smart home device that spreads sensor data in a local network via MQTT
 * @version   0.1
 * @date      2023-02-19
 * 
 * @copyright GPLv3
 * 
 * @todo      - Subscribe to a OTA flag, that prevents from going to deep sleep
 *            - ESP32 Touch Wake Up from Deep Sleep
 * 
 */

#include <Arduino.h>
#include <ThingManagerConfig.h>
#include <ThingManager.h>
#include <PubSubClient.h>
#include <Button2.h>
#include "Helpers.h"

#ifdef DEBUGGING
#include "Tests.h"
#endif

// Deep sleep
// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000 
// Save data in RTC fast memory (8kB SRAM)
#ifdef ESP8266
uint32_t bootCount = 0;
#endif
#ifdef ESP32
RTC_DATA_ATTR uint32_t bootCount = 0;
#define TOUCH_THRESHOLD 50
void touchCallback(){}; // TODO: fill this with content
#endif

// UI
const int kButtonPin = 0;
void btnHandler(Button2& btn);
void setupButton(void);
void printWiFiState(void);
void blinkOneTime(int blinkTime, bool shouldNotBlock);

// MQTT
void publishValue(float payload, const String& pubTopic);
String getSubstring(const String& data, char separator, int index);
String cutLastSubString(const String& topic, char separator);
void reconnect(void);
void callback(char* topic, byte* payload, unsigned int length);
char* timeToString(unsigned long t);

// Global objects
Button2 button;
static AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];
WiFiClient espClient;
PubSubClient client(espClient);
String listenTopic, sleepTopic, bootCountTopic, deviceName, brokerIP;

// WiFi and data management
using namespace ThingManager;

void setup() 
{ 
#ifdef DEBUGGING
  DBG.begin(BAUD);
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

  // UI setup
  setupButton();
#ifdef ESP32
  // Setup interrupt on Touch Pad 0 (GPIO4)
  touchAttachInterrupt(T0, touchCallback, TOUCH_THRESHOLD);
  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();
#endif
  // Initialize LittleFS
  // Use board_build.partitions in platformio.ini
  if ( ! setupLittleFS() ) 
  {
    formatLittleFS();
    if ( ! setupLittleFS() ) while (true) {}; // Here you should never hang
  }

  // Uncomment if you want to format (e. g after changing partition sizes)
  // (And dont forget to comment this again after one run ;)
  //formatLittleFS();
  //wipeLittleFSFiles();  // Use this for deleting all data without formatting
  
#ifdef DEBUGGING
  listFiles();
  // delay(1000);
#endif

  // Read LittleFS data
  deviceName = readFile(LittleFS, getPath(PARAM_THING_NAME).c_str());
  sleepTopic = readFile( LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_1).c_str() );
  listenTopic = readFile( LittleFS, getPath(PARAM_MQTT_PUB_TOPIC_1).c_str() );
  bootCountTopic = readFile( LittleFS, getPath(PARAM_MQTT_PUB_TOPIC_2).c_str() );
  brokerIP = readFile(LittleFS, getPath(PARAM_MQTT_BROKER_IP).c_str());

  // Increment boot number and print it every reboot
#ifdef ESP8266
  uint32_t readVal;
  ESP.rtcUserMemoryRead(0, &readVal, sizeof(readVal));
  if (readVal != 0) bootCount = readVal;
#endif
  bootCount++;
#ifdef ESP8266
  ESP.rtcUserMemoryWrite(0, &bootCount, sizeof(bootCount));
#endif
  DBG.println("\nBoot number: " + String(bootCount));
  
  /*
  ===============================================================================
  WiFi
  ===============================================================================
  */
  // WiFi setup
  setupWiFi(&server);
  // Stop if in AP mode
  printWiFiState();
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

  if (WiFi.getMode() == WIFI_AP)
  {
    previousMillis = millis();
    int devNum = WiFi.softAPgetStationNum();
    int prevDevNum = devNum;
    
    while (true)
    {
      if (devNum != prevDevNum) 
      {
        printWiFiState();
        prevDevNum = devNum;
      }
      devNum = WiFi.softAPgetStationNum();
      blinkOneTime(2000, true);
      button.loop();
      yield();
    }
  }

  if (WiFi.getMode() == WIFI_STA)
  {
    // Waiting here for WiFi connection
    while (! checkConnectionToWifiStation() ) 
    {
      blinkOneTime(1000, true);
      button.loop();
      yield();
    }

#ifdef ESP8266
      MDNS.update();
#endif 
  }
  else
  {
    DBG.println(F("WiFi setup error, freezing..."));
    while (true) 
    {
      blinkOneTime(100, true);
      button.loop();
      yield();
    }
  }
  
  /*
  ===============================================================================
  MQTT
  ===============================================================================
  */
  
  if (sleepTopic.isEmpty() || bootCountTopic.isEmpty() || brokerIP.isEmpty() || sleepTopic.isEmpty() )
  {
    DBG.print(F("Check your MQTT setup, enter "));
    DBG.print(WiFi.softAPIP());
    DBG.print(F(" of "));
    DBG.println(WiFi.softAPSSID());
    DBG.println("MQTT setup failed");

    while (true) 
    {
      blinkOneTime(100, true);
      button.loop();
      yield();
    }
  }
 
  client.setServer(brokerIP.c_str(), 1883);
  client.setCallback(callback);
  delay(1000);
 
  // 5 seconds to push the AP button and receive messages
  publishValue(bootCount, bootCountTopic);

  DBG.println("3 s to push the AP button");
  currentMillis = millis();
  previousMillis = currentMillis;
  publishValue((float)1.0, listenTopic);
  while (currentMillis - previousMillis < (unsigned long) 3000)
  {
    blinkOneTime(100, true);
    if ( ! client.connected() ) reconnect();
    client.loop();
    button.loop();
    currentMillis = millis();
    yield();
  }
  publishValue((float)0.0, listenTopic);

  /*
  ===============================================================================
  Sensor setup & reading
  ===============================================================================
  */

  // Show setup finished - Turn off led
#ifdef ESP32
  digitalWrite(LED_BUILTIN, LOW);
#else
  digitalWrite(LED_BUILTIN, HIGH);
#endif
  delay(10000); // 10 seconds to read the data on the display

  /*
  ===============================================================================
  Deep sleep
  ===============================================================================
  */
  // Deep sleep
  long sleepTimeSec  = (long) readFile(LittleFS, getPath(PARAM_SLEEP_TIME_SEC).c_str()).toInt();
  
  if ( ! (sleepTimeSec < 0 ) )
  {
    String time = String(timeToString((unsigned long) sleepTimeSec));
    DBG.print(F("Deep sleep: "));
    DBG.print(time);
    DBG.println(F(" (hh:mm:ss)"));
    DBG.flush();
    delay(2000);
    
    // Going to sleep now
    ESP.deepSleep(sleepTimeSec * uS_TO_S_FACTOR); 
    DBG.println("This will never be printed");    
  }
  else
  {
    DBG.println("Deep sleep disabled."); 
  }
}

void loop() 
{
  // Deactivate deep sleep to run this
#ifdef DEBUGGING
  aunit::TestRunner::run();
#endif
  
  button.loop();
  if ( ! client.connected() ) reconnect();
  client.loop();

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
            DBG.println(F("single click"));
            break;
        case double_click:
            DBG.println(F("double click"));
            printWiFiState();
            break;
        case triple_click:
            DBG.println(F("triple click"));
            break;
        case long_click:
            DBG.println(F("long click"));
            // Set AP-Mode:
            writeFile(LittleFS, getPath(PARAM_WIFI_MODE).c_str(), "WIFI_AP_MODE");
            // wipeLittleFSFiles();
            DBG.println( "Reboot" );
            delay(1000);
            DBG.println("3");
            delay(500);
            DBG.println("3 2");
            delay(500);
            DBG.println("3 2 1");
            delay(500);
            DBG.println("in AP-Mode");
            delay(1000);
            ESP.restart();
            break;
        case empty:
            DBG.print("empty ");
            break;
    }
   }
}

void printWiFiState()
{
  WiFiMode_t state = WiFi.getMode();
  String ssid = "no ssid";
  String devCon = "con: ";
  IPAddress ip(0, 0, 0, 0);
  uint8_t devNum = 0;
  String ipStr;

  switch (state)
  {
    case WIFI_AP:
      DBG.println(F("WIFI_AP"));
      ssid = WiFi.softAPSSID();
      devNum = WiFi.softAPgetStationNum();
      devCon.concat(devNum);
      break;
    case WIFI_STA:
      DBG.println(F("WIFI_STA"));
      ssid = WiFi.SSID();
      break;
    case WIFI_AP_STA:
      DBG.println(F("WIFI_AP_STA"));
      break;
    case WIFI_OFF:
      DBG.println(F("WIFI_OFF"));
      break;
      
    default:
      break;
  }
}

void setupButton()
{
  pinMode(kButtonPin, INPUT_PULLUP);
  button.begin(kButtonPin);
  button.setDebounceTime(40);
  button.setDoubleClickTime(500);
  button.setLongClickTime(1000);
  button.setClickHandler(btnHandler);
  button.setDoubleClickHandler(btnHandler);
  button.setLongClickHandler(btnHandler);
  DBG.println(F("Setup button done"));
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

String getSubstring(const String& data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for (int i=0; i<=maxIndex && found<=index; i++) 
  {
    if (data.charAt(i)==separator || i==maxIndex) 
    {
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String cutLastSubString(const String& topic, char separator) 
{
  int lastSlashIndex = topic.lastIndexOf(separator);
  if (lastSlashIndex >= 0) 
  {
    return topic.substring(0, lastSlashIndex);
  }
  return topic;
}

void reconnect() 
{
  // Loop until we're reconnected
  while ( ! client.connected() ) 
  {
    DBG.print("Attempting MQTT connection...");
    // Attempt to connect
    String rootTopic = getSubstring(sleepTopic, '/', 0);
    if (client.connect(deviceName.c_str())) 
    {
            DBG.println("connected");
      // Once connected, publish an announcement...
      client.publish(rootTopic.c_str(), ("connected " + deviceName).c_str() , true );
      // ... and resubscribe
      if ( ! sleepTopic.isEmpty() )
      {
        client.subscribe((char*) sleepTopic.c_str() );
        DBG.print(F("subscribed to : "));
        DBG.println(sleepTopic);
      }
      // String subTopic2 = readFile(LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_2).c_str() );
      // if ( ! subTopic2.isEmpty() )
      // {
      //   client.subscribe((char*) subTopic2.c_str() );
      //   DBG.print(F("subscribed to : "));
      //   DBG.println(subTopic2);
      // }
      // String subTopic3 = readFile(LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_3).c_str() );
      // if ( ! subTopic3.isEmpty() )
      // {
      //   client.subscribe((char*) subTopic3.c_str() );
      //   DBG.print(F("subscribed to : "));
      //   DBG.println(subTopic3);
      // }
    } 
    else 
    {
      DBG.print("failed, rc=");
      DBG.print(client.state());
      DBG.print(" wifi=");
      DBG.print(WiFi.status());
      DBG.println(" try again in 5 s");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishValue(float payload, const String& pubTopic) 
{
  // confirm still connected to mqtt server
  if ( ! client.connected() ) reconnect();
  client.loop();
  // Send sensor data
  String payloadStr = String(payload);

  DBG.print("Publish topic: ");
  DBG.println(pubTopic);
  DBG.print("Publish message: ");
  DBG.println(payload);
  client.publish( (char*) pubTopic.c_str() , (char*) payloadStr.c_str(), true );
}

void callback(char* topic, byte* message, unsigned int length) 
{
  DBG.print("Message arrived on topic: ");
  DBG.print(topic);
  DBG.print(".\nPayload: ");
  String payload = "";
  payload.reserve(length);
 
  for (unsigned int i = 0; i < length; i++) 
  {
    DBG.print((char)message[i]);
    payload += (char)message[i];
  }
  DBG.println();

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == sleepTopic) 
  {
    DBG.print(F("New sleep time received: "));
    // Check content:
    int newSleepTimeSec = payload.toInt(); // Turns to 0 if not Int
    DBG.println(newSleepTimeSec);
    if (newSleepTimeSec != 0)
    {
      writeFile(LittleFS, getPath(PARAM_SLEEP_TIME_SEC).c_str(), String(newSleepTimeSec).c_str());
      ESP.restart();
    }
    else
    {
      DBG.print(F("Setting to endless sleep over MQTT is disabled."));
    }
  }
}

// t is time in seconds = millis()/1000;
char* timeToString(unsigned long t)
{
  static char str[14];
  unsigned int h = t / 3600;
  t = t % 3600;
  unsigned int m = t / 60;
  unsigned int s = t % 60;
  sprintf(str, "%02d:%02d:%02d", h, m, s);
  return str;
}