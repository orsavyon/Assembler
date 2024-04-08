#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"
#include "data.h"

/**
 * @brief Creates a duplicate of the given string.
 *
 * Allocates memory for the new string and copies the content from the source string.
 *
 * @param s The string to duplicate.
 * @return A pointer to the newly allocated string, or NULL if allocation fails.
 */
char *strdup(char *s)
{
    char *p;
    p = (char *)malloc(strlen(s) + 1); /* +1 for the null terminator */
    if (p != NULL)
    {
        strcpy(p, s);
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
    fprintf(stderr, "Error in line %d: %s\n\t%s\n", lineNumber, errorMessage, line);
}
