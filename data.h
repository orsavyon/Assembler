#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LABEL_LENGTH 31
#define MAX_SYMBOLS 100
#define MAX_DATA 1000
#define MAX_LABELS 100
#define MAX_COMMANDS 100
#define MAX_LINE_LENGTH 81
#define CMD_NUM 16
#define MAX_OPERANDS 2
#define MAX_SYMBOLS 100

/* variable helpers */
extern int IC;
extern int DC;
extern int symbolCount;
extern int dataCount;
extern int labelCount;
extern int commandCount;
extern int labelFlag;
extern int dataFlag;
extern int commandFlag;
extern int symbolFlag;
extern int entryFlag;
extern int externFlag;
extern int lineNum;
extern int errorFlag;

/* saved words */
extern char *savedWords[];

/* command definitions */
typedef struct Command
{
    const char *cmdName;
    int opCode;
    int numOfOps;

} Command;

extern Command commandTable[CMD_NUM];

/* word definition */
typedef struct Word
{
    unsigned int ARE : 2;
    unsigned int desO : 2;
    unsigned int srcO : 2;
    unsigned int opcode : 4;
    unsigned int na : 4;
} Word;

/* symbol definition */
typedef enum
{
    DATA,
    CODE,
    EXTERN,
    ENTRY,
    MDEFINE
} SymbolType;
typedef struct Symbol
{
    const char *symbolName;
    SymbolType symbolType;
    unsigned int value;
    struct Symbol *next;
} Symbol;

enum symlDef
{
    mdefine,
    code
};

extern Symbol *symbolTable[MAX_SYMBOLS];

/* directives */
typedef enum
{
    DATA_DIRECTIVE,
    STRING_DIRECTIVE,
    ENTRY_DIRECTIVE,
    EXTERN_DIRECTIVE
} DirectiveType;

/* functions */

void initData();

void insertSymbolToTable(const char *symbolName, SymbolType symbolType, unsigned int value);
unsigned int hashSymbolName(const char *name);
void initSymbolTable();
struct Symbol *lookupSymbol(const char *name);
void addSymbol(const char *name, SymbolType type, unsigned int value);
void printSymbolTable();
#endif