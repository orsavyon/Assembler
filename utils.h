#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

/**
 * @brief Converts an integer to its binary string representation.
 *
 * Allocates memory for a 16-bit binary string and converts the integer value to binary.
 * Each bit of the integer is converted to '0' or '1' and placed into the string from right to left.
 *
 * @param value The integer value to convert.
 * @return A pointer to the binary string representation of the integer, or NULL if memory allocation fails.
 */
char *intToBinary(int value);

/**
 * @brief Converts an integer to a 14-bit integer by applying a mask.
 *
 * This function masks the integer with 0x3FFF to ensure that only the lower 14 bits are retained.
 * This is useful for operations that require values to be limited to 14 bits.
 *
 * @param value The integer to be converted to 14-bit.
 * @return The 14-bit masked integer.
 */

int intTo14Bit(int value);

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

/**
 * @brief Checks if the string represents a numeric value.
 *
 * This function iterates through each character of the string to determine if it is a digit,
 * or a leading '+' or '-' sign, which are valid in numeric strings. If any character outside
 * these categories is found, the function returns 0, indicating the string is not numeric.
 *
 * @param str The string to be checked.
 * @return 1 if the string is numeric, 0 otherwise.
 */
int isNumeric(const char *str);

/**
 * Compares two strings for equality, ignoring the case of characters.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @return Returns 0 if both strings are equal (case-insensitive), a negative value if s1 is less than s2, and a positive value if s1 is greater than s2.
 */
int strCaseCmp(const char *s1, const char *s2);

/**
 * @brief Checks if a character is a valid character for a label.
 *
 * @param c The character to check.
 * @return 1 if the character is valid, 0 otherwise.
 */
int isLegalCharacter(char c);

void cutOffExtension(char *filename);

#endif /* UTILS_H */