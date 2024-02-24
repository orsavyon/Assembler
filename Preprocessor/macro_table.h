#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H
#include "utils.h"

/**
 * @struct Macro
 * Represents a macro object.
 */
 
typedef struct {
    char name[MAX_LINE_LENGTH]; /** The name of the macro. */
    char **lines;               /** An array of strings representing the macro's content. */
    int num_of_lines;           /** The number of lines in the macro. */
    struct macro *next_macro;  /** Pointer to the next macro in the macro table (linked list). */
} macro;

#endif MACRO_TABLE_H 
