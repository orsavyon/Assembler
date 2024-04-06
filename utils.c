#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "data.h"

/* make a duplicate of s */
char *strdup(char *s)
{
    char *p;
    p = (char *)malloc(strlen(s) + 1); /* +1 for '\0' */
    if (p != NULL)
    {
        strcpy(p, s);
    }
    return p;
}

/* fileCopy: copy file ifp to file ofp */
void fileCopy(FILE *ifp, FILE *ofp)
{
    int c;
    while ((c = getc(ifp)) != EOF)
        putc(c, ofp);
}

/* printFile: print file content */

void printFile(FILE *fp)
{
    int c;
    rewind(fp);
    while ((c = getc(fp)) != EOF)
    {
        putc(c, stdout);
    }
}

/* skipAndCopy:*/

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

/* trimLine: trim white characters from both ends of line */

void trimLine(char *line)
{
    int i, j;
    for (i = 0; line[i] == ' ' || line[i] == '\t'; i++)
        ;
    for (j = 0; line[i] != '\0'; i++)
    {
        line[j++] = line[i];
    }
    for (i = j - 1; i >= 0 && (line[i] == ' ' || line[i] == '\t'); i--)
    {
        line[i] = '\0';
    }
}

/* skipWhiteLines: skip white lines in file */

void skipWhiteLines(FILE *fp)
{
    int c;
    while ((c = fgetc(fp)) == '\n' || c == '\t' || c == ' ')
        ;
    ungetc(c, fp);
}

/* handleError: print error message and line */

void handleError(const char *errorMessage, int lineNumber, char *line)
{
    errorFlag = 1;
    fprintf(stderr, "Error in line %d: %s\n\t%s\n", lineNumber, errorMessage, line);
}