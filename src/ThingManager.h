/**
 * @file    ThingManager.h
 * @author  jangleboom
 * @link    https://github.com/audio-communication-group/ThingManager.git
 * <br>
 * @brief   This is network base for a smart home device that uses MQTT. It offers
 *          a webform to enter and save credentials on a LittleFS. Further
 *          you can make Over-The-Air updates via AsyncElegantOTA.
 * <br>
 * @todo    - webform update to change this PW
 *          - a check for special characters in the form
 *          - upload html and (separated css and js) to LittleFS 
 * 
 */

#ifndef THING_MANAGER_H
#define THING_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h> 
#include <FS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "ThingManagerConfig.h"
#include "index_html.h"
#include "error_html.h"
#include "reboot_html.h"


namespace ThingManager 
{
  // LittleFS
  #define FILE_WRITE                              "w"
  #define FILE_READ                               "r"
  #define ROOT_DIR                                "/"

  // Parameters for LittleFS file management
  const char PARAM_THING_NAME[]         = "thing_name"; 
  const char PARAM_WIFI_SSID[]          = "wifi_ssid"; 
  const char PARAM_WIFI_PW[]            = "wifi_pw";
  const char PARAM_SERVER_USER[]        = "server_user";
  const char PARAM_SERVER_PW[]          = "server_pw";
  const char PARAM_OTA_USER[]           = "ota_user";
  const char PARAM_OTA_PW[]             = "ota_pw";
  const char PARAM_MQTT_BROKER_IP[]     = "broker_ip";
  const char PARAM_MQTT_PUB_TOPIC_1[]   = "pub_topic_1";
  const char PARAM_MQTT_PUB_TOPIC_2[]   = "pub_topic_2";
  const char PARAM_MQTT_PUB_TOPIC_3[]   = "pub_topic_3";
  const char PARAM_MQTT_SUB_TOPIC_1[]   = "sub_topic_1";
  const char PARAM_MQTT_SUB_TOPIC_2[]   = "sub_topic_2";
  const char PARAM_MQTT_SUB_TOPIC_3[]   = "sub_topic_3";
  const char PARAM_SLEEP_TIME_SEC[]     = "sleep_time_sec";
  const char PARAM_WIFI_MODE[]          = "wifi_mode";

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
   * @return bool       success
   */
  bool setupAPMode(const char* apSsid, const char* apPassword);

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
   * @return bool       success
   */
  bool setupWiFi(AsyncWebServer* server);

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
  void setServerCallbacks(AsyncWebServer *server);

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
   * @param request AsyncWebServerRequest
   */
  void notFound(AsyncWebServerRequest *request);

  /**
   * @brief Request logout handler
   * 
   * @param request AsyncWebServerRequest
   */
  void logout(AsyncWebServerRequest *request);

  /**
   * @brief Action to handle wipe LittleFS button
   * 
   * @param request AsyncWebServerRequest
   */
  void actionWipeData(AsyncWebServerRequest *request);

  /**
   * @brief Action to handle Reboot button
   * 
   * @param request AsyncWebServerRequest
   */
  void actionRebootESP(AsyncWebServerRequest *request);

  /**
   * @brief Action to handle Save button
   * 
   * @param request AsyncWebServerRequest
   */
  void actionUpdateData(AsyncWebServerRequest *request);

  /**
   * @brief Action to handle Over-The-Air-Update
   * 
   * @param request AsyncWebServerRequest
   */
  void actionOTA(AsyncWebServerRequest *request);

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
 * @param prefix Device name e. g. thing
 * @return String Name + ID
 */
String getDeviceName(const String &prefix);


/**
 * @brief Get the Chip Id (part of the MAC address)
 * 
 * @return uint32_t Chip Id
 */
uint32_t getChipId(void);

/**
 * @brief Get the WiFi Mode Str object
 * 
 * @param opmode WiFi mode 
 * @return String WiFi mode as String
 */
String getWiFiModeStr(uint8_t opmode);

// Define the callback function type with a return type of void and
// an int argument
typedef void (*Callback)(int);

// A function that takes a callback as an argument
void run(Callback callback); 

// void printCallback(int value);
// A sample callback function, declared as extern
void printValue(int value);

}
#endif /*** THING_MANAGER_H ***/

