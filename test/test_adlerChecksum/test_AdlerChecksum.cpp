#include <unity.h>
#include <esp_log.h>
#include <LogTags.h>

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <set/Set.h>

#include <PinDefinitions.h>

#include <index/FileIndex.h>
#include <FileManager.h>
#include <checksum/AdlerChecksum.h>

FileManager fileManager(SD, PIN_CS_SD);
AdlerChecksum testSubject(fileManager);

const char *PATH_TEST_DIR   = "/.test";
const char *PATH_SHORT_TEXT = "/.test/text_short.txt";
const char *PATH_LONG_TEXT  = "/.test/text_long.txt";
const char *PATH_NOT_EXIST  = "/.test/i_never_exist.txt";

String text = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor.";

void setUp(void)
{
    Serial.begin(115200);

    // Initalize display as it is used by text index
    SPI.begin();
    fileManager.begin();
    fileManager.createDir(PATH_TEST_DIR);

    // Prepare files
    fileManager.writeFile(PATH_SHORT_TEXT, text.c_str());

    String longText;
    for (uint8_t i = 0; i < UINT8_MAX; i++)
    {
        longText.concat(text);
    }

    fileManager.writeFile(PATH_LONG_TEXT, longText.c_str());

    ESP_LOGI(TAG_TEST, "---------------- SETUP DONE ----------------");
}

void tearDown(void)
{

    ESP_LOGI(TAG_TEST, "---------------- TEST DONE ----------------");

    fileManager.removeDirRecursive(PATH_TEST_DIR);
    // clean stuff up here
}

void testSmallFileChecksum_OK(void)
{

    File smallFile = fileManager.openFile(PATH_SHORT_TEXT, FILE_READ);
    size_t smallFileSize = smallFile.size();
    smallFile.close();

    ESP_LOGI(TAG_TEST, "Preparing checksum for file: %s, size: %i", PATH_SHORT_TEXT, smallFileSize);

    // Given
    unsigned long checksumStart = millis();

    // When
    String checksum = testSubject.checksum(PATH_SHORT_TEXT, 256);
    unsigned long timeTaken = millis() - checksumStart;

    // Then
    TEST_ASSERT_EQUAL_STRING("21a3", checksum.c_str());

    ESP_LOGI(TAG_TEST, "Checksum for file: %s, size: %i bytes, checksum: %s, took : %lu mills",
                  PATH_SHORT_TEXT, smallFileSize, checksum.c_str(), timeTaken);
}

void testLargeFileChecksum_OK(void)
{

    File largeFile = fileManager.openFile(PATH_LONG_TEXT, FILE_READ);
    size_t largeFileSize = largeFile.size();
    largeFile.close();

    ESP_LOGI(TAG_TEST, "Preparing checksum for file: %s, size: %i", PATH_LONG_TEXT, largeFileSize);

    // Given
    unsigned long checksumStart = millis();

    // When
    size_t bytesRead = 0; 
    String checksum = testSubject.checksum(PATH_LONG_TEXT, 5120, &bytesRead);
    unsigned long timeTaken = millis() - checksumStart;

    // Then
    TEST_ASSERT_EQUAL_STRING("824e", checksum.c_str());

    ESP_LOGI(TAG_TEST, "Checksum for file: %s, size: %i bytes, checksum: %s, took : %lu mills",
                  PATH_SHORT_TEXT, largeFileSize, checksum.c_str(), timeTaken);

    // Verify state is correct when checksum calculated 
    TEST_ASSERT_EQUAL(largeFileSize, bytesRead);
}

void testSmallFileSmallChecksum_OK(void)
{

    File smallFile = fileManager.openFile(PATH_SHORT_TEXT, FILE_READ);
    size_t smallFileSize = smallFile.size();
    smallFile.close();

    ESP_LOGI(TAG_TEST, "Preparing checksum for file: %s, size: %i", PATH_SHORT_TEXT, smallFileSize);

    // Given
    unsigned long checksumStart = millis();

    // Whenfunction
    String checksum = testSubject.checksum(PATH_SHORT_TEXT, 1);
    unsigned long timeTaken = millis() - checksumStart;

    // Then
    TEST_ASSERT_EQUAL_STRING("21a3", checksum.c_str());

    ESP_LOGI(TAG_TEST, "Checksum for file: %s, size: %i bytes, checksum: %s, took : %lu mills",
                  PATH_SHORT_TEXT, smallFileSize, checksum.c_str(), timeTaken);
}

void testSameFileDifferentBuffers_sameChecksum(void)
{

    // Given
    File smallFile = fileManager.openFile(PATH_SHORT_TEXT, FILE_READ);
    size_t smallFileSize = smallFile.size();
    smallFile.close();

    ESP_LOGI(TAG_TEST, "Preparing checksum for file: %s, size: %i", PATH_SHORT_TEXT, smallFileSize);

    // When
    String checksum1 = testSubject.checksum(PATH_SHORT_TEXT, 1);
    String checksum2 = testSubject.checksum(PATH_SHORT_TEXT, 32);
    String checksum3 = testSubject.checksum(PATH_SHORT_TEXT, UINT16_MAX);

    // Then
    TEST_ASSERT_EQUAL_STRING("21a3", checksum1.c_str());
    TEST_ASSERT_EQUAL_STRING(checksum1.c_str(), checksum2.c_str());
    TEST_ASSERT_EQUAL_STRING(checksum2.c_str(), checksum3.c_str());
    TEST_ASSERT_EQUAL_STRING(checksum1.c_str(), checksum3.c_str());
}

void testZeroBuferSizeOrExceded_emptyString(void)
{

    // When
    String cs1 = testSubject.checksum(PATH_SHORT_TEXT, 0);
    // String cs2 = fileManager.checksum(PATH_SHORT_TEXT, UINT16_MAX + 1); // This wil be visible as 0 inside

    // Then
    TEST_ASSERT_EQUAL_STRING(emptyString.c_str(), cs1.c_str());
    // TEST_ASSERT_EQUAL_STRING(emptyString.c_str(), cs2.c_str());
}

void testNonExistentFile_emptyString(void)
{

    // Given
    unsigned long checksumStart = millis();

    // When
    String checksum = testSubject.checksum(PATH_NOT_EXIST, 256);
    unsigned long timeTaken = millis() - checksumStart;

    // Then
    TEST_ASSERT_EQUAL_STRING(emptyString.c_str(), checksum.c_str());
    ESP_LOGI(TAG_TEST, "Executed checksum for non existent file, took : %lu mills", timeTaken);
}

void testSourceIsDirectory_emptyString(void)
{
    // Given
    unsigned long checksumStart = millis();

    // When
    String checksum = testSubject.checksum(PATH_TEST_DIR, 256);
    unsigned long timeTaken = millis() - checksumStart;

    // Then
    TEST_ASSERT_EQUAL_STRING(emptyString.c_str(), checksum.c_str());
    ESP_LOGI(TAG_TEST, "Executed checksum for directory, took : %lu mills", timeTaken);
}

// Actual test runner
void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(testSmallFileChecksum_OK);
    RUN_TEST(testLargeFileChecksum_OK);
    RUN_TEST(testSmallFileSmallChecksum_OK);
    RUN_TEST(testSameFileDifferentBuffers_sameChecksum);
    RUN_TEST(testZeroBuferSizeOrExceded_emptyString);
    RUN_TEST(testNonExistentFile_emptyString);
    RUN_TEST(testSourceIsDirectory_emptyString);

    UNITY_END(); // stop unit testing
}

void loop()
{
}