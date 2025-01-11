#include "TextIndex.h"
#include <esp_log.h>
#include <LogTags.h>

const TextIndex::Conf TextIndex::Conf::DFT = {432, 704, &lv_font_montserrat_14, 0, false};

TextIndex::TextIndex(FileManager &fileManager) : fm(fileManager)
{
}

String TextIndex::index(const char *path)
{
	// Drop values to zero
	pageIndex = 0;
	lineIndex = 0;

	// Set status
	currentStatus = STATUS_CHECKSUM;
	currentStatusVaue = 0;

    // Open file 
	File file = fm.openFile(path, FILE_READ);
	if (!file) {
		ESP_LOGE(TAG_INDEX, "Unable to open file %s", path);
        return emptyString;
	}

	ESP_LOGV(TAG_INDEX, "File size: %i", file.size());	

	AdlerChecksum checksumImpl(fm);
	String checksum = checksumImpl.checksum(path, 5120, &currentStatusVaue);

    // Create temp dir name
    const char* parentDir = getParentDir(file.path());
	ESP_LOGD(TAG_INDEX, "Dir Name: %s", parentDir);

	String filename = String(file.name());
	filename.toLowerCase();

	String idxDirName = "._" + filename + "_" + checksum + "_idx";
	String idxDirPath = String(parentDir);
	if (idxDirPath.length() > 0 && idxDirPath[idxDirPath.length() - 1] != '/') {
		idxDirPath.concat("/");
	}
	idxDirPath.concat(idxDirName);

	ESP_LOGD(TAG_INDEX, "Dir Name: %s", parentDir);

	const char* idxDirPathCharArr = idxDirPath.c_str();

    // If direectory exists, consider index complete and return
	bool dirExists = fm.exists(idxDirPathCharArr);
	if (dirExists) {
		if (forceIndex) {
			// Set status
			currentStatus = STATUS_CLEANUP;
			currentStatusVaue = 0;
			fm.removeDirRecursive(idxDirPathCharArr, reinterpret_cast<uint16_t*>(&currentStatusVaue));
		} else { 
			ESP_LOGD(TAG_INDEX, "Index for %s already exists, returning dir: %s", filename, idxDirPathCharArr);
			return idxDirPath;
		}
	}

	fm.createDir(idxDirPathCharArr);
	ESP_LOGD(TAG_INDEX, "Idx directory path: %s", idxDirPathCharArr);

	// Set status
	currentStatus = STATUS_INDEXING;
	currentStatusVaue = 0;

	// Reserve values for strings
	currentPage.reserve(5120);
	currentLine.reserve(512);
	currentWord.reserve(128);

    // ---------------- Start indexing ----------------
    while (file.available())
	{
		c = file.read(); // Read the file character by character

		// If we hit a newline or space, treat it as a word boundary
		if (c == ' ' || c == '\n')
		{
			// Drop values in case word or line are empty
			wordWidth = linewidth = height = 0;

			wordWidth = lv_txt_get_width(currentWord.c_str(), currentWord.length(), lvFont, 1, lvTextFlag);
			linewidth = lv_txt_get_width(currentLine.c_str(), currentLine.length(), lvFont, 1, lvTextFlag);
		
			// Check if the current word fits on the line
			if (linewidth + spaceWidth + wordWidth >= textAreaWidth)
			{
				// If the word doesn't fit, print the current line and start a new one
				currentPage.concat(currentLine);
				currentPage.concat('\n');
				currentLine.clear();	  // Reset current line
				
				skipLeadingSpaces = true; // Skip leading spaces for the next line
				wrappedLine       = true; // Mark that the line was wrapped
				spaceAfterWrap    = true;

				lineIndex++;
			}

			// Add the word to the current line, but only if it's not an empty line
			if (!skipLeadingSpaces || currentWord.length() > 0)
			{
				currentLine += currentWord;

				if (wrappedLine && spaceAfterWrap) {
					currentLine += ' ';
					spaceAfterWrap = false;
				}
			}

			// Add space if it's not the start of a new line
			if (c == ' ')
			{
				// Avoid more then one space in row
				if (!skipLeadingSpaces && (currentLine.length() > 0 && currentLine.charAt(currentLine.length() - 1) != ' '))
				{
					currentLine += ' ';
				}
			}

			// If there's a newline, print the current line and move to the next line
			if (c == '\n')
			{
				if (!wrappedLine)  // Only print the line if it wasn't already wrapped
				{							 
					currentPage.concat(currentLine); // Display the current line
					currentPage.concat('\n');
					lineIndex++;

					currentLine.clear();	  // Reset the line
					skipLeadingSpaces = true; // Skip leading spaces for the next line
				} else {
					// Add space instead of '\n' when line was wrapped
					if (currentLine.length() > 0 && currentLine.charAt(currentLine.length() - 1) != ' ') {
						currentLine += ' ';
						// currentLineWidth += spaceWidth;
					}
				}

				wrappedLine = false;
			}

			// Clear the current word
			currentWord.clear();
		}
		
		else if (c != ' ' && isPrintable(c))
		{
			// If it's a normal character, build the current word
			currentWord += c;
			skipLeadingSpaces = false; // We've found non-space content, so we stop skipping spaces
		}

		if (lineIndex >= linesPerPage) {
			// Creating page file on SD card
			String pageFileName = idxDirPath + "/._" + String(pageIndex) + ".page.txt";
			bool pageFileWritten = fm.writeFile(pageFileName.c_str(), currentPage.c_str());
            if (!pageFileWritten) {
                ESP_LOGE(TAG_INDEX, "Error creating page file");
            }
			
			// Drop current page, so next is processed
			currentPage.clear();
			pageIndex++;
			lineIndex = currentLine.isEmpty() ? 0 : 1;
		}

		// Only check for page limit in case it has value different than
		if (pageLimit != 0 && pageIndex > pageLimit) {
            ESP_LOGD(TAG_INDEX, "Page limit of %i exceeded", pageLimit);
			break;
		}
	}

	if (!currentWord.isEmpty()) {
		currentLine.concat(currentWord);
		currentWord.clear();
	}

    if (!currentLine.isEmpty())
	{
		currentLine.trim();
		currentPage.concat(currentLine);
		currentLine.clear();
	}

	if (!currentPage.isEmpty()) {
		String pageFileName = idxDirPath + "/._" + String(pageIndex) + ".page.txt";
		bool pageFileWritten = fm.writeFile(pageFileName.c_str(), currentPage.c_str());
		if (!pageFileWritten) {
			ESP_LOGE(TAG_INDEX, "Error creating page file");
		}

		currentPage.clear();
	}

    // Close original file
    file.close();

	// Set status and clear status bits
	currentStatus = STATUS_IDLE;
	currentStatusVaue = 0;
	pageIndex = 0;

    return idxDirPath;
}

TextIndex::StatusValue TextIndex::status() const
{
	size_t returnValue;
	switch (currentStatus) 
	{
	case TextIndex::STATUS_IDLE:
	case TextIndex::STATUS_CHECKSUM:
	case TextIndex::STATUS_CLEANUP:
		returnValue = currentStatusVaue;
		break;
	case TextIndex::STATUS_INDEXING:
		returnValue = pageIndex;
		break;
	default:
		returnValue = 0;
		break;
	}

    return {currentStatus, returnValue};
}

void TextIndex::configure(const TextIndex::Conf& conf)
{
    textAreaWidth  = conf.textWidth;
    textAreaHeight = conf.textHeight;
    pageLimit      = conf.pageLimit;
	forceIndex     = conf.forceIndex;
	lvFont         = conf.font;

	lv_point_t sizeRes;

    // Get Space width
	lv_txt_get_size(&sizeRes, "s w", lvFont, 1, 1, 480, lvTextFlag);
	spaceWidth = sizeRes.x;
	lv_txt_get_size(&sizeRes, "sw", lvFont, 1, 1, 480, lvTextFlag);
    spaceWidth = spaceWidth - sizeRes.x;
    ESP_LOGV(TAG_INDEX, "Calculated space width: %i", spaceWidth);

    // Calculate lines per page. Find fine one line height 
	lv_txt_get_size(&sizeRes, "LINE\n\nLINE", lvFont, 1, 1, 480, lvTextFlag);
	uint8_t lineHeight = sizeRes.y / 3;
	linesPerPage  = textAreaHeight / lineHeight; 
	ESP_LOGV(TAG_INDEX, "Lines per page: %i, line height: %i\n", linesPerPage, lineHeight);
}
