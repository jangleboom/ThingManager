/**
 * @file    ThingManager.h
 * @author  jangleboom
 * @link    https://github.com/audio-communication-group/ThingManager.git
 * <br>
 * @brief   This is part of a distributed software, here: the web interface to config 
 *          the realtime kinematics rover
 * <br>
 * @todo    - a simular version for the head tracker
 *          - a check for special characters in the form
 *          - a check of the number of decimal places in the input of the geo-coordinates 
 *            with regard to a suitable level of accuracy
 *          - upload html and (separated css and js) to LittleFS 
 * 
 * @note    FYI: A good tutorial about how to transfer input data from a from and save them to LittleFS
 *          https://medium.com/@adihendro/html-form-data-input-c942ba23224
 */

#ifndef SMART_HOME_DEVICE_MANAGER_H
#define SMART_HOME_DEVICE_MANAGER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ThingManagerConfig.h>
#include <LittleFS.h> 
#include <FS.h>

#include <index_html.h>
#include <error_html.h>
#include <reboot_html.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPmDNS.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <Hash.h>
  #include <ESP8266mDNS.h>
#endif

namespace ThingManager 
{
  // LittleFS
  #define FILE_WRITE                              "w"
  #define FILE_READ                               "r"
  #define ROOT_DIR                                "/"

  // DEVICE_TYPE can be defined e. g. in a separate SmartHomeDeviceConfig.h file, if not use this here
  #ifndef DEVICE_TYPE
  static const char DEVICE_TYPE[]       = "thing";
  #endif

  // WiFi credentials for AP mode
  #define MAX_SSIDS 10 // Space to scan and remember SSIDs
  const char AP_PASSWORD[]              = "12345678";
  const char IP_AP[]                    = "192.168.4.1";

  // Parameters for LittleFS file management
  const char PARAM_THING_NAME[]         = "thing_name"; 
  const char PARAM_WIFI_SSID[]          = "ssid"; 
  const char PARAM_WIFI_PASSWORD[]      = "password";
  const char PARAM_MQTT_BROKER_IP[]     = "broker_ip";
  const char PARAM_MQTT_PUB_TOPIC_1[]   = "pub_topic_1";
  const char PARAM_MQTT_PUB_TOPIC_2[]   = "pub_topic_2";
  const char PARAM_MQTT_PUB_TOPIC_3[]   = "pub_topic_3";
  const char PARAM_MQTT_SUB_TOPIC_1[]   = "sub_topic_1";
  const char PARAM_MQTT_SUB_TOPIC_2[]   = "sub_topic_2";
  const char PARAM_MQTT_SUB_TOPIC_3[]   = "sub_topic_3";
  const char PARAM_SLEEP_TIME_SEC[]     = "sleep_time_sec";

  /**
   * @brief Get the Path From File Name object
   * 
   * @param fileName 
   * @return String 
   */
  String getPath(const char* fileName);

  //===============================================================================
  // Wifi
  /**
   * @brief Setup RTK rover station in station mode to enter further settings
   * 
   * @param ssid        SSID of the local network
   * @param password    Password of the local network
   * @param deviceName  MDNS name, connect via http://<deviceName>.local
   * @return bool       True if succeed, false if not
   */
  bool setupStationMode(const char* ssid, const char* password, const char* deviceName);

  /**
   * @brief Setup RTK rover station in access point mode to enter local network
   *        credentials and other settings
   * 
   * @param apSsid      SSID of the access point
   * @param apPassword  Password of the access point
   */
  void setupAPMode(const char* apSsid, const char* apPassword);

  /**
   * @brief Setup WiFi: Access point on first run (if no credentials saved in LittleFS),
   *        if this is the case you must enter your wifi credentials into the web form 
   *        (192.168.4.1 is default IP) and reboot. You can change the WiFi credentials
   *        while connected via the web form or press the wipe button to delete the memory.
   *        The device boots in AP mode again.
   *        If the saved credentials are correct (and the AP is available), the 
   *        device will switch connect in station mode after reboot.
   * 
   * @param server Pointer to AsyncWebServer, where the web form is hosted.
   * 
   */
  void setupWiFi(AsyncWebServer* server);

  /**
   * @brief Recennect to the last AP, if reachable
   * 
   * @return true If WiFi connection established
   * @return false If WiFi connection fails
   */
  bool checkConnectionToWifiStation(void);

  /**
   * @brief Check possibility of connecting with an availbale network.
   * 
   * @param ssid        SSID of saved network in LittleFS
   * @return true       If the credentials are complete and the network is available.
   * @return false      If the credentials are incomplete or the network is not available.
   */
  bool savedNetworkAvailable(const String& ssid);

  //===============================================================================
  // Web server
  /**
   * @brief         Start web server 
   * 
   * @param server  Pointer to global web server object
   */
  void startServer(AsyncWebServer *server);

  /**
   * @brief Relaces placeholders in HTML code
   * 
   * @param var Placeholder
   * @return String Text to replace the placeholder
   */
  String processor(const String& var);

  /**
   * @brief Request not found handler
   * 
   * @param request Request
   */
  void notFound(AsyncWebServerRequest *request);

  /**
   * @brief Action to handle wipe LittleFS button
   * 
   * @param request Request
   */
  void actionWipeData(AsyncWebServerRequest *request);

  /**
   * @brief Action to handle Reboot button
   * 
   * @param request Request
   */
  void actionRebootESP(AsyncWebServerRequest *request);

  /**
   * @brief Action to handle Save button
   * 
   * @param request Request
   */
  void actionUpdateData(AsyncWebServerRequest *request);

  //===============================================================================
  // LittleFS
  /**
   * @brief Just init LittleFS for ESP32 or ESP8266
   * 
   * @return true   If LittleFS is successfully initialized
   *         false  If LittleFS init failed
   */
  bool setupLittleFS();

  /**
   * @brief 
   * 
   * @return true   Formatting succeeded
   * @return false  Formatting failed
   */
  bool formatLittleFS(void);

  /**
   * @brief         Write data to LittleFS
   * 
   * @param fs      Address of file system
   * @param path    Path to file
   * @param message Content to save in file on path
   */
  void writeFile(fs::FS &fs, const char* path, const char* message);

  /**
   * @brief           Read data from LittleFS
   * 
   * @param fs        Address of file system
   * @param path      Path to file
   * @return String   Content saved in file on path
   */
  String readFile(fs::FS &fs, const char* path);

  /**
   * @brief List all saved LittleFS files 
   * 
   */
  void listFiles(void);

  /**
   * @brief Delete all saved LittleFS files 
   * 
   */
  void wipeLittleFSFiles(void);

/**
 * @brief Get the unique Device Name 
 * 
 * @param prefix Device name e. g. rtkrover
 * @return String Name + ID
 */
String getDeviceName(const String &prefix);


/**
 * @brief Get the Chip Id (part of the MAC address)
 * 
 * @return uint32_t Chip Id
 */
uint32_t getChipId(void);

}
#endif /*** SMART_HOME_DEVICE_MANAGER_H ***/