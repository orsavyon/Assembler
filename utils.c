#include <stddef.h>
#include "utils.h"

/*
 * Removes all white spaces from the source_line and saves the result in dest_line.
 *
 * @param source_line The input string with white spaces.
 * @param dest_line The output string without white spaces.
 */
void remove_white_spaces(char *source_line, char *dest_line)
{
    while (*source_line != '\0')
    {
        if (*source_line != ' ' && *source_line != '\t' && *source_line != '\n')
        {
            *dest_line++ = *source_line;
        }
        source_line++;
    }
    *dest_line = '\0'; /* Null-terminate the destination string */
}

/*
 * Removes the prefix white spaces from the source_line and saves the result in dest_line.
 *
 * @param source_line The input string with prefix white spaces.
 * @param dest_line The output string without prefix white spaces.
 */
void remove_prefix_white_spaces(char *source_line, char *dest_line)
{
    /* Skip the leading white spaces */
    while (*source_line == ' ' || *source_line == '\t' || *source_line == '\n')
    {
        source_line++;
    }

    /* Copy the rest of the string */
    while (*source_line != '\0')
    {
        *dest_line++ = *source_line++;
    }
    *dest_line = '\0'; /* Null-terminate the destination string */
}

/*
 * Removes the macro indentation from the sourceline and saves the result in dest_line.
 * The macro indentation is assumed to start at index 4 of the sourceline.
 *
 * @param sourceline The input string with macro indentation.
 * @param dest_line The output string without macro indentation.
 * @note The assumption is that macro content is written in indentation of 4 chars.
 */
void remove_macro_indentation(char *sourceline, char *dest_line)
{
    int i;
    /* Skip the first 4 characters (assuming they are the macro indentation) */
    for (i = 0; i < 4 && *sourceline != '\0'; i++)
    {
        sourceline++;
    }

    /* Copy the rest of the string */
    while (*sourceline != '\0')
    {
        *dest_line++ = *sourceline++;
    }
    *dest_line = '\0'; /* Null-terminate the destination string */
}
