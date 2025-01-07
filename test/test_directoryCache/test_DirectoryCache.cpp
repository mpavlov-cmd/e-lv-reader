#define ENABLE_GxEPD2_GFX 1

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
#include <cache/DirectoryCache.h>

FileManager fileManager(SD, PIN_CS_SD);
DirectoryCache testSubject(fileManager);
DirectoryCache::Model testModel;

const char *PATH_TEST_DIR = "/.test";

void setUp(void)
{
    Serial.begin(115200);

    // Initalize display as it is used by text index
    SPI.begin();
    fileManager.begin();

    // Prepare json for read test
    fileManager.createDir(PATH_TEST_DIR);

    ESP_LOGI(TAG_TEST, "---------------- SETUP DONE ----------------");
}

void tearDown(void)
{
    // Clean stuff up here
    ESP_LOGI(TAG_TEST, "---------------- TEST DONE ----------------");
    fileManager.removeDirRecursive(PATH_TEST_DIR);
}

void testGetCacheFilePathNull_returnsNullptr(void)
{
    // Given
    String cacheFilePath;

    // When
    const char *result = testSubject.getCacheFilePath(nullptr, cacheFilePath);

    // Then
    TEST_ASSERT_NULL(result);
}

void testGetCacheFilePathCorrect_returnsCachePath(void)
{
    // Given
    const char *path1 = "/.test";
    const char *path2 = "/.test/";

    String cacheFilePath1 = "Initia Text";
    String cacheFilePath2;

    // When
    const char *result1 = testSubject.getCacheFilePath(path1, cacheFilePath1);
    const char *result2 = testSubject.getCacheFilePath(path1, cacheFilePath2);

    // Then
    TEST_ASSERT_EQUAL_STRING("/.test/.dir_cache/cache.json", result1);
    TEST_ASSERT_EQUAL_STRING(result1, result2);
}

void testReadWhenNoFile_returnsFalse(void)
{
    // Given
    DirectoryCache::Model emptyModel;

    // When
    bool result = testSubject.read(PATH_TEST_DIR, emptyModel);

    // Then
    TEST_ASSERT_FALSE(result);
}

void testReadWhenInvalidFile_returnsFalse(void)
{

    // Given
    DirectoryCache::Model emptyModel;
    String fileName;

    const char *fullPath = testSubject.getCacheFilePath(PATH_TEST_DIR, fileName);
    const char *parentDir = getParentDir(fullPath);

    fileManager.createDir(parentDir);
    fileManager.writeFile(fullPath, "invalid json");

    // When
    bool result = testSubject.read(PATH_TEST_DIR, emptyModel);

    // Then
    TEST_ASSERT_FALSE(result);
}

void testReadWhenValidFile_returnsTrue(void)
{
    // Given
    DirectoryCache::Model emptyModel;
    String fileName;

    const char *fullPath = testSubject.getCacheFilePath(PATH_TEST_DIR, fileName);
    const char *parentDir = getParentDir(fullPath);

    fileManager.createDir(parentDir);
    fileManager.writeFile(
        fullPath,
        "{\"curFileIdx\":1,\"totalFiles\":100,\"lastOpened\":1734792636,\"curFileNme\":\"test.txt\"}");

    // When
    bool result = testSubject.read(PATH_TEST_DIR, emptyModel);

    // Then
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(1, emptyModel.curFileIdx);
    TEST_ASSERT_EQUAL(100, emptyModel.totalFiles);
    TEST_ASSERT_EQUAL(1734792636, emptyModel.lastOpened);
    TEST_ASSERT_EQUAL_STRING("test.txt", emptyModel.curFileNme);    
}

void testWriteWhenNullPath_returnsFalse(void)
{
    // Given
    DirectoryCache::Model emptyModel;

    // When
    bool result = testSubject.write(nullptr, emptyModel);

    // Then
    TEST_ASSERT_FALSE(result);
}

void testWriteWhenValidPath_returnsTrue(void) 
{
    // Given
    DirectoryCache::Model model{2, 200, (unsigned long) 34792636, "write_test.txt"};

    // When
    bool result = testSubject.write(PATH_TEST_DIR, model);

    // Then 
    // Verify result is true
    TEST_ASSERT_TRUE(result);

    String filePath;
    const char *filePathC = testSubject.getCacheFilePath(PATH_TEST_DIR, filePath);

    // Verify fle exists
    bool fileExists = fileManager.exists(filePathC);
    TEST_ASSERT_TRUE(fileExists);

    // File contents is expecetd
    const char *fileContent 
        = "{\"curFileIdx\":2,\"totalFiles\":200,\"lastOpened\":34792636,\"curFileNme\":\"write_test.txt\"}";

    char buffer[1024];
    fileManager.readFileToBuffer(filePathC, buffer, 1024);
    TEST_ASSERT_EQUAL_STRING(fileContent, buffer);

    // Verify read own write works as expecetd
    DirectoryCache::Model emptyModel;
    bool readResult = testSubject.read(PATH_TEST_DIR, emptyModel);

    TEST_ASSERT_TRUE(readResult);
    TEST_ASSERT_EQUAL(2, emptyModel.curFileIdx);
    TEST_ASSERT_EQUAL(200, emptyModel.totalFiles);
    TEST_ASSERT_EQUAL(34792636, emptyModel.lastOpened);
    TEST_ASSERT_EQUAL_STRING("write_test.txt", emptyModel.curFileNme); 
}

// Actual test runner
void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(testGetCacheFilePathNull_returnsNullptr);
    RUN_TEST(testGetCacheFilePathCorrect_returnsCachePath);
    RUN_TEST(testReadWhenNoFile_returnsFalse);
    RUN_TEST(testReadWhenInvalidFile_returnsFalse);
    RUN_TEST(testReadWhenValidFile_returnsTrue);
    RUN_TEST(testWriteWhenNullPath_returnsFalse);
    RUN_TEST(testWriteWhenValidPath_returnsTrue);

    UNITY_END(); // stop unit testing
}

void loop()
{
}