#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <Arduino.h>

/**
 * Utility method that would accept directry patch and return one level up directory
 * Works without dynamic memory allocation
 * Example:
 *
 * "/books" → "/"
 * "/books/current" → "/books"
 * "/books/current/" → "/books"
 * "/" → "/"
 *
 * @param directory path
 * @return prent level directory or "/" in case FS root
 *
 */
const char *getParentDir(const char *path);

/**
 * Fild file extention
 *
 * @param filename pointer to the string where extention needs to be found
 * @returns pointer to the same string but 1 char after last dot ext or nullptr
 */
const char *getFileExtension(const char *filename);

#endif
