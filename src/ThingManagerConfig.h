#ifndef SMART_HOME_DEVICE_CONFIG_H
#ifndef SMART_HOME_DEVICE_MANAGER_CONFIG_H
#define SMART_HOME_DEVICE_MANAGER_CONFIG_H
#include <Arduino.h>

/* 
THIS WILL BE REPLACED BY THE RTKRoverConfig.h after including this project 
as submodule into the Smart Home Device project under lib/
*/

/******************************************************************************/
//                       Default Serial settings
/******************************************************************************/

//set to true for debug output, false for no debug output
#define DEBUGGING true 
#define DBG \
  if (DEBUGGING) Serial

#define BAUD                          115200
#define STATION_SERVER_ENABLED        true

#endif  /*** SMART_HOME_DEVICE_MANAGER_CONFIG_H ***/
#endif