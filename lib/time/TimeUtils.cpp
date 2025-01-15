#include "TimeUtils.h"

void formatTime(uint8_t hour, uint8_t minute, uint8_t second, const char *pattern, char *output)
{
    const char *p = pattern; // Pointer to traverse the pattern
    char *out = output;      // Pointer to construct the output string

    while (*p != '\0')
    {
        if (*p == 'H')
        {
            if (*(p + 1) == 'H')
            {   // Handle HH
                if (hour < 10)
                    *out++ = '0';
                itoa(hour, out, 10); // Convert hour to string
                out += strlen(out);  // Move output pointer
                p += 2;              // Skip HH in the pattern
            }
            else
            {   // Handle H
                itoa(hour, out, 10);
                out += strlen(out);
                p++;
            }
        }
        else if (*p == 'M')
        {
            if (*(p + 1) == 'M')
            {   // Handle MM
                if (minute < 10)
                    *out++ = '0';
                itoa(minute, out, 10); // Convert minute to string
                out += strlen(out);
                p += 2; // Skip MM in the pattern
            }
            else
            {   // Handle M
                itoa(minute, out, 10);
                out += strlen(out);
                p++;
            }
        }
        else if (*p == 'S')
        {
            if (*(p + 1) == 'S')
            {   // Handle SS
                if (second < 10)
                    *out++ = '0';
                itoa(second, out, 10); // Convert second to string
                out += strlen(out);
                p += 2; // Skip SS in the pattern
            }
            else
            {   // Handle S
                itoa(second, out, 10);
                out += strlen(out);
                p++;
            }
        }
        else
        {
            *out++ = *p++; // Copy other characters directly
        }
    }
    *out = '\0'; // Null-terminate the output string
}