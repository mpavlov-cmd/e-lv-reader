#include "FileIndex.h"

FileIndex::FileIndex(
    const char *fileName,
    const char *filePath,
    const char *fileExt,
    const uint32_t fileSize,
    const boolean isDirectory
):
 size(fileSize), isDir(isDirectory)
{
    strncpy(name, fileName, MAX_NAME_LENGTH - 1);
    name[MAX_NAME_LENGTH - 1] = '\0'; // Ensuring null-termination

    strncpy(path, filePath, MAX_PATH_LENGTH - 1);
    path[MAX_PATH_LENGTH - 1] = '\0'; // Ensuring null-termination

    strncpy(ext, fileExt, MAX_EXT_LENGTH - 1);
    ext[MAX_EXT_LENGTH - 1] = '\0'; // Ensuring null-termination
}

const char* FileIndex::getName()
{
    return name;
}

const char *FileIndex::getPath()
{
    return path;
}

const char *FileIndex::getExt()
{
    return ext;
}

const uint32_t FileIndex::getSize()
{
    return size;
}

const boolean FileIndex::getIsDirectry()
{
    return isDir;
}
