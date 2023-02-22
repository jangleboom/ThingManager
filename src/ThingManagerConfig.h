#ifndef THING_CONFIG_H
#ifndef THING_MANAGER_CONFIG_H
#define THING_MANAGER_CONFIG_H
#include <Arduino.h>

/* 
THIS WILL BE REPLACED BY THE ThingManagerConfig.h after including this project 
as submodule into the Smart Home Device project under lib/
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
#define STATION_SERVER_ENABLED        true
#define OTA_USER                      "maker"   // Change this, or set up a THING_CONFIG_H with own values
#define OTA_PW                        "m4K3fr"  // Change this, or set up a THING_CONFIG_H with own values

#endif  /*** THING_MANAGER_CONFIG_H ***/
#endif