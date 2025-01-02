#ifndef FILEINDEX_H
#define FILEINDEX_H

#include <Arduino.h>

#define MAX_NAME_LENGTH 64    // Maximum length for the file name
#define MAX_PATH_LENGTH 128   // Maximum length for the file path
#define MAX_EXT_LENGTH  12

struct FileIndex
{
    private:
        char name[MAX_NAME_LENGTH];
        char path[MAX_PATH_LENGTH];
        char ext[MAX_EXT_LENGTH];
        const uint32_t size;
        const boolean isDir;

    public:

        // Primary
        FileIndex(
            const char* fileName,
            const char* filePath,
            const char* fileExt,
            const uint32_t fileSize,
            const boolean isDirectry
        );

        // Secondary
        // TODO: Add empty string constant
        FileIndex() : FileIndex("", "", "", 0, false) {};
        FileIndex(const FileIndex& other) :
            FileIndex(other.name, other.path, other.ext, other.size, other.isDir) {}; 

        const char* getName();
        const char* getPath();
        const char* getExt();
        const uint32_t getSize();
        const boolean getIsDirectry();

        ~FileIndex() {
            // no action since delete should be called only for dynamically allocated memory: objects and arrays created with
            // new operator
        }
};

#endif
