#ifndef TEXTINDEX_H
#define TEXTINDEX_H

#include "lvgl.h"
#include <Arduino.h>
#include <FileManager.h>
#include <path/PathUtils.h>
#include <checksum/AdlerChecksum.h>

struct TextIndex {

    public:

        enum Status {
            STATUS_IDLE,
            STATUS_CHECKSUM,
            STATUS_CLEANUP,
            STATUS_INDEXING,
        };

        struct StatusValue {
            TextIndex::Status status;
            size_t value;
        };

        struct Conf
        {
            static const TextIndex::Conf DFT;

            const uint16_t textWidth;
            const uint16_t textHeight;
            const uint16_t pageLimit;
            const bool forceIndex;
        };
        
        TextIndex(FileManager& fileManager);

        /**
         * Generates index of the text file by splitting it by pages and saving every page to the separate file in the folder
         * 
         * @param path full path to the file to index;
         * @returns absolute address pointing to the folder where index is created or emptyStrig in case of error
         */
        String index(const char *path);

        /**
         * Returns current page index, and used to indicate progress during async indexing
         * 
         * @returns current status depending on what operation is now ongoing
         */
        StatusValue status() const;

        /**
         * Setting configuration for indexing.
         * Calculates space width, max lines per page; To be executed once font, or text area size changes 
         * 
         * @param conf self explenatory
        */
        void configure(const TextIndex::Conf& conf);

    private:

        FileManager& fm;        

        TextIndex::Status currentStatus = TextIndex::STATUS_IDLE;
        size_t currentStatusVaue = 0;

        uint16_t textAreaWidth  = 480;
        uint16_t textAreaHeight = 760;

        // Define variables for text size detection
        int16_t x = 0, y = 0;
        uint16_t width = 0, height = 0, wordWidth = 0, linewidth = 0;
        uint8_t spaceWidth = 0;

        // LVGL Config
        lv_text_flag_t lvTextFlag = LV_TEXT_FLAG_NONE;
        const lv_font_t* lvFont = &lv_font_montserrat_24; 

        // Define containers for word, line and page
        String currentPage;
	    String currentLine; // Holds the text for the current line
	    String currentWord; // Holds the current word being built

        // int currentLineWidth = 0;	// Tracks the pixel width of the current line
        char c;
        bool skipLeadingSpaces = true;  // Skip leading spaces on new lines
        bool wrappedLine       = false; // Track if the last line was wrapped
        bool spaceAfterWrap    = false; 
        bool forceIndex        = false;

        uint16_t lineIndex = 0;
        u_int8_t pageIndex = 0;

        // max abount of pages to produce
        uint16_t pageLimit = 0;
        uint16_t linesPerPage = 0;
};


#endif