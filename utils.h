#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

/**
 * @brief Duplicates a string by allocating memory for the new string and copying the content.
 *
 * @param str The string to duplicate.
 * @return A pointer to the new string, which is a duplicate of the original.
 */
char *strdup(char *str);

/**
 * @brief Copies the content of one file to another.
 *
 * @param src The source file to copy from.
 * @param dest The destination file to copy to.
 */
void fileCopy(FILE *src, FILE *dest);

/**
 * @brief Prints the content of a file to the standard output.
 *
 * @param fp The file to print.
 */
void printFile(FILE *fp);

/**
 * @brief Skips white lines and copies the content from the source file to the destination file.
 *
 * @param src The source file to read from.
 * @param dest The destination file to write to.
 */
void skipAndCopy(FILE *src, FILE *dest);

/**
 * @brief Skips white lines in the file.
 *
 * @param fp The file pointer to operate on.
 */
void skipWhiteLines(FILE *fp);

/**
 * @brief Handles errors by printing an error message along with the line number and the line where the error occurred.
 *
 * @param errorMessage The error message to be printed.
 * @param lineNumber The line number where the error occurred.
 * @param line The line of code that caused the error.
 */
void handleError(const char *errorMessage, int lineNumber, char *line);

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * @param line The string to be trimmed.
 */
void trimLine(char *line);

#endif /* UTILS_H */
