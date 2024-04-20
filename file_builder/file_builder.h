#ifndef FILE_BUILDER_H
#define FILE_BUILDER_H

#include "../assembler/assembler.h" /*assume*/

#define DOT_ENT_SUFFIX ".ent"
#define DOT_EXT_SUFFIX ".ext"
#define DOT_OB_SUFFIX ".ob"

/**
 * @brief Get entry_table and build '.ent' file with all the entries defenitions' lines.
 * 
 * @param entry_table Table with all the entries defenitions' lines.
 * @param entry_filename The am_filename without the '.am' suffix.
 */
void build_entry_file(char * entry_filename, Symbol * entry_table);

/**
  @brief Get the IC, DC, Memory address and build an '.ob' file from them.
  
  @param IC Instructions count.
  @param DC Data count.
  @param memory_address Address of first memory word.
 */
void build_ob_file(int IC, int DC, int* memory_address);

#endif /* FILE_BUILDER_H */