#include "FileManager.h"

const DirIndexConf DirIndexConf::FULL       = {0, true, true, false, nullptr};
const DirIndexConf DirIndexConf::NO_HIDDEN  = {0, true, false, false, nullptr};
const DirIndexConf DirIndexConf::NO_DIR     = {0, false, true, false, nullptr};

FileManager::FileManager(fs::FS &fs, const uint8_t csPin) : fs(fs), csPin(csPin)
{
}

uint64_t FileManager::begin()
{
    cardAvailable = SD.begin(csPin);
    if (!cardAvailable)
    {
        ESP_LOGE(TAG_FM, "Card Mount Failed");

        this->cardSize = 0;
        return cardSize;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        ESP_LOGE(TAG_FM, "No SD card attached");

        this->cardSize = 0;
        return cardSize;
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    ESP_LOGI(TAG_FM, "FS Monunted. SD Card Size: %lluMB", cardSize);

    return cardSize;
}

bool FileManager::exists(const char *path)
{
    return fs.exists(path);
}

File FileManager::openFile(const char *path, const char *mode)
{
    ESP_LOGD(TAG_FM, "Opening file: %s", path);
    return fs.open(path, mode);
}

bool FileManager::indexDirectory(const char *path, const DirIndexConf conf, Set<FileIndex>& result)
{
    ESP_LOGD(TAG_FM, "Indexing directory: %s", path);
    File root = fs.open(path, FILE_READ);

    // TODO: Duplicate code
    if (!root)
    {
        ESP_LOGE(TAG_FM, "Failed to open directory");
        return false;
    }
    if (!root.isDirectory())
    {
        ESP_LOGE(TAG_FM, "Not a directory");
        root.close();
        return false;
    }

    // Init Dir Index
    File file = root.openNextFile();
    while (file)
    {
        // ESP_LOGD(TAG_FM, "Opening file: %s", file.name());
        if (!conf.showDir && file.isDirectory())
        {
            closeAndOpenNext(root, file);
            continue;
        }

        if (!conf.showHidden && file.name()[0] == '.')
        {
            closeAndOpenNext(root, file);
            continue;
        }

        // Finding extention
        const char* fileExt = getFileExtension(file.name());

        // Filter by ext. See correct way to compare 2 strings (const char *)
        if (conf.filterByExt && strcmp(conf.extNeedle, fileExt) != 0)
        {
            closeAndOpenNext(root, file);
            continue;
        }

        // ESP_LOGD(TAG_FM, Found file ext: %s", fileExt);
        FileIndex *fileIndex = new FileIndex(file.name(), file.path(), fileExt, file.size(), file.isDirectory());
        result.addItem(fileIndex);

        // Limiting results, in case limit is set to 0 it will be ignored
        if (conf.limit != 0 && result.size() >= conf.limit)
        {
            file.close();
            break;
        }

        closeAndOpenNext(root, file);
    }

    root.close();
    return true;
}

bool FileManager::writeFile(const char *path, const char *message)
{
    ESP_LOGD(TAG_FM, "Writing file: %s", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        ESP_LOGE(TAG_FM, "Failed to open file for writing");
        return false;
    }
    bool result = file.print(message);
    file.close();

    return result;
}

void FileManager::removeDirRecursive(const char *path, uint16_t *removedFiles)
{
    File root = fs.open(path);

    ESP_LOGD(TAG_FM, "Attempt for recursively remove directory: %s", path);
    if (!root)
    {
        ESP_LOGE(TAG_FM, "Failed to open directory");
        return;
    }

    if (!root.isDirectory())
    {
        ESP_LOGE(TAG_FM, "Not a directory");
        root.close();
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            // Recursively delete contents of this directory
            removeDirRecursive(file.path());

            ESP_LOGD(TAG_FM, "Deleted directory: %s", file.path());
        }
        else
        {
            // It's a file, delete it
            fs.remove(file.path());
            ESP_LOGD(TAG_FM, "Deleted file: %s", file.path());

            // Increent counter in case it was provided
            if (removedFiles != nullptr)
            {
                (*removedFiles)++;
            }
        }
        // Move to the next file or directory in the folder
        file.close();
        file = root.openNextFile();
    }

    // Once all files and folders are deleted, delete the root directory itself
    file.close();
    root.close();
    fs.rmdir(path);

    // Increent counter in case it was provided
    if (removedFiles != nullptr)
    {
        (*removedFiles)++;
    }

    ESP_LOGD(TAG_FM, "Deleted root directory: %s", path);
}

bool FileManager::readFileToBuffer(const char *path, char *buffer, size_t bufferSize)
{
    File file = fs.open(path);
    if (!file)
    {
        ESP_LOGE(TAG_FM, "Failed to open file for reading");
        return false;
    }

    size_t index = 0;

    while (file.available() && index < bufferSize - 1) // Leave space for null terminator
    {
        buffer[index++] = file.read();
    }
    buffer[index] = '\0'; // Null terminate the string

    file.close();

    return true;
}

void FileManager::deleteFile(const char *path)
{
    ESP_LOGD(TAG_FM, "Deleting file: %s", path);
    if (fs.remove(path))
    {
        ESP_LOGD(TAG_FM, "File deleted");
    }
    else
    {
        ESP_LOGE(TAG_FM, "Delete failed");
    }
}

void FileManager::listDir(const char *dirname, uint8_t levels)
{
    ESP_LOGD(TAG_FM, "Listing directory: %s", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        ESP_LOGE(TAG_FM, "Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        ESP_LOGE(TAG_FM, "Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            ESP_LOGD(TAG_FM, "DIR: %s", file.name());
            if (levels)
            {
                listDir(file.path(), levels - 1);
            }
        }
        else
        {
            ESP_LOGD(TAG_FM, "FILE: %s; SIZE: %i", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}

void FileManager::createDir(const char *path)
{
    ESP_LOGD(TAG_FM, "Creating Dir: %s", path);
    if (fs.mkdir(path))
    {
         ESP_LOGD(TAG_FM, "Dir created");
    }
    else
    {
        ESP_LOGE(TAG_FM, "mkdir failed");
    }
}

void FileManager::removeDir(const char *path)
{
    ESP_LOGD(TAG_FM, "Removing Dir: %s", path);
    if (fs.rmdir(path))
    {
        ESP_LOGD(TAG_FM, "Dir removed");
    }
    else
    {
        ESP_LOGE(TAG_FM, "rmdir failed");
    }
}

void FileManager::readFile(const char *path)
{
    ESP_LOGI(TAG_FM, "Reading file: %s", path);

    File file = fs.open(path);
    if (!file)
    {
        ESP_LOGE(TAG_FM, "Failed to open file for reading");
        return;
    }

    ESP_LOGI(TAG_FM, "Read from file:");
    while (file.available())
    {
        // TODO: Rewrite to log
        Serial.write(file.read());
    }
    ESP_LOGI(TAG_FM, "");
    file.close();
}


void FileManager::appendFile(const char *path, const char *message)
{
    ESP_LOGD(TAG_FM, "Appending to file: %s", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        ESP_LOGE(TAG_FM, "Failed to open file for appending");
        return;
    }

    if (file.print(message))
    {
        ESP_LOGD(TAG_FM, "Message appended");
    }
    else
    {
        ESP_LOGE(TAG_FM, "Append failed");
    }
    file.close();
}

void FileManager::renameFile(const char *path1, const char *path2)
{
    ESP_LOGD(TAG_FM, "Renaming file %s to %s", path1, path2);
    if (fs.rename(path1, path2))
    {
        ESP_LOGD(TAG_FM, "File renamed");
    }
    else
    {
       ESP_LOGE(TAG_FM, "Rename failed");
    }
}

void FileManager::closeAndOpenNext(File &root, File &file)
{
    file.close();
    file = root.openNextFile();
}
