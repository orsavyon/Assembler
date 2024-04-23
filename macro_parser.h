#ifndef MACRO_PARSER_H
#define MACRO_PARSER_H

typedef struct Macro
{
  char *name;
  char *content;
  struct Macro *next;
} Macro;

#define MACRO_TABLE_SIZE 100
#define MAX_LINE 1024

extern Macro *macroTable[MACRO_TABLE_SIZE];

/* macro parser: first parse of file for macro */
void macroParser(FILE *, char *);

/* insert macro: insert macro to file */

void insertMacroToTable(FILE *, char *);

/* hash function for macro names */
unsigned int hashMacroName(char *);

/* initialize macro table */
void initMacroTable();

/* macro table lookup */
struct Macro *lookup(char *);

/* add macro to table */
void addMacro(char *, char *);

/* print macro table */
void printMacroTable(Macro **);

#endif
