#include <Arduino.h>
#include <unity.h>

void test_example()
{
    TEST_ASSERT_EQUAL(2 + 2, 4);
}

void setup() 
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_example);
    UNITY_END();
}

void loop() 
{
}