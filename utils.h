#ifndef UTILS
#define UTILS

#include <stdio.h>
#define MAX_FILE_NAME_LENGTH 200
#define MACRO_DEF_STR_LENGTH 3
#define MAX_LINE_LENGTH 81
#define FOUR_CHARS_INDENTATION 4
#define COMMENT_PREFIX ';'

/**
 * @brief Removes all white spaces from the source_line and saves the result in dest_line.
 *
 * @param source_line The input string with white spaces.
 * @param dest_line The output string without white spaces.
 */
void remove_white_spaces(char *source_line, char *dest_line);

/**
 * @brief Removes the prefix white spaces from the source_line and saves the result in dest_line.
 *
 * @param source_line The input string with prefix white spaces.
 * @param dest_line The output string without prefix white spaces.
 */
void remove_prefix_white_spaces(char *source_line, char *dest_line);

/**
 * @brief Removes the macro indentation from the sourceline and saves the result in dest_line.
 * The macro indentation is assumed to start at index 4 of sourceline.
 *
 * @param sourceline The input string with macro indentation.
 * @param dest_line The output string without macro indentation.
 * @note The assumption is that macro content is written in indentation of 4 chars.
 */

void remove_macro_indentation(char *sourceline, char *dest_line);

#endif