#include <unity.h>
#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <set/Set.h>

#include <path/PathUtils.h>


void setUp(void)
{
    Serial.begin(115200);
}

void tearDown(void)
{
    // clean stuff up here
}

void testFindFileExtension_hasExt(void) {
    
    // Given 
    const char* rootPath = "test.txt";
    // When
    const char* ext = getFileExtension(rootPath);
    // Then
    TEST_ASSERT_EQUAL_STRING("txt", ext);
}

void testFindFileExtension_noExtNoDot(void) {
    
    // Given 
    const char* rootPath = "test";
    // When
    const char* ext = getFileExtension(rootPath);
    // Then
    TEST_ASSERT_EQUAL_STRING("", ext);
}


void testFindFileExtension_noExtWhenDot(void) {
    
    // Given 
    const char* rootPath = "test.";
    // When
    const char* ext = getFileExtension(rootPath);
    // Then
    TEST_ASSERT_EQUAL_STRING("", ext);
}

void testGetPreviousLevel_emptyString(void) {

    // Given
    const char* path = "";
    // When
    const char* result = getParentDir(path);
    // Then
    TEST_ASSERT_EQUAL_STRING("/", result);
}

void testGetPreviousLevel_slashSlash(void) {

    // Given
    const char* path = "/";
    // When
    const char* result = getParentDir(path);
    // Then
    TEST_ASSERT_EQUAL_STRING("/", result);
}

void testGetPreviousLevel_oneLevenRoot(void) {

    // Given
    const char* path = "/one";
    // When
    const char* result = getParentDir(path);
    // Then
    TEST_ASSERT_EQUAL_STRING("/", result);
}

void testGetPreviousLevel_twoLevelsOne(void) {

    // Given
    const char* path = "/one/two";
    // When
    const char* result = getParentDir(path);
    // Then
    TEST_ASSERT_EQUAL_STRING("/one", result);
}

void testGetPreviousLevel_twoLevelsSlashOne(void) {

    // Given
    const char* path = "/one/two/";
    // When
    const char* result = getParentDir(path);
    // Then
    TEST_ASSERT_EQUAL_STRING("/one", result);
}

void testGetPreviousLevel_filePassedParentDirProvided(void) {

    // Given
    const char* path = "/one/two/max.txt";
    // When
    const char* result = getParentDir(path);
    // Then
    TEST_ASSERT_EQUAL_STRING("/one/two", result);
}

// Actual test runner
void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();
    RUN_TEST(testFindFileExtension_hasExt);
    RUN_TEST(testFindFileExtension_noExtNoDot);
    RUN_TEST(testFindFileExtension_noExtWhenDot);
    RUN_TEST(testGetPreviousLevel_emptyString);
    RUN_TEST(testGetPreviousLevel_slashSlash);
    RUN_TEST(testGetPreviousLevel_oneLevenRoot);
    RUN_TEST(testGetPreviousLevel_twoLevelsOne);
    RUN_TEST(testGetPreviousLevel_twoLevelsSlashOne);
    RUN_TEST(testGetPreviousLevel_filePassedParentDirProvided);
    UNITY_END(); // stop unit testing
}

void loop()
{
}