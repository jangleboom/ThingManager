
#include <Arduino.h>
#include <unity.h>
// #include "../src/ThingManager.h"
#include <ThingManager.h>

namespace tman = ThingManager;

// void test_writeAndReadFileOnLittleFS(void) 
// {
//     String testValue = "TestValue";
//     const char* PATH_TEST_VALUE = "/pathTestValue.txt";
//     if (LittleFS.exists(PATH_TEST_VALUE)) 
//     {
//         if (!LittleFS.remove(PATH_TEST_VALUE))
//         {
//             assert("Failed to remove LittleFS path.");
//         }
//     }
//     writeFile(LittleFS, PATH_TEST_VALUE, testValue.c_str());
//     delay(100);
//     String savedValue = readFile(LittleFS, PATH_TEST_VALUE);
//     TEST_ASSERT_TRUE(savedValue.equals(testValue));
// }

// void test_deleteFileOnLittleFS(void)
// {
//     String testValue = "TestValueToDelete";
//     const char PATH_TEST_VALUE[] = "/pathTestValueToDelete.txt";
//     if (LittleFS.exists(PATH_TEST_VALUE)) 
//     {
//         if (!LittleFS.remove(PATH_TEST_VALUE))
//         {
//             assert("Failed to remove LittleFS path.");
//         }
//     }
//     writeFile(LittleFS, PATH_TEST_VALUE, testValue.c_str());
//     delay(100);
//     String savedValue = readFile(LittleFS, PATH_TEST_VALUE);
//     TEST_ASSERT_TRUE(savedValue.equals(testValue));
//     LittleFS.remove(PATH_TEST_VALUE);

//     TEST_ASSERT_FALSE(LittleFS.exists(PATH_TEST_VALUE));
// }

// void test_getPath(void)
// {
//     const char testValue[]  = "testValue";
//     String testValuePath    = "/testValue.txt";
//     String result           = getPath(testValue);

//     TEST_ASSERT_TRUE(result.equals(testValuePath));
// }

void test_true(void) {
    TEST_ASSERT_TRUE(true);
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);


  if ( ! tman::setupLittleFS()) 
  {
    tman::formatLittleFS();
    if ( ! tman::setupLittleFS()) while (true) {}; // Here you should never hang
  }

  // Uncomment if you want to format (e. g after changing partition sizes)
  // (And dont forget to comment this again after one run ;)
  //formatLittleFS();

  // wipeLittleFSFiles();  // Use this for deleting all data without formatting
  

  UNITY_BEGIN(); // IMPORTANT LINE!
  RUN_TEST(test_true);
//   RUN_TEST(test_getPath);
//   RUN_TEST(test_writeAndReadFileOnLittleFS);
//   RUN_TEST(test_deleteFileOnLittleFS);
  UNITY_END(); 
}

void loop()
{
  // if (i < max_blinks)
  // {
  //   RUN_TEST(test_led_state_high);
  //   delay(500);
  //   RUN_TEST(test_led_state_low);
  //   delay(500);
  //   i++;
  // }
  // else if (i == max_blinks)
  // {
  //   UNITY_END(); // stop unit testing
  // }
}
