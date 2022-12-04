#include <SmartHomeDeviceManager.h>

/*
=================================================================================
                                WiFi
=================================================================================
*/

bool SmartHomeDeviceManager::setupStationMode(const char* ssid, const char* password, const char* deviceName) 
{
  bool success = false;

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    DBG.println("WiFi Failed! Try to decrease the distance to the AP or check your PW!");
    success = false;
  }
  else 
  {
    DBG.print(F("WiFi connected to SSID: "));
    DBG.println(WiFi.SSID());
    DBG.print(F("Wifi client started: "));
    DBG.println(WiFi.getHostname());
    DBG.print(F("IP Address: "));
    DBG.println(WiFi.localIP());

    success = true;
  }

  return success;
}

bool SmartHomeDeviceManager::formatLittleFS()
{
  bool formatted = LittleFS.format();
 
  if (formatted) 
  {
    DBG.println("\n\nSuccessfully formatted");
  }
  else
  {
    DBG.println("\n\nError during formatting");
  }

  return formatted;
}

bool SmartHomeDeviceManager::checkConnectionToWifiStation() 
{ 
  bool isConnectedToStation = false;
  
  int wifiStationMode;
#ifdef ESP32
  wifiStationMode = WIFI_MODE_STA;
#else
  wifiStationMode = WIFI_STA;
#endif

  if (WiFi.getMode() == wifiStationMode)
  {
    if (WiFi.status() != WL_CONNECTED) 
    {
      DBG.println(F("Reconnecting to access point."));
      isConnectedToStation = WiFi.reconnect();
    } 
    else 
    {
      DBG.println(F("WiFi connected."));
      isConnectedToStation = true;
    }
  }

  return isConnectedToStation;
}

void SmartHomeDeviceManager::setupAPMode(const char* apSsid, const char* apPassword) 
{
  DBG.print("Setting soft-AP ... ");
  // WiFi.disconnect();
  WiFiMode_t wifiAPMode;
#ifdef ESP32
  wifiAPMode = WIFI_MODE_AP;
  WiFi.mode(WIFI_MODE_AP);
#else
  wifiAPMode = WIFI_AP;
#endif

bool result = WiFi.softAP(apSsid, apPassword);

if (WiFi.getMode() == wifiAPMode)
  {
    DBG.print(F("Set to WIFI_AP mode."));
    
    if (result == true)
    {
      DBG.print(F("Access point started: "));
      DBG.println(apSsid);
      DBG.print(F("IP address: "));
      IPAddress apIP = WiFi.softAPIP();
      DBG.println(apIP);
    }
    else
    {
      DBG.print(F("Access point could not started."));
    }
  } 
  else
  {
    DBG.print(F("Set to WIFI_AP mode failed! "));
  }
// DBG.print("wait a minute");
// while(true){};
}

void SmartHomeDeviceManager::setupWiFi(AsyncWebServer* server)
{
  WiFi.softAPdisconnect(true); // AP  sollte noch verbunden sein
  WiFi.disconnect(true);       // STA sollte noch verbunden sein

  // Check if we have credentials for a available network
  String lastSSID = readFile(LittleFS, PATH_WIFI_SSID);
  String lastPassword = readFile(LittleFS, PATH_WIFI_PASSWORD);

  if (lastSSID.isEmpty() || lastPassword.isEmpty() ) 
  {
    setupAPMode(getDeviceName(DEVICE_TYPE).c_str(), AP_PASSWORD);
    delay(500);
    startServer(server);
    delay(500);
  } 
  else
  {
    while ( !savedNetworkAvailable(lastSSID) ) 
    {
      DBG.print(F("Waiting for HotSpot "));
      DBG.print(lastSSID);
      DBG.println(F(" to appear..."));
      // vTaskDelay(1000/portTICK_RATE_MS);
      delay(1000);
    }
    setupStationMode(lastSSID.c_str(), lastPassword.c_str(), getDeviceName(DEVICE_TYPE).c_str());
    delay(500);
  }
}

bool SmartHomeDeviceManager::savedNetworkAvailable(const String& ssid) 
{
  if (ssid.isEmpty()) return false;

  uint8_t nNetworks = (uint8_t) WiFi.scanNetworks();
  DBG.print(nNetworks);  
  DBG.println(F(" networks found."));
  for (uint8_t i=0; i<nNetworks; i++) 
  {
    if (ssid.equals(String(WiFi.SSID(i)))) 
    {
      DBG.print(F("A known network with SSID found: ")); 
      DBG.print(WiFi.SSID(i));
      DBG.print(F(" (")); 
      DBG.print(WiFi.RSSI(i)); 
      DBG.println(F(" dB), connecting..."));
      return true;
    }
  }
  return false;
}
/*
=================================================================================
                                Web server
=================================================================================
*/
void SmartHomeDeviceManager::startServer(AsyncWebServer *server) 
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send_P(200, "text/html", INDEX_HTML, processor);
  });

  // server->on("/actionUpdateData", HTTP_POST, actionUpdateData);
  // server->on("/actionWipeData", HTTP_POST, actionWipeData);
  // server->on("/actionRebootESP32", HTTP_POST, actionRebootESP32);

  // server->onNotFound(notFound);
  server->begin();
}
  
void SmartHomeDeviceManager::notFound(AsyncWebServerRequest *request) 
{
  request->send(404, "text/plain", "Not found");
}

void SmartHomeDeviceManager::actionRebootESP32(AsyncWebServerRequest *request) 
{
  DBG.println("ACTION actionRebootESP32!");
  request->send_P(200, "text/html", REBOOT_HTML, SmartHomeDeviceManager::processor);
  delay(3000);
  ESP.restart();
}

void SmartHomeDeviceManager::actionWipeData(AsyncWebServerRequest *request) 
{
  DBG.println("ACTION actionWipeData!");
  int params = request->params();
  DBG.printf("params: %d\n", params);
  for (int i = 0; i < params; i++) 
  {
    AsyncWebParameter* p = request->getParam(i);
    DBG.printf("%d. POST[%s]: %s\n", i+1, p->name().c_str(), p->value().c_str());
    if (strcmp(p->name().c_str(), "wipe_button") == 0) 
    {
      if (p->value().length() > 0) 
      {
        DBG.printf("wipe command received: %s",p->value().c_str());
        wipeLittleFsFiles();
      } 
     }
    } 

  DBG.print(F("Data in LittleFS was wiped out!"));
  request->send_P(200, "text/html", INDEX_HTML, processor);
}

void SmartHomeDeviceManager::actionUpdateData(AsyncWebServerRequest *request) 
{
  DBG.println("ACTION actionUpdateData!");

  int params = request->params();
  for (int i = 0; i < params; i++) 
  {
    AsyncWebParameter* p = request->getParam(i);
    DBG.printf("%d. POST[%s]: %s\n", i+1, p->name().c_str(), p->value().c_str());

    if (strcmp(p->name().c_str(), PARAM_WIFI_SSID) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_WIFI_SSID, p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_WIFI_PASSWORD) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_WIFI_PASSWORD, p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_BROKER_IP) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_MQTT_BROKER_IP, p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_PUB_TOPIC_1) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_MQTT_PUB_TOPIC_1, p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_PUB_TOPIC_2) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_MQTT_PUB_TOPIC_2, p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_PUB_TOPIC_3) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_MQTT_PUB_TOPIC_3, p->value().c_str());
      } 
    }
    if (strcmp(p->name().c_str(), PARAM_MQTT_SUB_TOPIC_1) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_MQTT_SUB_TOPIC_1, p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_SUB_TOPIC_2) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_MQTT_SUB_TOPIC_2, p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_SUB_TOPIC_3) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_MQTT_SUB_TOPIC_3, p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_SLEEP_TIME_SEC) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, PATH_SLEEP_TIME_SEC, p->value().c_str());
      } 
    }
  }

  DBG.println(F("Data saved to LittleFS!"));
  request->send_P(200, "text/html", INDEX_HTML, SmartHomeDeviceManager::processor);
}

// Replaces placeholder with stored values
String SmartHomeDeviceManager::processor(const String& var) 
{
  if (var == PARAM_WIFI_SSID) 
  {
    String savedSSID = readFile(LittleFS, PATH_WIFI_SSID);
    return (savedSSID.isEmpty() ? String(PARAM_WIFI_SSID) : savedSSID);
  }
  else if (var == PARAM_WIFI_PASSWORD) 
  {
    String savedPassword = readFile(LittleFS, PATH_WIFI_PASSWORD);
    return (savedPassword.isEmpty() ? String(PARAM_WIFI_PASSWORD) : "*******");
  }

  else if (var == PARAM_MQTT_BROKER_IP) 
  {
    String savedBroker = readFile(LittleFS, PATH_MQTT_BROKER_IP);
    return (savedBroker.isEmpty() ? String(PARAM_MQTT_BROKER_IP) : savedBroker);
  }

  else if (var == PARAM_MQTT_PUB_TOPIC_1) 
  {
    String savedPubTopic = readFile(LittleFS, PATH_MQTT_PUB_TOPIC_1);
    return (savedPubTopic.isEmpty() ? String(PARAM_MQTT_PUB_TOPIC_1) : savedPubTopic);
  }

  else if (var == PARAM_MQTT_PUB_TOPIC_2) 
  {
    String savedPubTopic = readFile(LittleFS, PATH_MQTT_PUB_TOPIC_2);
    return (savedPubTopic.isEmpty() ? String(PARAM_MQTT_PUB_TOPIC_2) : savedPubTopic);
  }

  else if (var == PARAM_MQTT_PUB_TOPIC_3) 
  {
    String savedPubTopic = readFile(LittleFS, PATH_MQTT_PUB_TOPIC_3);
    return (savedPubTopic.isEmpty() ? String(PARAM_MQTT_PUB_TOPIC_3) : savedPubTopic);
  }
  else if (var == PARAM_MQTT_SUB_TOPIC_1) 
  {
    String savedSubTopic = readFile(LittleFS, PATH_MQTT_SUB_TOPIC_1);
    return (savedSubTopic.isEmpty() ? String(PARAM_MQTT_SUB_TOPIC_1) : savedSubTopic);
  }

  else if (var == PARAM_MQTT_SUB_TOPIC_2) 
  {
    String savedSubTopic = readFile(LittleFS, PATH_MQTT_SUB_TOPIC_2);
    return (savedSubTopic.isEmpty() ? String(PARAM_MQTT_SUB_TOPIC_2) : savedSubTopic);
  }

  else if (var == PARAM_MQTT_SUB_TOPIC_3) 
  {
    String savedSubTopic = readFile(LittleFS, PATH_MQTT_SUB_TOPIC_3);
    return (savedSubTopic.isEmpty() ? String(PARAM_MQTT_SUB_TOPIC_3) : savedSubTopic);
  }

  else if (var == PARAM_SLEEP_TIME_SEC) 
  {
    String savedSleepTime = readFile(LittleFS, PATH_SLEEP_TIME_SEC);
    return (savedSleepTime.isEmpty() ? String(PARAM_SLEEP_TIME_SEC) : savedSleepTime);
  }
 
  else if (var == "next_addr") 
  {
    String savedSSID = readFile(LittleFS, PATH_WIFI_SSID);
    String savedPW = readFile(LittleFS, PATH_WIFI_PASSWORD);
    if (savedSSID.isEmpty() || savedPW.isEmpty()) 
    {
      return String(IP_AP);
    } else 
    {
      String clientAddr = getDeviceName(DEVICE_TYPE);
      clientAddr += ".local";
      return clientAddr;
    }
  }
  else if (var == "next_ssid") 
  {
    String savedSSID = readFile(LittleFS, PATH_WIFI_SSID);
    return (savedSSID.isEmpty() ? getDeviceName(DEVICE_TYPE) : savedSSID);
  }
  return String();
}

/*
=================================================================================
                                LittleFS
=================================================================================
*/
bool SmartHomeDeviceManager::setupLittleFS() 
{
  bool isMounted = false; 

  if ( !LittleFS.begin() ) 
  {
    Serial.println(F("An Error has occurred while mounting LittleFS"));
    return isMounted;
  } 
  else
  {
    DBG.println("LittleFS mounted");
    isMounted = true;
  }

  return isMounted;
}

String SmartHomeDeviceManager::readFile(fs::FS &fs, const char* path) 
{
  DBG.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, FILE_READ);

  if (!file || file.isDirectory()) 
  {
    DBG.println("- empty file or failed to open file");
    return String();
  }

  DBG.println("- read from file:");
  String fileContent;

  while (file.available()) 
  {
    fileContent += String((char)file.read());
  }
  file.close();
  DBG.println(fileContent);

  return fileContent;
}

void SmartHomeDeviceManager::writeFile(fs::FS &fs, const char* path, const char* message) 
{
  DBG.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) 
  {
    DBG.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) 
  {
    DBG.println("- file written");
  } else 
  {
    DBG.println("- write failed");
  }
  file.close();
}

void SmartHomeDeviceManager::listFiles() 
{
  File root = LittleFS.open(ROOT_DIR, FILE_READ);
  File file = root.openNextFile();
 
  while (file)
  {
      DBG.print(F("File name: "));
      DBG.print(file.name());
      DBG.print(F(", content: "));
      String fileContent;

      while (file.available()) 
      {
        fileContent += String((char)file.read());
      }
      file.close();
      DBG.println(fileContent);
 
      file = root.openNextFile();
  }
  file.close();
  root.close();
}

void SmartHomeDeviceManager::wipeLittleFsFiles() 
{
  File root = LittleFS.open(ROOT_DIR, FILE_WRITE);
  File file = root.openNextFile();

  DBG.println(F("Wiping: "));

  while (file) 
  {
    DBG.print("FILE: ");
#if defined(ESP8266)
    const char* PATH = file.fullName();
#endif
#if defined(ESP32)
    const char* PATH = file.path();
#endif
    
    DBG.println(PATH);
    file.close();
    LittleFS.remove(PATH);
    file = root.openNextFile();
  }
}

String SmartHomeDeviceManager::getDeviceName(const String& prefix) 
  {
    String deviceName((char *)0);
    String suffix = String(getChipId(), HEX);

    unsigned int prefixLen = prefix.length();
    unsigned int suffixLen = suffix.length();
    
    deviceName.reserve(prefixLen + suffixLen);
    deviceName += prefix;
    deviceName += "-";
    deviceName += suffix;


    return deviceName;
  }

  uint32_t SmartHomeDeviceManager::getChipId() 
  {
    uint32_t chipID = 0;
#ifdef ESP8266
    chipID = ESP.getChipId();
#endif

#ifdef ESP32
    for (int i=0; i<17; i=i+8) 
    {
      chipID |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
      DBG.print("chipID: ");
      DBG.println(chipID, HEX);
#endif

      return chipID;
  }
