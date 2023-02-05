#ifndef THING_CONFIG_H
#ifndef THING_MANAGER_CONFIG_H
#define THING_MANAGER_CONFIG_H
#include <Arduino.h>

/* 
THIS WILL BE REPLACED BY THE ThingManagerConfig.h after including this project 
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

#endif  /*** THING_MANAGER_CONFIG_H ***/
#endif