#ifndef FILE_BUILDER_H
#define FILE_BUILDER_H

#include "data.h"

#define DOT_ENT_SUFFIX ".ent"
#define DOT_EXT_SUFFIX ".ext"
#define DOT_OB_SUFFIX ".ob"
#define MAX_FILE_NAME_LENGTH 200
#define MACRO_DEF_STR_LENGTH 4
#define MAX_LINE_LENGTH 81
#define FOUR_CHARS_INDENTATION 4
#define COMMENT_PREFIX ';'
#define BINARY_DIGITS 14
#define BASE_4_DIGITS 7

/**
  @brief Get Memory address and build an '.ob' file from them.

  @param ob_filename The name of the '.ob' file.
  @param memory_address Address of first memory word.
 */
void createObFile(char *, unsigned int memory_address[]);

/**
 * @brief Get the entry symbols and their addresses and build an '.ent' file from them.
 *
 * @param ent_filename The name of the '.ent' file.
 */
void createEntryFile(char *ent_filename);

/**
 * @brief Get the external symbols and their addresses and build an '.ext' file from them.
 *
 * @param ext_filename The name of the '.ext' file.

*/
void createExtFile(char *ext_filename);

/**
 * @brief Transform an decimal array to base4 array.
 *
 * @param decimal The decimal array.
 */
void decimalToBase4(int decimal, int base4[]);

/**
 * @brief Transform an base4 array to encoded strings array.
 *
 * @param base4 The base4 array.
 */
char *base4ToEncoded(const int base4[]);

#endif /* FILE_BUILDER_H */