#ifndef DROPDOWNUTILS_H
#define DROPDOWNUTILS_H

#include <Arduino.h>


/**
 * @brief Generates a sequence of numbers as a string, separated by newline characters.
 * 
 * This function generates a sequence of integers from `start` to `end` and stores them
 * as a newline-separated string in the provided `result` buffer. If the buffer is too 
 * small to hold the entire sequence, an empty string is returned.
 * 
 * @param start The starting integer of the sequence (inclusive).
 * @param end The ending integer of the sequence (inclusive).
 * @param result A pointer to the buffer where the resulting string will be stored.
 * @param length The size of the `result` buffer.
 * 
 * @note If `start > end` or `length` is 0, the function returns an empty string.
 * 
 * @details 
 * - The function clears the `result` buffer before processing.
 * - If the buffer is not large enough to hold the entire sequence, including separators 
 *   and the null-terminator, the function writes an empty string to `result`.
 * - The sequence of numbers is formatted with each number on a new line, except the last 
 *   number, which has no trailing newline.
 * 
 * Example Usage:
 * @code
 * char buffer[50];
 * generateStringSeq(1, 5, buffer, sizeof(buffer));
 * // buffer contains:
 * // "1\n2\n3\n4\n5"
 * 
 * generateStringSeq(10, 15, buffer, 10); // Insufficient buffer size
 * // buffer contains: ""
 * @endcode
 */
void generateStringSeq(uint16_t start, uint16_t end, char* result, size_t length);

/**
 * @brief Searches for a specific number in a newline-separated string and returns its index.
 * 
 * This function looks for the given `number` in a string where numbers are separated by newline 
 * characters (`\n`). It returns the zero-based index of the number if found, or `-1` if the number 
 * is not present or the input string is null.
 * 
 * @param inputString A pointer to the input string containing newline-separated numbers.
 * @param number The number to search for in the string.
 * @return int16_t The zero-based index of the `number` in the string if found, or `-1` if not found 
 *                 or if `inputString` is null.
 * 
 * @note
 * - The function assumes the numbers in the string are formatted as non-negative integers.
 * - It performs boundary checks to ensure matches correspond to whole numbers (not substrings).
 * - If `inputString` is null, the function returns `-1`.
 * 
 * @details 
 * - The function converts the `number` to its string representation and compares it with each number 
 *   in the input string.
 * - Numbers in the string are separated by newline characters, and the function ensures that matches 
 *   occur only at full boundaries (i.e., the next character must be a newline or null terminator).
 * - If the `number` is not found, `-1` is returned.
 * 
 * Example Usage:
 * @code
 * const char *data = "10\n20\n30\n40";
 * int16_t index = findValueInString(data, 30); // Returns 2 (index of 30)
 * 
 * index = findValueInString(data, 50); // Returns -1 (50 is not in the string)
 * 
 * index = findValueInString(NULL, 10); // Returns -1 (null string)
 * @endcode
 */
int16_t findValueInString(const char *inputString, uint16_t number);


#endif