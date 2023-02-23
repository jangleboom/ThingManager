#ifndef THING_CONFIG_H
#ifndef THING_MANAGER_CONFIG_H
#define THING_MANAGER_CONFIG_H
#include <Arduino.h>

/* 
THIS WILL BE REPLACED BY THE your own ThingConfig.h after including this project 
as submodule into the lib/ dir of your thing project. This is just an example.
*/

/*
===============================================================================
Default serial settings
===============================================================================
*/

//set to true for debug output, false for no debug output
#define DEBUGGING true 
#define DBG \
  if (DEBUGGING) Serial

#define BAUD                          115200
// If false, the OTA- and config-server only runs in WIFI_AP mode
#define STATION_SERVER_ENABLED        true   

#define HTTP_USER                     "admin"
#define HTTP_PW                       "4dm1n"
// Separate OTA credentials possible
#define OTA_USER                      ""
#define OTA_PW                        ""

#ifndef DEVICE_TYPE
static const char DEVICE_TYPE[]       = "thing";
#endif

// WiFi credentials for AP mode
#define MAX_SSIDS 10 // Space to scan and remember SSIDs
const char AP_PASSWORD[]              = "12345678";
const char IP_AP[]                    = "192.168.4.1";

#endif  /*** THING_MANAGER_CONFIG_H ***/
#endif
