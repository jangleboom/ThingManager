#include <ThingManager.h>


/**
   * @brief Get the path to a file stored in LittleFS
   * 
   * @param fileName  Name of stored file
   * @return String   Path to LittleFS file
   */
String ThingManager::getPath(const char* fileName)
{
  String path = "/";
  path += fileName;
  path += ".txt";

  return path;
}

/*
=================================================================================
                                WiFi
=================================================================================
*/

bool ThingManager::setupStationMode(const char* ssid, const char* password, const char* deviceName) 
{
  bool success = false;

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();

  if ( ! WiFi.isConnected() )
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
    DBG.print(F("Local IP address: "));
    DBG.println(WiFi.localIP());

    success = true;
  }

  return success;
}

bool ThingManager::formatLittleFS()
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

bool ThingManager::checkConnectionToWifiStation() 
{ 
  bool isConnectedToStation = WiFi.isConnected();
  // Check if we have credentials for a available network
  String ssid = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
  String password = readFile(LittleFS, getPath(PARAM_WIFI_PW).c_str());
  String deviceName = getDeviceName(DEVICE_TYPE);

  if (WiFi.getMode() == WIFI_STA)
  {
    if ( ! isConnectedToStation)
    {
      if ( ! savedNetworkAvailable(ssid) ) 
      {
        DBG.print(F("Don't see HotSpot: "));
        DBG.println(ssid);
        return false;
      }
      else 
      {
      if ( ! ssid.isEmpty() && ! password.isEmpty() ) 
        {
          DBG.println("Try reconnect to access point.");
          isConnectedToStation = setupStationMode(ssid.c_str(), password.c_str(), deviceName.c_str());
          DBG.printf("isConnectedToStation: %s\n", isConnectedToStation ? "yes" : "no");
        } 
      } 
    }
    else 
    {
      DBG.print(F("WiFi connected to SSID: "));
      DBG.println(WiFi.SSID());
      DBG.print(F("WiFi client name: "));
      DBG.println(WiFi.getHostname());
      DBG.print(F("IP Address: "));
      DBG.println(WiFi.localIP());
  #ifdef ESP8266
      MDNS.update();
  #endif
    }
  }

  return isConnectedToStation;
}

bool ThingManager::setupAPMode(const char* apSsid, const char* apPassword) 
{
  DBG.print("Setting soft-AP ... ");
  WiFi.disconnect();
  WiFiMode_t wifiAPMode;
#ifdef ESP32
  wifiAPMode = WIFI_AP;
  WiFi.mode(WIFI_AP);
#else
  wifiAPMode = WIFI_AP;
#endif

bool result = WiFi.softAP(apSsid, apPassword);

if (WiFi.getMode() == wifiAPMode)
  {
    DBG.print(F("Set to WIFI_AP mode."));
    DBG.println(result ? "Ready" : "Failed!");
    if (result)
    {
      DBG.print(F("Access point started: "));
      DBG.println(apSsid);
      DBG.print(F("AP IP address: "));
      DBG.println(WiFi.softAPIP());
      DBG.print(F("AP Password: "));
      DBG.println(apPassword);
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

  return result;
}

bool ThingManager::setupWiFi(AsyncWebServer* server)
{
  bool success = false;
  // Test callback:
  run(printValue);
  // Check if we have credentials for a available network
  String ssid = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
  String password = readFile(LittleFS, getPath(PARAM_WIFI_PW).c_str());
  String deviceName = getDeviceName(DEVICE_TYPE);
  String wifiMode = readFile(LittleFS, getPath(PARAM_WIFI_MODE).c_str());
  bool startAP = strcmp(wifiMode.c_str(), "WIFI_AP_MODE") == 0;

  WiFi.softAPdisconnect(true); // AP  sollte noch verbunden sein
  WiFi.disconnect(true);       // STA sollte noch verbunden sein
  WiFi.setHostname(deviceName.c_str());

  

  if (ssid.isEmpty() || password.isEmpty() || startAP) 
  {
    success = setupAPMode(deviceName.c_str(), AP_DEFAULT_PW);
    delay(500);
    setServerCallbacks(server); 
    String otaUser = readFile( LittleFS, getPath(PARAM_OTA_USER).c_str() );
    String otaPw = readFile( LittleFS, getPath(PARAM_OTA_PW).c_str() );
    DBG.printf("otaUser: %s\n", otaUser.c_str());
    DBG.printf("otaPw: %s\n", otaPw.c_str());
    if ( ! otaUser.isEmpty() && ! otaPw.isEmpty() )
    {
      AsyncElegantOTA.begin(server, otaUser.c_str(), otaPw.c_str());  // Start ElegantOTA with password protection
    }
    else
    {
      AsyncElegantOTA.begin(server);  // Start ElegantOTA without password protection
    }
    server->begin();  // Start WebInterface + OTA (http://LOCAL_IP/update)
    delay(500);
  } 
  else
  {
    if ( ! savedNetworkAvailable(ssid) ) 
    {
      DBG.print(F("Don't see HotSpot: "));
      DBG.println(ssid);
      success = false;
      // #ifdef ESP32
      // vTaskDelay(1000/portTICK_RATE_MS);
      // #else
      // delay(1000);
      // #endif
    }
    else 
    {
      success = setupStationMode(ssid.c_str(), password.c_str(), deviceName.c_str());
      if (STATION_SERVER_ENABLED)
      {
        if (!MDNS.begin(deviceName.c_str())) 
        {
          DBG.println("Error starting mDNS, use local IP instead!");
        } 
        else 
        {
          DBG.print(F("Starting mDNS, find me under <http://"));
          DBG.print(WiFi.getHostname());
          DBG.println(F(".local>"));
        }
        delay(500);
        setServerCallbacks(server); 
        String OTAUser = readFile( LittleFS, getPath(PARAM_OTA_USER).c_str() );
        String OTAPW = readFile( LittleFS, getPath(PARAM_OTA_PW).c_str() );
        if ( ! OTAUser.isEmpty() && ! OTAPW.isEmpty() )
        {
          AsyncElegantOTA.begin(server, OTAUser.c_str(), OTAPW.c_str());  // Start ElegantOTA with password protection
        }
        else
        {
          AsyncElegantOTA.begin(server);  // Start ElegantOTA without password protection
        }
        server->begin();  // Start WebInterface + OTA (http://LOCAL_IP/update)
        delay(500);
        delay(500);

      }
    }
  }

  return success;
}

bool ThingManager::savedNetworkAvailable(const String& ssid) 
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
void ThingManager::setServerCallbacks(AsyncWebServer *server) 
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    String serverUser = readFile( LittleFS, getPath(PARAM_SERVER_USER).c_str() );
    String serverPW = readFile( LittleFS, getPath(PARAM_SERVER_PW).c_str() );
    if ( ! serverUser.isEmpty() && ! serverPW.isEmpty() )
    {
      if ( ! request->authenticate(serverUser.c_str(), serverPW.c_str() ) ) 
      return request->requestAuthentication();
    }
   
    request->send_P(200, "text/html", INDEX_HTML, processor);
  });

  server->on("/actionUpdateData", HTTP_POST, actionUpdateData);
  server->on("/actionWipeData", HTTP_POST, actionWipeData);
  server->on("/actionRebootESP", HTTP_POST, actionRebootESP);
  server->on("/actionOTA", HTTP_POST, actionOTA);
  server->onNotFound(notFound);
  // server->begin();
}
  
void ThingManager::notFound(AsyncWebServerRequest *request) 
{
  request->send(404, "text/plain", "Not found");
}

void ThingManager::actionRebootESP(AsyncWebServerRequest *request) 
{
  DBG.println("ACTION actionRebootESP!");
  request->send_P(200, "text/html", REBOOT_HTML, ThingManager::processor);
  writeFile(LittleFS, getPath(PARAM_WIFI_MODE).c_str(), "WIFI_STA_MODE");
  // delay(3000);
  ESP.restart();
}

void ThingManager::actionOTA(AsyncWebServerRequest *request) 
{
  DBG.println("ACTION actionOTA!");
  request->redirect("/update");
}

void ThingManager::actionWipeData(AsyncWebServerRequest *request) 
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
        wipeLittleFSFiles();
      } 
     }
    } 

  DBG.print(F("Data in LittleFS was wiped out!"));
  request->send_P(200, "text/html", INDEX_HTML, processor);
}

void ThingManager::actionUpdateData(AsyncWebServerRequest *request) 
{
  DBG.println("ACTION actionUpdateData!");

  int params = request->params();
  for (int i = 0; i < params; i++) 
  {
    AsyncWebParameter* p = request->getParam(i);
    DBG.printf("%d. POST[%s]: %s\n", i+1, p->name().c_str(), p->value().c_str());

    if (strcmp(p->name().c_str(), PARAM_THING_NAME) == 0) 
    {
      if (p->value().length() > 0) 
      {
        String newName = p->value();
        newName.toLowerCase();
        writeFile(LittleFS, getPath(PARAM_THING_NAME).c_str(), newName.c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_WIFI_SSID) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_WIFI_PW) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_WIFI_PW).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_SERVER_USER) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_SERVER_USER).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_SERVER_PW) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_SERVER_PW).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_OTA_USER) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_OTA_USER).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_OTA_PW) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_OTA_PW).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_BROKER_IP) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_MQTT_BROKER_IP).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_PUB_TOPIC_1) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_MQTT_PUB_TOPIC_1).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_PUB_TOPIC_2) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_MQTT_PUB_TOPIC_2).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_PUB_TOPIC_3) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_MQTT_PUB_TOPIC_3).c_str(), p->value().c_str());
      } 
    }
    if (strcmp(p->name().c_str(), PARAM_MQTT_SUB_TOPIC_1) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_1).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_SUB_TOPIC_2) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_2).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_MQTT_SUB_TOPIC_3) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_3).c_str(), p->value().c_str());
      } 
    }

    if (strcmp(p->name().c_str(), PARAM_SLEEP_TIME_SEC) == 0) 
    {
      if (p->value().length() > 0) 
      {
        writeFile(LittleFS, getPath(PARAM_SLEEP_TIME_SEC).c_str(), p->value().c_str());
        DBG.print("write PARAM_SLEEP_TIME_SEC: ");
        DBG.println("p->value().c_str()");
      } 
    }
  }

  DBG.println(F("Data saved to LittleFS!"));
  request->send_P(200, "text/html", INDEX_HTML, processor);
}

// Replaces placeholder with stored values
String ThingManager::processor(const String& var) 
{
  String result = "";
  if (var == PARAM_WIFI_SSID) 
  {
    String savedSSID = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
    return (savedSSID.isEmpty() ? "" : savedSSID);
  }

  else if (var == PARAM_WIFI_PW) 
  {
    String savedPassword = readFile(LittleFS, getPath(PARAM_WIFI_PW).c_str());
    return (savedPassword.isEmpty() ? "" : "*******");
  }

  else if (var == PARAM_SERVER_USER) 
  {
    String savedUser = readFile(LittleFS, getPath(PARAM_SERVER_USER).c_str());
    return (savedUser.isEmpty() ? "" : savedUser);
  }

  else if (var == PARAM_SERVER_PW) 
  {
    String savedPassword = readFile(LittleFS, getPath(PARAM_SERVER_PW).c_str());
    return (savedPassword.isEmpty() ? "" : "*******");
  }

  else if (var == PARAM_OTA_USER) 
  {
    String otaUser = readFile(LittleFS, getPath(PARAM_OTA_USER).c_str());
    return (otaUser.isEmpty() ? "" : otaUser);
  }

  else if (var == PARAM_OTA_PW) 
  {
    String otaPassword = readFile(LittleFS, getPath(PARAM_OTA_PW).c_str());
    return (otaPassword.isEmpty() ? "" : "*******");
  }

  else if (var == PARAM_THING_NAME) 
  {
    String savedThingName = readFile(LittleFS, getPath(PARAM_THING_NAME).c_str());
    return (savedThingName.isEmpty() ? "" : savedThingName);
  }

  else if (var == PARAM_MQTT_BROKER_IP) 
  {
    String savedBroker = readFile(LittleFS, getPath(PARAM_MQTT_BROKER_IP).c_str());
    return (savedBroker.isEmpty() ? "" : savedBroker);
  }

  else if (var == PARAM_MQTT_PUB_TOPIC_1) 
  {
    String savedPubTopic = readFile(LittleFS, getPath(PARAM_MQTT_PUB_TOPIC_1).c_str());
    return (savedPubTopic.isEmpty() ? "" : savedPubTopic);
  }

  else if (var == PARAM_MQTT_PUB_TOPIC_2) 
  {
    String savedPubTopic = readFile(LittleFS, getPath(PARAM_MQTT_PUB_TOPIC_2).c_str());
    return (savedPubTopic.isEmpty() ? "" : savedPubTopic);
  }

  else if (var == PARAM_MQTT_PUB_TOPIC_3) 
  {
    String savedPubTopic = readFile(LittleFS, getPath(PARAM_MQTT_PUB_TOPIC_3).c_str());
    return (savedPubTopic.isEmpty() ? "" : savedPubTopic);
  }
  else if (var == PARAM_MQTT_SUB_TOPIC_1) 
  {
    String savedSubTopic = readFile(LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_1).c_str());
    return (savedSubTopic.isEmpty() ? "" : savedSubTopic);
  }

  else if (var == PARAM_MQTT_SUB_TOPIC_2) 
  {
    String savedSubTopic = readFile(LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_2).c_str());
    return (savedSubTopic.isEmpty() ? "" : savedSubTopic);
  }

  else if (var == PARAM_MQTT_SUB_TOPIC_3) 
  {
    String savedSubTopic = readFile(LittleFS, getPath(PARAM_MQTT_SUB_TOPIC_3).c_str());
    return (savedSubTopic.isEmpty() ? "" : savedSubTopic);
  }

  else if (var == PARAM_SLEEP_TIME_SEC) 
  {
    String savedSleepTime = readFile(LittleFS, getPath(PARAM_SLEEP_TIME_SEC).c_str());
    return (savedSleepTime.isEmpty() ? "" : savedSleepTime);
  }
 
  else if (var == "next_addr") 
  {
    String savedSSID = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
    String savedPW = readFile(LittleFS, getPath(PARAM_WIFI_PW).c_str());
    if (savedSSID.isEmpty() || savedPW.isEmpty()) 
    {
      return String(AP_DEFAULT_IP);
    } else 
    {
      String clientAddr = getDeviceName(DEVICE_TYPE);
      clientAddr += ".local";
      return clientAddr;
    }
  }
  else if (var == "next_ssid") 
  {
    String savedSSID = readFile(LittleFS, getPath(PARAM_WIFI_SSID).c_str());
    return (savedSSID.isEmpty() ? getDeviceName(DEVICE_TYPE) : savedSSID);
  }
  return String();
}

/*
=================================================================================
                                LittleFS
=================================================================================
*/
bool ThingManager::setupLittleFS() 
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

String ThingManager::readFile(fs::FS &fs, const char* path) 
{
  DBG.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, FILE_READ);

  if ( !file || file.isDirectory() ) 
  {
    DBG.println("- empty file or failed to open file");
    return String();
  }

  DBG.println("- read from file:");
  String fileContent;

  while ( file.available() ) 
  {
    fileContent += String( (char) file.read() );
  }
  file.close();
  DBG.println(fileContent);

  return fileContent;
}

void ThingManager::writeFile(fs::FS &fs, const char* path, const char* message) 
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

void ThingManager::listFiles() 
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

void ThingManager::wipeLittleFSFiles() 
{
  File root = LittleFS.open("/", FILE_WRITE);
  File file = root.openNextFile();

  DBG.println(F("Wiping: "));

  while (file) 
  {
#ifdef ESP32
      const char* pathStr = strdup(file.path());
#else
      const char* pathStr = strdup(file.fullName());
#endif
      file.close();
      LittleFS.remove(pathStr);
      file = root.openNextFile();
  }

  DBG.println(F("Data in LittleFS wiped out!"));
}

String ThingManager::getDeviceName(const String& prefix) 
  {
    String thingName = readFile(LittleFS, getPath(PARAM_THING_NAME).c_str());

    if (thingName.isEmpty())
    {
      String suffix = String(getChipId(), HEX);

      thingName += prefix;
      thingName += "-";
      thingName += suffix;
    }

   return thingName; 
  }

  uint32_t ThingManager::getChipId() 
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

/**
 * @brief 
 * enum WiFiMode { WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3 };
 * 
 */

String ThingManager::getWiFiModeStr(const uint8_t opmode)
{
  String mode = "";

  switch (opmode)
  {
    case 0:
      mode = "WIFI_OFF";
      break;
    case 1:
      mode = "WIFI_STA";
      break;
    case 2:
      mode = "WIFI_AP";
      break;
    case 3:
      mode = "WIFI_AP_STA";
      break;
    default:
      break;
  }
  
  return mode;
}

// A function that takes a callback as an argument
void ThingManager::run(Callback callback) {
  // Invoke the callback function with an argument of 10
  callback(10);
}

// A sample callback function
void ThingManager::printValue(int value) {
  DBG.print(F("The value is: "));
  DBG.println(value);
}
