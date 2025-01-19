#include "DropDownUtils.h"

// Options generator
void generateStringSeq(uint16_t start, uint16_t end, char *result, size_t length)
{
    // Clean the buffer
    memset(result, 0, length);

    // Check if input is valid
    if (start > end || length == 0)
    {
        if (length > 0)
        {
            result[0] = '\0'; // Return empty string for invalid inputs
        }
        return;
    }

    size_t pos = 0; // Current position in the result array
    for (uint16_t i = start; i <= end; i++)
    {
        // Convert the number to a string and calculate the length
        char buffer[5]; // Buffer to store the string representation of the number
        int numLength = snprintf(buffer, sizeof(buffer), "%d", i);

        // Check if there's enough space in the result array for the number and delimiter
        if (pos + numLength + ((i < end) ? 1 : 0) >= length)
        {
            result[0] = '\0'; // Not enough space, return empty string
            return;
        }

        // Append the number to the result
        strncpy(result + pos, buffer, numLength);
        pos += numLength;

        // Append the newline character except for the last number
        if (i < end)
        {
            result[pos] = '\n';
            pos++;
        }
    }

    // Null-terminate the string
    result[pos] = '\0';
}

int16_t findValueInString(const char *inputString, uint16_t number)
{
    if (!inputString)
    {
        return -1; // Handle null pointer case
    }

    char buffer[6];                                 // Temporary buffer to hold the string representation of the number
    snprintf(buffer, sizeof(buffer), "%u", number); // Convert the number to a string

    const char *pos = inputString; // Pointer to traverse the input string
    int16_t index = 0;             // Index counter for the numbers in the string

    while (*pos != '\0')
    {
        // Compare the number at the current position
        if (strncmp(pos, buffer, strlen(buffer)) == 0)
        {
            // Ensure it matches fully (check for number boundaries)
            char nextChar = *(pos + strlen(buffer));
            if (nextChar == '\n' || nextChar == '\0')
            {
                return index;
            }
        }

        // Move to the next number by finding the next newline or end of string
        while (*pos != '\n' && *pos != '\0')
        {
            pos++;
        }
        if (*pos == '\n')
        {
            pos++; // Skip newline if found
        }
        index++;
    }

    return -1; // Return -1 if the number is not found
}
