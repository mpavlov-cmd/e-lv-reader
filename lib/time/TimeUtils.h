#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <Arduino.h>
/**
 * @brief Formats the given time values into a string based on the provided pattern.
 *
 * This function takes hour, minute, and second values and formats them into a string 
 * according to the specified pattern. The output is written into the pre-allocated 
 * buffer provided by the caller. The pattern supports the following placeholders:
 *
 * - `H`  : Hour (1-23, no leading zero)
 * - `HH` : Hour (01-23, with leading zero)
 * - `M`  : Minute (0-59, no leading zero)
 * - `MM` : Minute (00-59, with leading zero)
 * - `S`  : Second (0-59, no leading zero)
 * - `SS` : Second (00-59, with leading zero)
 *
 * Any other characters in the pattern are copied directly to the output string.
 * 
 * **Example Patterns:**
 * - `"HH:MM:SS"` → `05:03:09`
 * - `"H:M:S"` → `5:3:9`
 * - `"H hour, MM minutes"` → `5 hour, 03 minutes`
 *
 * @param hour    The hour value (0-23).
 * @param minute  The minute value (0-59).
 * @param second  The second value (0-59).
 * @param pattern The format pattern string containing placeholders for time components.
 * @param output  A pre-allocated buffer to store the formatted string. The caller must
 *                ensure that the buffer is large enough to hold the result, including
 *                the null terminator.
 *
 * @return None. The formatted string is written to the `output` buffer.
 */
void formatTime(uint8_t hour, uint8_t minute, uint8_t second, const char* pattern, char* output);

#endif