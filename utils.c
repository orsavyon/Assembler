#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"
#include "data.h"

char *strdup(char *str)
{
    char *p;
    p = (char *)malloc(strlen(str) + 1); /* +1 for the null terminator */
    if (str == NULL)
    {
        return NULL;
    }

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
 * @param line The string to be trimmed.
 */
/**
 * @brief Trims whitespace characters from both ends of a string.
 *
 * @param line The string to be trimmed.
 *
 * @brief Trims whitespace characters from both ends of a string.
 *
 * @param line The string to be trimmed.
 */
void trimLine(char *line)
{
    char *start = line;
    char *end;

    while (isspace((unsigned char)*start))
    {
        start++;
    }

    if (start != line)
    {
        memmove(line, start, strlen(start) + 1);
    }

    end = line + strlen(line) - 1;
    while (end >= line && (isspace((unsigned char)*end) || *end == '\n'))
    {
        end--;
    }

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
    fprintf(stderr, "ERROR ____ > Error in line %d: %s\n\t%s\n", lineNumber, errorMessage, line);
}

char *intToBinary(int value)
{
    char *binaryStr = malloc(17);
    int i;
    if (binaryStr == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    binaryStr[16] = '\0';

    for (i = 15; i >= 0; i--)
    {
        binaryStr[i] = (value & 1) + '0';
        value >>= 1;
    }

    return binaryStr;
}

int intTo14Bit(int value)
{
    return value & 0x3FFF;
}

int isNumeric(const char *str)
{
    while (*str)
    {
        if (!isdigit(*str++) && *str != '+' && *str != '-')
            return 0;
    }
    return 1;
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