#ifndef TESTS_SMART_HOME_DEVICE_MANAGER_H
#define TESTS_SMART_HOME_DEVICE_MANAGER_H

#include <AUnit.h>
#include <SmartHomeDeviceManager.h>

using namespace aunit;
using namespace SmartHomeDeviceManager;

test(processorWriteAndReadSpiffs) 
{
    String testValue = "TestValue";
    const char PATH_TEST_VALUE[] PROGMEM = "/pathTestValue.txt";
    if (SPIFFS.exists(PATH_TEST_VALUE)) 
    {
        if (!SPIFFS.remove(PATH_TEST_VALUE))
        {
            assert("Failed to remove SPIFFS path.");
        }
    }
    writeFile(SPIFFS, PATH_TEST_VALUE, testValue.c_str());
    delay(100);
    String savedValue = readFile(SPIFFS, PATH_TEST_VALUE);
    assertTrue(savedValue.equals(testValue));
}

#endif /*** TESTS_SMART_HOME_DEVICE_MANAGER_H ***/