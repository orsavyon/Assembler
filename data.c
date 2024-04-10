#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "utils.h"

/* Array of reserved words used in the program */
char *savedWords[] = {
    "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
    "jmp", "bne", "red", "prn", "jsr", "rts", "hlt",
    "data", "string", "entry", "extern", "define", "mcr", "endmcr"};

/* Array of register names in the CPU */
char *registers[] = {"r1", "r2", "r3", "r4", "r5", "r6", "r7"};
/* Table containing all commands supported by the assembler */
Command commandTable[16];

/* Table for holding translations during assembly process */
Translation *translationTable[MAX_DATA];

/* Initialize the translation table */
void initTranslationTable()
{
    int i;
    for (i = 0; i < MAX_DATA; i++)
    {
        translationTable[i] = NULL;
    }
}

/* Insert a translation into the translation table */
void addTranslationLine(int decimalAddress, Word *word)
{
    translationTable[decimalAddress] = (Translation *)malloc(sizeof(Translation));
    if (translationTable[decimalAddress] == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        return;
    }
    translationTable[decimalAddress]->word = word;
}

/* Global variables used in the assembly process */
int IC = 100;         /* Instruction counter */
int DC = 0;           /* Data counter */
int L = 0;            /* Line number (not used here) */
int symbolCount = 0;  /* Number of symbols */
int dataCount = 0;    /* Number of data entries */
int labelCount = 0;   /* Number of labels */
int commandCount = 0; /* Number of commands */
int labelFlag = 0;    /* Flag for label processing */
int dataFlag = 0;     /* Flag for data processing */
int commandFlag = 0;  /* Flag for command processing */
int symbolFlag = 0;   /* Flag for symbol processing */
int entryFlag = 0;    /* Flag for entry directive processing */
int externFlag = 0;   /* Flag for extern directive processing */
int lineNum = 0;      /* Current line number */
int errorFlag = 0;    /* Flag for error detection */
int memory[MAX_DATA]; /* Memory array for the assembler */

/**
 * Initializes the command table with predefined assembly commands.
 */
void initData()
{
    /* Initializing command table with command names, opcodes, and number of operands */

    commandTable[0].cmdName = "mov";
    commandTable[0].opCode = 0;
    commandTable[0].numOfOps = 2;

    commandTable[1].cmdName = "cmp";
    commandTable[1].opCode = 1;
    commandTable[1].numOfOps = 2;

    commandTable[2].cmdName = "add";
    commandTable[2].opCode = 2;
    commandTable[2].numOfOps = 2;

    commandTable[3].cmdName = "sub";
    commandTable[3].opCode = 3;
    commandTable[3].numOfOps = 2;

    commandTable[4].cmdName = "not";
    commandTable[4].opCode = 4;
    commandTable[4].numOfOps = 1;

    commandTable[5].cmdName = "clr";
    commandTable[5].opCode = 5;
    commandTable[5].numOfOps = 1;

    commandTable[6].cmdName = "lea";
    commandTable[6].opCode = 6;
    commandTable[6].numOfOps = 2;

    commandTable[7].cmdName = "inc";
    commandTable[7].opCode = 7;
    commandTable[7].numOfOps = 1;

    commandTable[8].cmdName = "dec";
    commandTable[8].opCode = 8;
    commandTable[8].numOfOps = 1;

    commandTable[9].cmdName = "jmp";
    commandTable[9].opCode = 9;
    commandTable[9].numOfOps = 1;

    commandTable[10].cmdName = "bne";
    commandTable[10].opCode = 10;
    commandTable[10].numOfOps = 1;

    commandTable[11].cmdName = "red";
    commandTable[11].opCode = 11;
    commandTable[11].numOfOps = 1;

    commandTable[12].cmdName = "prn";
    commandTable[12].opCode = 12;
    commandTable[12].numOfOps = 1;

    commandTable[13].cmdName = "jsr";
    commandTable[13].opCode = 13;
    commandTable[13].numOfOps = 1;

    commandTable[14].cmdName = "rts";
    commandTable[14].opCode = 14;
    commandTable[14].numOfOps = 0;

    commandTable[15].cmdName = "hlt";
    commandTable[15].opCode = 15;
    commandTable[15].numOfOps = 0;
}

/* Table containing all symbols found by the assembler */

Symbol *symbolTable[MAX_SYMBOLS];

/* Generate a hash value for a symbol name */
unsigned int hashSymbolName(const char *name)
{
    unsigned int hashVal = 0;
    for (; *name != '\0'; name++)
    {
        hashVal = *name + 31 * hashVal;
    }
    return hashVal % MAX_SYMBOLS;
}

/* Initialize the symbol table */
void initSymbolTable()
{
    int i;
    for (i = 0; i < MAX_SYMBOLS; i++)
    {
        symbolTable[i] = NULL;
    }
}

/* Look up a symbol in the symbol table */
struct Symbol *lookupSymbol(const char *name)
{
    struct Symbol *sym;
    printf("Looking up symbol: '%s'\n", name);
    sym = symbolTable[hashSymbolName(name)];
    while (sym != NULL)
    {
        printf("Checking symbol: '%s'\n", sym->symbolName);
        if (strcmp(name, sym->symbolName) == 0)
        {
            printf("Symbol found: '%s'\n", sym->symbolName);
            return sym;
        }
        sym = sym->next;
    }
    printf("Symbol '%s' not found.\n", name);
    return NULL;
}

/* Add a symbol to the symbol table */

void addSymbol(const char *name, SymbolType type, unsigned int value)
{
    unsigned int hashVal;
    Symbol *sym;

    printf("Inserting symbol %s to table\n", name);

    if ((sym = lookupSymbol(name)) == NULL)
    {
        sym = (Symbol *)malloc(sizeof(*sym));
        if (sym == NULL || (sym->symbolName = strdup((char *)name)) == NULL)
        {
            fprintf(stderr, "Memory allocation error\n");
            return;
        }
        sym->symbolType = type;
        sym->value = value;
        hashVal = hashSymbolName(name);
        sym->next = symbolTable[hashVal];
        symbolTable[hashVal] = sym;
    }
    else
    {
        fprintf(stderr, "%s already exists in the symbol table\n", name);
    }
}

/* Print the contents of the symbol table */
void printSymbolTable()
{
    int i;
    Symbol *sym;
    printf("Symbol Table Content:\n");
    for (i = 0; i < MAX_SYMBOLS; i++)
    {
        sym = symbolTable[i];
        if (sym != NULL)
        {
            printf("Bucket %d:\n", i);
            while (sym != NULL)
            {
                printf("  Name: '%s'\n", sym->symbolName);
                printf("  Type: %d\n", sym->symbolType);
                printf("  Value: %u\n", sym->value);
                sym = sym->next;
            }
        }
    }
}

void printMemory()
{
    int i;
    printf("Memory Content:\n");
    for (i = 0; i < DC; i++)
    {
        printf("Memory[%d]: %d\n", i, memory[i]);
    }
}