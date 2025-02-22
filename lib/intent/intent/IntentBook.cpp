#include "IntentBook.h"

void IntentBook::bookLoadingTask()
{
    ESP_LOGD(TAG_INTNT, "Book loading task stated");
    uint16_t indexWidth = boxText.width - boxText.padding;
    uint16_t indexHeight = boxText.height - boxText.padding;

    // Index Book
    TextIndex::Conf conf{indexWidth, indexHeight, &lv_font_montserrat_18, 20, false};

    // Configure and run text index
    textIndex.configure(conf);
    String textIndexDirPath = textIndex.index(bookPath);

    // Copy path to intent local variabe
    strlcpy(bookIndexPath, textIndexDirPath.c_str(), sizeof(bookIndexPath));
    ESP_LOGV(TAG_INTNT, "Index Generated! Dir: %s", bookIndexPath);

    DirectoryCache::Model dirCacheModel;
    bool hasDirCache = directoryCache.read(bookIndexPath, dirCacheModel);

    if (!hasDirCache)
    {
        Set<FileIndex> fileIndex(1024);
        fileManager.indexDirectory(bookIndexPath, DirIndexConf::NO_DIR, fileIndex);

        if (fileIndex.size() == 0)
        {
            ESP_LOGE(TAG_INTNT, "Coud not index directory TODO: Handle this case");
            return;
        }

        FileIndex *firstPage = fileIndex.getItem(0);
        ESP_LOGV(TAG_INTNT, "First page file name: %s", firstPage->getName());

        dirCacheModel.curFileIdx = 0;
        dirCacheModel.totalFiles = fileIndex.size();
        dirCacheModel.lastOpened = millis();

        strlcpy(
            dirCacheModel.filePath,        // <- destination
            firstPage->getPath(),          // <- source
            sizeof(dirCacheModel.filePath) // <- destination's capacity
        );

        bool writeResult = directoryCache.write(bookIndexPath, dirCacheModel);
        if (!writeResult)
        {
            ESP_LOGE(TAG_INTNT, "Failed to write directory cache");
            return;
        }
    }

    String pagePath = dirCacheModel.filePath;
    const char *pagePacthC = pagePath.c_str();

    // TODO: Resut ignored
    fileManager.readFileToBuffer(pagePacthC, bookPage, PAGE_BUFFER_SIZE);

    // Set values for bookStat;
    modelBookStat->currentPage = dirCacheModel.curFileIdx + 1;
    modelBookStat->totalPages  = dirCacheModel.totalFiles;

    ESP_LOGV(TAG_INTNT, "Page: %s", bookPage);

    // Mark page as ready
    pageReady = true;
    vTaskDelete(NULL);
}

uint16_t IntentBook::newPageIndex(uint16_t currentPageIndex, uint16_t maxPages, bool increment)
{
    if (maxPages == 0)
    {
        // If there are no pages, return 0 as a default.
        return 0;
    }

    if (increment)
    {
        // Increment and wrap around if necessary.
        return (currentPageIndex + 1) % maxPages;
    }
    else
    {
        // Decrement and wrap around if necessary.
        return (currentPageIndex == 0) ? (maxPages - 1) : (currentPageIndex - 1);
    }
}

// void IntentBook::bookDiaplayTask()
// {
    // Serial.println("Book display task stated");

    // widgetText = new WidgetText(display);
    // ModelText modelTextObj = {textBox, CENTER_CENTER, "Loading book..."};

    // EventBits_t uxBits;
    // while (true)
    // {
    //     Serial.println("UI thread waiting for evernts");
    //     uxBits = xEventGroupWaitBits(
    //         bookEventGroup, 
    //         BIT0 | BIT1 | BIT2 | BIT3,
    //         pdFALSE,
    //         pdFALSE,
    //         portMAX_DELAY
    //     );

    //     // Book loading started
    //     if ((uxBits & BIT0) != 0)
    //     {
    //         Serial.println("BIT0 set");

    //         xSemaphoreTake(displaySemaphoreHandle, portMAX_DELAY);
    //         widgetText->upgrade(modelTextObj);
    //         xSemaphoreGive(displaySemaphoreHandle);

    //         xEventGroupClearBits(bookEventGroup, BIT0);
    //         continue;
    //     }

    //     // Stargetd book idex
    //     if ((uxBits & BIT1) != 0)
    //     {
    //         Serial.println("BIT1 set");

    //         // Check if indexing coompleted and exit
    //         if ((uxBits & BIT2) != 0)
    //         {
    //             Serial.println("BIT2 set, clearing all bites");
    //             xEventGroupClearBits(bookEventGroup, BIT1 | BIT2);
    //             continue;
    //         }

    //         // Create widget output
    //         TextIndex::StatusValue indexStatus = textIndex.status();
    //         String statusValue = String(indexStatus.value);
    //         String statusText;

    //         switch (indexStatus.status)
    //         {
    //         case TextIndex::STATUS_CHECKSUM:
    //             statusText = "Calculatig checksum:";
    //             break;
            
    //         case TextIndex::STATUS_CLEANUP:
    //             statusText = "Cleaning-up files:";
    //             break;
            
    //         case TextIndex::STATUS_INDEXING:
    //             statusText = "Indexing Pages:";
    //             break;

    //         default:
    //             statusText = "Loading Book...";
    //             break;
    //         }

    //         Serial.printf("%s %s\n",statusText.c_str(), statusValue.c_str());
    //         modelTextObj.text = statusText + " " + statusValue;

    //         xSemaphoreTake(displaySemaphoreHandle, portMAX_DELAY);
    //         widgetText->upgrade(modelTextObj);
    //         xSemaphoreGive(displaySemaphoreHandle);

    //         vTaskDelay(250 / portTICK_RATE_MS);
    //         continue;
    //     }

    //     if ((uxBits & BIT3) != 0)
    //     {

    //         Serial.println("BIT3 set, printing book and exiting");
    //         modelTextObj.text = bookPage;

    //         xSemaphoreTake(displaySemaphoreHandle, portMAX_DELAY);
    //         widgetText->upgrade(modelTextObj);
    //         xSemaphoreGive(displaySemaphoreHandle);

    //         xEventGroupClearBits(bookEventGroup, BIT3);
    //         // Exit UI thread loop
    //         break;
    //     }
    // }

    // // delete widgetText;
    // vTaskDelete(NULL);
// }

IntentBook::IntentBook(QueueHandle_t& mEventQueue, FileManager &fileManager, TextIndex &textIndex, DirectoryCache &directoryCache) :
    AbstractIntent(mEventQueue), fileManager(fileManager), textIndex(textIndex), directoryCache(directoryCache) {}

void IntentBook::onStartUp(IntentArgument arg)
{
    ESP_LOGD(TAG_INTNT, "Book intent started with arg: %s", arg.strValue);
    strlcpy(bookPath, arg.strValue, sizeof(bookPath));

    // Disable invalidate vidget on input
    // lv_joystick_invalidate(false);

    // Inint wdgets
    widgetText = new WidgetText(widgetGroup, eventQueue);

    modelText = ModelText::newPtr();
    modelText->box = boxText;
    modelText->font = &lv_font_montserrat_18;
    modelText->hasAction = true;

    widgetBookStat = new WidgetBookStat(widgetGroup, eventQueue);
    modelBookStat  = new ModelBookStat{boxBookStat, 0, 0, &lv_font_montserrat_14};

    // Ensure UI task is runnng with higher priority
    xTaskCreate(bookLoadingEntry, "bookLoading", 1024 * 10, this, 1, &bookLoadingHandle);
}

void IntentBook::onFrequncy()
{
    if (pageReady && !pageShown) {
        ESP_LOGV(TAG_INTNT, "IntentBook on frequency");

        // Display text
        modelText->text = String(bookPage);
        widgetText->upgrade(*modelText);

        widgetBookStat->upgrade(*modelBookStat);

        // Reset page shown
        pageShown = true;
    }
}

void IntentBook::onExit()
{
    ESP_LOGD(TAG_INTNT, "IntentBook onExit");
    // lv_joystick_invalidate(true); // Re-inable on input invalidation
}

ActionResult IntentBook::onAction(ActionArgument arg)
{
    ESP_LOGV(TAG_INTNT, "IntentBook onAction");

    uint32_t key = lv_indev_get_key(lv_get_keypad());
    ESP_LOGD(TAG_INTNT, "Last key input: %i",  key);

    if (arg.code == LV_EVENT_PRESSED)
	{
        return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
    }

    // Handle page up and down
    if (key == LV_KEY_UP || key == LV_KEY_DOWN)  {
        ESP_LOGD(TAG_INTNT, "Up or down");
    }

    // Default page navigation
    if (key == LV_KEY_NEXT || key == LV_KEY_PREV)  {
    
        ESP_LOGV(TAG_INTNT, "Opening next page for cached book: %s", bookPath);

        DirectoryCache::Model cacheModel;
        bool result = directoryCache.read(bookIndexPath, cacheModel);
        if (!result) {
            ESP_LOGE(TAG_INTNT, "Error opening page");
        }

        bool increment = key == LV_KEY_NEXT;
        uint16_t nextPageIndex = newPageIndex(cacheModel.curFileIdx, cacheModel.totalFiles, increment);

        String pagePath = String(getParentDir(cacheModel.filePath)) + "/._" + String(nextPageIndex) + ".page.txt";
        const char* pagePathC = pagePath.c_str();
        ESP_LOGD(TAG_INTNT, "Next page path: %s", pagePathC);

        // TODO: Resut ignored
        fileManager.readFileToBuffer(pagePathC, bookPage, PAGE_BUFFER_SIZE);

        // Save last opened page
        cacheModel.curFileIdx = nextPageIndex;
        strlcpy(cacheModel.filePath, pagePathC, sizeof(cacheModel.filePath));
        bool writeResult = directoryCache.write(bookIndexPath, cacheModel);

        // Print values
        ESP_LOGD(TAG_INTNT, "Printing page...");

        modelText->text = bookPage;
        modelBookStat->currentPage = nextPageIndex + 1;

        widgetText->upgrade(*modelText);
        widgetBookStat->upgrade(*modelBookStat);
    }

    return ActionResult::VOID;
}

uint8_t IntentBook::getId()
{
    return ID;
}
