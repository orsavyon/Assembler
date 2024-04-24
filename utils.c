#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"
#include "data.h"

/**
 * @brief Duplicates a string by allocating memory for the new string and copying the content.
 *
 * @param str The string to duplicate.
 * @return A pointer to the new string, which is a duplicate of the original.
 */
char *strdup(char *str)
{
    /* Attempt to allocate memory for the duplicate string */
    char *p;
    p = (char *)malloc(strlen(str) + 1); /* +1 for the null terminator */
    /* Return NULL if the original string is NULL */
    if (str == NULL)
    {
        return NULL;
    }
    /* Copy the original string if memory allocation was successful */
    if (p != NULL)
    {
        strcpy(p, str);
    }
    return p;
}

/**
 * @brief Copies the content from one file to another.
 *
 * @param ifp Pointer to the input file.
 * @param ofp Pointer to the output file.
 */
void fileCopy(FILE *ifp, FILE *ofp)
{
    int c;
    while ((c = getc(ifp)) != EOF)
        putc(c, ofp);
}

/**
 * @brief Prints the content of a file to standard output.
 *
 * @param fp Pointer to the file to be printed.
 */
void printFile(FILE *fp)
{
    int c;
    rewind(fp);
    while ((c = getc(fp)) != EOF)
    {
        putc(c, stdout);
    }
}

/**
 * @brief Skips comments and copies the remaining content from the source file to the destination file.
 *
 * Comments are lines starting with ';' and are not copied to the destination.
 *
 * @param source Pointer to the source file.
 * @param dest Pointer to the destination file.
 */
void skipAndCopy(FILE *source, FILE *dest)
{
    int c, inComment = 0;

    while ((c = fgetc(source)) != EOF)
    {
        if (inComment)
        {
            if (c == '\n')
            {
                inComment = 0;
                fputc(c, dest);
            }
        }
        else
        {
            if (c == ';')
            {
                inComment = 1;
            }
            else
            {
                fputc(c, dest);
            }
        }
    }
}

/**
 * @brief Trims whitespace characters from both ends of a string.
 *
 * It adjusts the provided string in place, removing all leading and trailing whitespace
 * characters, including spaces, tabs, and newlines.
 *
 * @param line The string to be trimmed. This string is modified in place.
 */
void trimLine(char *line)
{
    char *start = line; /* Pointer to the start of the string */
    char *end;
    /* Skip all leading whitespace characters */
    while (isspace((unsigned char)*start))
    {
        start++;
    }
    /* Move the non-whitespace part to the beginning of the original string if needed */
    if (start != line)
    {
        memmove(line, start, strlen(start) + 1); /* +1 to include the null terminator */
    }
    /* Find the end of the string after leading whitespace is removed */
    end = line + strlen(line) - 1;

    /* Remove trailing whitespace characters, including possible newline characters */
    while (end >= line && (isspace((unsigned char)*end) || *end == '\n'))
    {
        end--;
    }

    /* Null terminate the string after the last non-whitespace character */
    *(end + 1) = '\0';
}

/**
 * @brief Skips white lines (lines with only space, tab, or newline characters) in the file.
 *
 * @param fp Pointer to the file.
 */
void skipWhiteLines(FILE *fp)
{
    int c;
    while ((c = fgetc(fp)) == '\n' || c == '\t' || c == ' ')
        ;
    ungetc(c, fp); /*Put back the first non-white character*/
}

/**
 * @brief Handles and prints an error message along with the line number and the problematic line.
 *
 * @param errorMessage The error message to print.
 * @param lineNumber The line number where the error occurred.
 * @param line The actual line from the source file where the error occurred.
 */
void handleError(const char *errorMessage, int lineNumber, char *line)
{
    errorFlag = 1;
    fprintf(stderr, "ERROR >> Error in line %d: %s\n\t%s\n", lineNumber, errorMessage, line);
}

/**
 * @brief Converts an integer to its binary string representation.
 *
 * Allocates memory for a 16-bit binary string and converts the integer value to binary.
 * Each bit of the integer is converted to '0' or '1' and placed into the string from right to left.
 *
 * @param value The integer value to convert.
 * @return A pointer to the binary string representation of the integer, or NULL if memory allocation fails.
 */
char *intToBinary(int value)
{
    /* Allocate memory for a 17-character string to store 16 bits and a null terminator */
    char *binaryStr = malloc(17);
    int i;

    /* Check for memory allocation failure */
    if (binaryStr == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    /* Set the last character of the array to the null terminator */
    binaryStr[16] = '\0';

    /* Fill the string with binary digits from the least significant to the most significant bit */
    for (i = 15; i >= 0; i--)
    {
        binaryStr[i] = (value & 1) + '0'; /* Convert the least significant bit of value to '0' or '1' */
        value >>= 1;                      /* Shift the value right by one bit to process the next bit */
    }

    return binaryStr;
}

/**
 * @brief Converts an integer to a 14-bit integer by applying a mask.
 *
 * This function masks the integer with 0x3FFF to ensure that only the lower 14 bits are retained.
 * This is useful for operations that require values to be limited to 14 bits.
 *
 * @param value The integer to be converted to 14-bit.
 * @return The 14-bit masked integer.
 */
int intTo14Bit(int value)
{
    return value & 0x3FFF; /* Apply a 14-bit mask to the value */
}

/**
 * @brief Checks if the string represents a numeric value.
 *
 * This function checks if the first character is a digit, or a leading '+' or '-'. Subsequent characters must all be digits.
 * If any character outside these categories is found, the function returns 0, indicating the string is not numeric.
 *
 * @param str The string to be checked.
 * @return 1 if the string is numeric, 0 otherwise.
 */
int isNumeric(const char *str)
{
    /* Check if the initial character is a sign */
    if (*str == '+' || *str == '-')
        str++; /* Move to the next character */

    /* Check each character to see if it is a digit */
    if (!*str) /* Check for an empty string after the sign */
        return 0;

    while (*str)
    {
        if (!isdigit(*str++)) /* Check for non-digit characters */
            return 0;
    }
    return 1; /* All characters are valid digits, return true */
}

/**
 * Compares two strings for equality, ignoring the case of characters.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @return Returns 0 if both strings are equal (case-insensitive), a negative value if s1 is less than s2, and a positive value if s1 is greater than s2.
 */
int strCaseCmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && tolower((unsigned char)*s1) == tolower((unsigned char)*s2))
    {
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

/* Function to check if a character is legal */
int isLegalCharacter(char c)
{
    /* Define legal characters: alphanumeric, space, and common punctuation */
    if (isalnum(c) || isspace(c) || strchr(",.!?;:'\"", c))
    {
        return 1; /* Legal */
    }
    return 0; /* Illegal */
}

void cutOffExtension(char *filename)
{
    char *dot = strrchr(filename, '.');
    if (dot)
    {
        *dot = '\0';
    }
}