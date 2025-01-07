#ifndef DIRECTORYCACHE_H
#define DIRECTORYCACHE_H

#include <Arduino.h>
#include <FileManager.h>
#include <ArduinoJson.h>

struct DirectoryCache
{

private:
    FileManager &fileManager;

    static constexpr const char* chacheDirName = ".dir_cache";
    static constexpr const char* chacheFileName = "cache.json";

public:
    struct Model
    {
        uint16_t curFileIdx;
        uint16_t totalFiles;
        unsigned long lastOpened;
        char curFileNme[1024];
    };

    DirectoryCache(FileManager &fileManager);

    /**
     * Reads cache file and returns model based on the cache file contents, 
     * 
     * @param path path to the directory to be cached
     * @param model model to be filled with cache data
     * 
     * @returns true if cache file has been read successfully, false in case of error or no cache file
     */
    bool read(const char *path, Model &model);

    /**
     * Writes cache file with the model data
     * 
     * @param path path to the directory to be cached
     * @param model model to be written to the cache file
     * 
     * @returns true if cache file has been written successfully
     */
    bool write(const char *path, Model &model);


    /**
     * Returns cache file name based on the path
     * 
     * @param path path to the directory to be cached
     * @param cacheFilePath container for the path to be created in the method
     * 
     * @returns cacheFilePath.cstr() if cache file path has been created successfully, nullptr otherwise
     */
    const char* getCacheFilePath(const char *path, String &cacheFilePath);
};

#endif