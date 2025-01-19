#include <unity.h>
#include <Arduino.h>

#include <TimeUtils.h>

void setUp(void)
{
    Serial.begin(115200);
}

void tearDown(void)
{
    // clean stuff up here
}

// Test cases
void test_formatTime_full_pattern() {
    char output[32];
    formatTime(5, 3, 9, "HH:MM:SS", output);
    TEST_ASSERT_EQUAL_STRING("05:03:09", output);
}

void test_formatTime_single_letter_pattern() {
    char output[32];
    formatTime(5, 3, 9, "H:M:S", output);
    TEST_ASSERT_EQUAL_STRING("5:3:9", output);
}

void test_formatTime_mixed_pattern() {
    char output[64];
    formatTime(12, 0, 59, "H hours, MM minutes and SS seconds", output);
    TEST_ASSERT_EQUAL_STRING("12 hours, 00 minutes and 59 seconds", output);
}

void test_formatTime_edge_case_midnight() {
    char output[32];
    formatTime(0, 0, 0, "HH:MM:SS", output);
    TEST_ASSERT_EQUAL_STRING("00:00:00", output);
}

void test_formatTime_edge_case_noon() {
    char output[32];
    formatTime(12, 30, 45, "H:M:S", output);
    TEST_ASSERT_EQUAL_STRING("12:30:45", output);
}

void test_formatTime_no_padding() {
    char output[32];
    formatTime(5, 8, 15, "H:M:S", output);
    TEST_ASSERT_EQUAL_STRING("5:8:15", output);
}

void test_formatTime_only_minutes_and_seconds() {
    char output[32];
    formatTime(0, 3, 9, "MM:SS", output);
    TEST_ASSERT_EQUAL_STRING("03:09", output);
}

// Actual test runner
void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(test_formatTime_full_pattern);
    RUN_TEST(test_formatTime_single_letter_pattern);
    RUN_TEST(test_formatTime_mixed_pattern);
    RUN_TEST(test_formatTime_edge_case_midnight);
    RUN_TEST(test_formatTime_edge_case_noon);
    RUN_TEST(test_formatTime_no_padding);
    RUN_TEST(test_formatTime_only_minutes_and_seconds);

    UNITY_END(); // stop unit testing
}

void loop()
{
}