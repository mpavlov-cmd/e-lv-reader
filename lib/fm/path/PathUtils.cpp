#include "PathUtils.h"

const char *getParentDir(const char *path)
{
     // Check if the input path is root "/"
    if (strcmp(path, "/") == 0)
    {
        return "/";
    }

    // Determine the length of the path
    size_t length = strlen(path);

    // Remove trailing slash if present (but ensure root "/" is unchanged)
    if (length > 1 && path[length - 1] == '/')
    {
        length--;
    }

    // Find the last slash in the adjusted path
    for (size_t i = length; i > 0; --i)
    {
        if (path[i - 1] == '/')
        {
            // If the last slash is at the beginning, return root
            if (i == 1)
            {
                return "/";
            }

            // Allocate a new string for the previous level
            // When static variable allocation is used, the space will be reserverd in static mameory once and forever. 
            // Thus, no need to do a cleanup
            static char previousLevel[256]; // Adjust size as needed for your environment
            strncpy(previousLevel, path, i - 1);
            previousLevel[i - 1] = '\0';

            return previousLevel;
        }
    }

    // If no slash is found, return root (this shouldn't occur with valid input)
    return "/";
}

const char *getFileExtension(const char *filename)
{
    const char *dot = "";

    // Traverse the string to find the last dot '.'
    for (int i = 0; filename[i] != '\0'; i++) {
        if (filename[i] == '.') {
            dot = &filename[i + 1];  // Point to the character after the dot
        }
    }

    // Check if an extension was found
    return dot;
}
