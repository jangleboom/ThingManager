#ifndef TESTS_SMART_HOME_DEVICE_MANAGER_H
#define TESTS_SMART_HOME_DEVICE_MANAGER_H

#include <AUnit.h>
#include <SmartHomeDeviceManager.h>

using namespace aunit;
using namespace SmartHomeDeviceManager;

test(writeAndReadFileOnLittleFS) 
{
    String testValue = "TestValue";
    const char* PATH_TEST_VALUE = "/pathTestValue.txt";
    if (LittleFS.exists(PATH_TEST_VALUE)) 
    {
        if (!LittleFS.remove(PATH_TEST_VALUE))
        {
            assert("Failed to remove LittleFS path.");
        }
    }
    writeFile(LittleFS, PATH_TEST_VALUE, testValue.c_str());
    delay(100);
    String savedValue = readFile(LittleFS, PATH_TEST_VALUE);
    assertTrue(savedValue.equals(testValue));
}

test(deleteFileOnLittleFS)
{
    String testValue = "TestValueToDelete";
    // const char* PATH_TEST_VALUE = "/pathTestValueToDelete.txt";
    const char PATH_TEST_VALUE[] = "/pathTestValueToDelete.txt";
    if (LittleFS.exists(PATH_TEST_VALUE)) 
    {
        if (!LittleFS.remove(PATH_TEST_VALUE))
        {
            assert("Failed to remove LittleFS path.");
        }
    }
    writeFile(LittleFS, PATH_TEST_VALUE, testValue.c_str());
    delay(100);
    String savedValue = readFile(LittleFS, PATH_TEST_VALUE);
    assertTrue(savedValue.equals(testValue));
    LittleFS.remove(PATH_TEST_VALUE);

    assertFalse(LittleFS.exists(PATH_TEST_VALUE));
}

#endif /*** TESTS_SMART_HOME_DEVICE_MANAGER_H ***/