#include <Arduino.h>
#include <ThingManagerConfig.h>
#include <ThingManager.h>
#include <PubSubClient.h>

#ifdef DEBUGGING
#include <TestsThingManager.h>
#endif

#ifndef ARDUINO_M5STACK_Core2
void blinkOneTime(int blinkTime, bool isBlocking);
#else
#include <M5Core2.h>
void updateButtonState(void);
void vibrateOneTime(int vibrateTime, bool isBlocking);
float getBatteryVoltage(void);
float getBatteryPercentage(void);
#endif

static AsyncWebServer server(80);
String scannedSSIDs[MAX_SSIDS];
WiFiClient espClient;
PubSubClient client(espClient);
String rootTopic, shouldSleepTopic, isSleepingTopic, batteryTopic, isListeningTopic, brokerIP, deviceName;
const String kDelimiter = "/";
long sleepTimeSec;

// DEEP SLEEP
// Conversion factor for micro seconds to seconds
#define uS_TO_S_FACTOR 1000000 
// Time ESP will go to sleep (in seconds)
// #define TIME_TO_SLEEP_S 60 * 15 // Sleep for 15 min
#define TIME_TO_SLEEP_S 20
// Save data in RTC fast memory (8kB SRAM)
unsigned int bootCount = 0;

// MQTT
void publishValue(float payload, String& pubTopic);
String getSubstring(const String& data, char separator, int index);
void reconnect(void);
void callback(char* topic, byte* payload, unsigned int length);
char* timeToString(unsigned long t);
//===============================================================================
namespace tman = ThingManager;

void setup() 
{ 
  #ifdef DEBUGGING
  Serial.begin(BAUD);
  while (!Serial) {};
  delay(1000);
  DBG.println(F("Serial setup done."));
  #endif

#ifndef ARDUINO_M5STACK_Core2
  pinMode(LED_BUILTIN, OUTPUT);
  // Show setup began - Turn on led
  #ifdef ESP32
  digitalWrite(LED_BUILTIN, HIGH);
  #else
  digitalWrite(LED_BUILTIN, LOW);
  #endif
#else
  M5.begin();  //Init M5Core.  
  M5.Lcd.setTextColor(
      YELLOW);  //Set the font color to yellow.  
  M5.Lcd.setTextSize(2);  //Set the font size.  
  M5.Lcd.setCursor(
      65, 10);  //Move the cursor position to (x, y). 
  M5.Lcd.println(
      "Button example");  //The screen prints the formatted string and wraps the line.  输出格式化字符串并换行
  M5.Lcd.setCursor(3, 35);
  M5.Lcd.println("Press button B for 700ms");
  M5.Lcd.println("to clear screen.");
  M5.Lcd.setTextColor(RED);
#endif 
  //===============================================================================
  // Initialize LittleFS
  // Use board_build.partitions in platformio.ini
  if ( ! tman::setupLittleFS()) 
  {
    tman::formatLittleFS();
    if ( ! tman::setupLittleFS()) while (true) {}; // Here you should never hang
  }

  // Uncomment if you want to format (e. g after changing partition sizes)
  // (And dont forget to comment this again after one run ;)
  //tman::formatLittleFS();

  // tman::wipeLittleFSFiles();  // Use this for deleting all data without formatting
  
#ifdef DEBUGGING
  tman::listFiles();
  delay(1000); // Time to watch the serial output
#endif
  //===============================================================================
  tman::setupWiFi(&server);
  if (WiFi.getMode() == WIFI_AP)
  {
    int devNum = WiFi.softAPgetStationNum();
    int prevDevNum = devNum;
    
    while (true)
    {
      if (devNum != prevDevNum) 
      {
        prevDevNum = devNum;
        DBG.printf("Connected client(s): %i\n", devNum);
      }
      devNum = WiFi.softAPgetStationNum();
#ifndef ARDUINO_M5STACK_Core2
      blinkOneTime(500, true);
#endif
      yield();
    }
  }

  if (WiFi.getMode() == WIFI_STA)
  {
    // Waiting here for WiFi connection
    DBG.println(F("Searching for hotspot..."));
    while (! tman::checkConnectionToWifiStation() ) 
    {
#ifndef ARDUINO_M5STACK_Core2
      blinkOneTime(1000, true);
#endif
      yield();
    }

#ifdef ESP8266
    MDNS.update();
#endif 
  }

  //===============================================================================
  
  /*
  ===============================================================================
  MQTT
  ===============================================================================
  */
  brokerIP  =  tman::readFile(LittleFS, tman::getPath(tman::PARAM_MQTT_BROKER_IP).c_str());
  String pubTopic1 = tman::readFile( LittleFS, tman::getPath(tman::PARAM_MQTT_PUB_TOPIC_1).c_str() );
  shouldSleepTopic = tman::readFile( LittleFS, tman::getPath(tman::PARAM_MQTT_SUB_TOPIC_1).c_str() );
  deviceName = tman::readFile(LittleFS, tman::getPath(tman::PARAM_THING_NAME).c_str());

  if (pubTopic1.isEmpty() || brokerIP.isEmpty() )
  {
    DBG.print(F("Check your MQTT setup, enter "));
    DBG.print(WiFi.softAPIP());
    DBG.print(F(" of "));
    DBG.println(WiFi.softAPSSID());

    while (true) 
    {
#ifndef ARDUINO_M5STACK_Core2
      blinkOneTime(100, true);
#endif
      yield();
    }
  }

  rootTopic = getSubstring(pubTopic1, '/', 0);
  String baseTopic = rootTopic + kDelimiter + deviceName + kDelimiter;
  isSleepingTopic = baseTopic + "sleeping";
  batteryTopic = baseTopic + "battery";
  isListeningTopic = baseTopic + "listening";
  
  client.setServer(brokerIP.c_str(), 1883);
  DBG.printf("brokerIP: %s\n", brokerIP.c_str());
  client.setCallback(callback);
 
  delay(1000);
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  previousMillis = currentMillis;

  // Read sleep time fresh from memory in case it has changed
  String sleepTimeStr = tman::readFile( LittleFS, tman::getPath(tman::PARAM_SLEEP_TIME_SEC).c_str() );
  sleepTimeSec = -1; // No deep sleep by default
  if ( ! sleepTimeStr.isEmpty() )
    sleepTimeSec = (long) sleepTimeStr.toInt();
  
  DBG.print("sleepTimeSec: ");
  DBG.println(sleepTimeSec);

  if (sleepTimeSec != -1)
  {
    publishValue( 1.f, isListeningTopic);
    while (currentMillis - previousMillis < (unsigned long) 10000)
    {
     if ( ! client.connected() ) reconnect();
     client.loop();
#ifndef ARDUINO_M5STACK_Core2
    blinkOneTime(100, true);
#else 
    vibrateOneTime(100, true);
#endif
    currentMillis = millis();
    yield();
    }
    publishValue((float)0.0, isListeningTopic);
  }
  

 
 
  //===============================================================================
#ifndef ARDUINO_M5STACK_Core2
  // Show setup finished - Turn off led
#ifdef ESP32
  digitalWrite(LED_BUILTIN, LOW);
#else
  digitalWrite(LED_BUILTIN, HIGH);
#endif
#endif

  /*
  ===============================================================================
  Deep sleep
  ===============================================================================
  */
  // Deep sleep
  // Increment boot number and print it every reboot
  ++bootCount;
  DBG.println("\nBoot number: " + String(bootCount));

  if ( ! (sleepTimeSec < 0) )
  {
    String time = String(timeToString((unsigned long) sleepTimeSec));
    DBG.print(F("Going to sleep for "));
    DBG.println(time.c_str());
    DBG.flush();
    
    delay(2000);

    // Going to sleep now
#ifdef ARDUINO_M5STACK_Core2
    M5.Axp.DeepSleep(SLEEP_SEC(sleepTimeSec));
#elif defined ESP32 || defined ESP8266
    ESP.deepSleep(sleepTimeSec * uS_TO_S_FACTOR);
#endif
    DBG.println("This will never be printed");   
  }
  else
  {
    DBG.println("Deep sleep disabled."); 
  }
}

//===============================================================================

const unsigned long RECONNECT_INTERVAL = 10000;
unsigned long previousMillis = RECONNECT_INTERVAL * 2;

void loop() 
{
  // Deactivate deep sleep to run this
#ifdef DEBUGGING
  // aunit::TestRunner::run();
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
    connected = tman::checkConnectionToWifiStation();
    DBG.printf("connected: %s\n", connected ? "yes" : "no");
    if (connected) 
    {
      float sleepTime = tman::readFile( LittleFS, tman::getPath(tman::PARAM_SLEEP_TIME_SEC).c_str() ).toFloat();
      
#ifdef ARDUINO_M5STACK_Core2
      static uint8_t counter = 0;
      counter % 2 ? publishValue(sleepTime, isSleepingTopic) : \
      publishValue(getBatteryPercentage(), batteryTopic);
      ++counter %= 100;
#else
      publishValue(sleepTime, isSleepingTopic);
#endif
      
    }
    // Turn on/of led
#ifndef ARDUINO_M5STACK_Core2
#ifdef ESP32
    digitalWrite(LED_BUILTIN, connected ? LOW : HIGH);
#else
    digitalWrite(LED_BUILTIN, connected ? HIGH : LOW);
#endif
#endif
  }

  if ( ! client.connected() ) reconnect();
  client.loop();
#ifdef ARDUINO_M5STACK_core2
  M5.update();  
  updateButtonState();
#endif
}

//===============================================================================
#ifndef ARDUINO_M5STACK_Core2
void blinkOneTime(int blinkTime, bool isBlocking = false)
{
  static int ledState = LOW;  // ledState used to set the LED
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

#ifdef ESP32
  ledState = (ledState == HIGH) ? LOW : HIGH;
  digitalWrite(LED_BUILTIN, ledState);
  isBlocking ? delay(blinkTime) : vTaskDelay(blinkTime);
#elif ESP8266
  if ( ! isBlocking)
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

void reconnect() 
{
  // Loop until we're reconnected
  while ( ! client.connected() ) 
  {
    DBG.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(deviceName.c_str())) 
    {
      DBG.println("connected");
      // Once connected, publish an announcement...
      client.publish(rootTopic.c_str(), ("connected " + deviceName).c_str() , true );
      // ... and resubscribe
      String subscription = tman::readFile(LittleFS, tman::getPath(tman::PARAM_MQTT_SUB_TOPIC_1).c_str() );
      if ( ! subscription.isEmpty() )
      {
        // client.subscribe((char*) subscription.c_str() );
         client.subscribe( subscription.c_str() );
        DBG.print("subscribed to : ");
        DBG.println(subscription);
      }
    } 
    else 
    {
      DBG.print("failed, rc=");
      DBG.print(client.state());
      DBG.print(" wifi=");
      DBG.print(WiFi.status());
      DBG.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishValue(float payload, String& pubTopic) 
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
  // client.publish( (char*) pubTopic.c_str() , (char*) payloadStr.c_str(), true );
   client.publish( pubTopic.c_str() , payloadStr.c_str(), true );
}

void callback(char* topic, byte* message, unsigned int length) 
{
  DBG.print("Message arrived on topic: ");
  DBG.print(topic);
  DBG.print(". Message: ");
  String shouldSleepTopicPayload = "";
  
  for (unsigned int i = 0; i < length; i++) 
  {
    DBG.print((char)message[i]);
    shouldSleepTopicPayload += (char)message[i];
  }
  DBG.println();

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == shouldSleepTopic) 
  {
    DBG.print(F("Sleep time change received: "));
    // Check content:
    long newSleepTimeSec = shouldSleepTopicPayload.toInt(); // Turns to 0 if not Int
    DBG.println(newSleepTimeSec);
    if (newSleepTimeSec != 0)
    {
      tman::writeFile(LittleFS, tman::getPath(tman::PARAM_SLEEP_TIME_SEC).c_str(), String(newSleepTimeSec).c_str());
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
  static char str[12];
  long h = t / 3600;
  t = t % 3600;
  int m = t / 60;
  int s = t % 60;
  sprintf(str, "%04ld:%02d:%02d", h, m, s);
  return str;
}

/*
  ===============================================================================
  M5Core2
  ===============================================================================
  */
#ifdef ARDUINO_M5STACK_Core2
void updateButtonState()
{
  if ( M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed() ) 
    vibrateOneTime(50, false);

  //Read the press state of the key. 
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200)) 
  {
    vibrateOneTime(50, false);
    M5.Lcd.print('A');
  } else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) 
  {
    vibrateOneTime(50, false);
    M5.Lcd.print('B');
  } else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200)) 
  {
    vibrateOneTime(50, false);
    M5.Lcd.print('C');
  } else if (M5.BtnB.wasReleasefor(700)) {
    M5.Lcd.clear(
        WHITE);  // Clear the screen and set white to the background color.  清空屏幕并将白色设置为底色
    M5.Lcd.setCursor(0, 0);
  }
}

void vibrateOneTime(int vibrateTime, bool isBlocking)
{
  static int vibState = false;  // vibState to set the vibration motor

  vibState = vibState ? false : true;
  M5.Axp.SetLDOEnable(3, vibState);
  isBlocking ? delay(vibrateTime * 2) : vTaskDelay(vibrateTime * 2);
  vibState = vibState ? false : true;
  M5.Axp.SetLDOEnable(3, vibState);
}

float getBatteryVoltage()
{
  return  M5.Axp.GetBatVoltage();
}

float getBatteryPercentage()
{
  float batVoltage = getBatteryVoltage();
  float batPercentage = ( batVoltage < 3.2 ) ? 0 : ( batVoltage - 3.2 ) * 100;

  return batPercentage;
}

#endif