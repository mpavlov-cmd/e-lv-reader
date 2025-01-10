#include "IntentBook.h"

void IntentBook::bookLoadingTask()
{
    Serial.println("Book loading task stated");

    // Index Book
    TextIndex::Conf conf{
        (uint16_t)(textBox.width - textBox.padding),
        (uint16_t)(textBox.height - textBox.padding),
        &lv_font_montserrat_18,
        50,
        false
    };

    // Configure and run text index
    textIndex.configure(conf);
    String textIndexDirPath = textIndex.index(bookPath);

    // Copy path to intent local variabe
    strlcpy(bookIndexPath, textIndexDirPath.c_str(), sizeof(bookIndexPath));
    Serial.printf("-- Index Generated! Dir: %s --\n", bookIndexPath);

    DirectoryCache::Model dirCacheModel;
    bool hasDirCache = directoryCache.read(bookIndexPath, dirCacheModel);

    if (!hasDirCache)
    {
        Set<FileIndex> fileIndex(1024);
        fileManager.indexDirectory(bookIndexPath, DirIndexConf::NO_DIR, fileIndex);

        if (fileIndex.size() == 0)
        {
            Serial.println("Coud not index directory TODO: Handle this case");
            return;
        }

        FileIndex *firstPage = fileIndex.getItem(0);
        Serial.printf("First page file name: %s\n", firstPage->getName());

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
            Serial.println("Failed to write directory cache");
            return;
        }
    }

    String pagePath =dirCacheModel.filePath;
    const char *pagePacthC = pagePath.c_str();

    // TODO: Resut ignored
    fileManager.readFileToBuffer(pagePacthC, bookPage, PAGE_BUFFER_SIZE);

    Serial.println("Page:");
    Serial.println(bookPage);

    // Mark page as ready
    pageReady = true;
    vTaskDelete(NULL);
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
    Serial.printf("Book intent started with arg: %s\n", arg.strValue);
    strlcpy(bookPath, arg.strValue, sizeof(bookPath));

    // Disable invalidate vidget on input
    lv_joystick_invalidate(false);

    widgetText = new WidgetText(widgetGroup, eventQueue);

    modelText = ModelText::newPtr();
    modelText->box = textBox;
    modelText->font = &lv_font_montserrat_18;
    modelText->hasAction = true;

    // Ensure UI task is runnng with higher priority
    xTaskCreate(bookLoadingEntry, "bookLoading", 1024 * 10, this, 1, &bookLoadingHandle);
}

void IntentBook::onFrequncy()
{
    if (pageReady && !pageShown) {
        Serial.println("IntentBook on frequency");
        Serial.println(bookPage);

        modelText->text = String(bookPage);
        widgetText->upgrade(*modelText);

        // Reset page shown
        pageShown = true;
    }
}

void IntentBook::onExit()
{
    Serial.println("IntentBook onExit");
    lv_joystick_invalidate(true); // Re-inable on input invalidation
}

ActionResult IntentBook::onAction(ActionArgument arg)
{
    Serial.println("IntentBook onAction");

    uint32_t key = lv_indev_get_key(lv_get_keypad());
    Serial.println(key);

    // TODO: Temp return home on click, todo: add menu;
    if (arg.code == LV_EVENT_CLICKED)
	{
        return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
    }

    // Handle page up and down
    if (key == LV_KEY_UP || key == LV_KEY_DOWN)  {
        // Serial.println("Up or down");
    }

    // Default page navigation
    if (key == LV_KEY_NEXT || key == LV_KEY_PREV)  {
    
        Serial.printf("Opening next page for cached book: %s\n", bookPath);

        DirectoryCache::Model cacheModel;
        bool result = directoryCache.read(bookIndexPath, cacheModel);
        if (!result) {
            Serial.println("Error opening page");
        }

        // TODO: Hande max and min page nums 
        uint16_t pageNum = cacheModel.curFileIdx;
        pageNum++;

        String pagePath = String(getParentDir(cacheModel.filePath)) + "/._" + String(pageNum) + ".page.txt";
        const char* pagePathC = pagePath.c_str();
        Serial.printf("Next page path: %s\n", pagePathC);

        // TODO: Resut ignored
        fileManager.readFileToBuffer(pagePathC, bookPage, PAGE_BUFFER_SIZE);
        modelText->text = bookPage;

        Serial.println("Printing page...");
        widgetText->upgrade(*modelText);

        // Save last opened page
        cacheModel.curFileIdx = pageNum;
        strlcpy(cacheModel.filePath, pagePathC, sizeof(cacheModel.filePath));
        bool writeResult = directoryCache.write(bookIndexPath, cacheModel);
    }

    return ActionResult::VOID;
}

uint8_t IntentBook::getId()
{
    return ID;
}
