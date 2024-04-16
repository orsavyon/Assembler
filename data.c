#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
int IC = 0;           /* Instruction counter */
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

MemoryEntry memoryLines[MAX_DATA];
/* Data array for the assembler */

/**
 * Initializes the command table with predefined assembly commands.
 */
void initData()
{
    /* Initializing command table with command names, opcodes, and number of operands */

    commandTable[0].cmdName = "mov";
    commandTable[0].opCode = 0;
    commandTable[0].numOfOps = 2;
    commandTable[0].srcLegalAddrs[0] = 0;
    commandTable[0].srcLegalAddrs[1] = 1;
    commandTable[0].srcLegalAddrs[2] = 2;
    commandTable[0].srcLegalAddrs[3] = 3;
    commandTable[0].destLegalAddrs[0] = 1;
    commandTable[0].destLegalAddrs[1] = 2;
    commandTable[0].destLegalAddrs[2] = 3;

    commandTable[1].cmdName = "cmp";
    commandTable[1].opCode = 1;
    commandTable[1].numOfOps = 2;
    commandTable[1].srcLegalAddrs[0] = 0;
    commandTable[1].srcLegalAddrs[1] = 1;
    commandTable[1].srcLegalAddrs[2] = 2;
    commandTable[1].srcLegalAddrs[3] = 3;
    commandTable[1].destLegalAddrs[0] = 0;
    commandTable[1].destLegalAddrs[1] = 1;
    commandTable[1].destLegalAddrs[2] = 2;
    commandTable[1].destLegalAddrs[3] = 3;

    commandTable[2].cmdName = "add";
    commandTable[2].opCode = 2;
    commandTable[2].numOfOps = 2;
    commandTable[2].srcLegalAddrs[0] = 0;
    commandTable[2].srcLegalAddrs[1] = 1;
    commandTable[2].srcLegalAddrs[2] = 2;
    commandTable[2].srcLegalAddrs[3] = 3;
    commandTable[2].destLegalAddrs[0] = 1;
    commandTable[2].destLegalAddrs[1] = 2;
    commandTable[2].destLegalAddrs[2] = 3;

    commandTable[3].cmdName = "sub";
    commandTable[3].opCode = 3;
    commandTable[3].numOfOps = 2;
    commandTable[3].srcLegalAddrs[0] = 0;
    commandTable[3].srcLegalAddrs[1] = 1;
    commandTable[3].srcLegalAddrs[2] = 2;
    commandTable[3].srcLegalAddrs[3] = 3;
    commandTable[3].destLegalAddrs[0] = 1;
    commandTable[3].destLegalAddrs[1] = 2;
    commandTable[3].destLegalAddrs[2] = 3;

    commandTable[4].cmdName = "not";
    commandTable[4].opCode = 4;
    commandTable[4].numOfOps = 1;
    commandTable[4].destLegalAddrs[0] = 1;
    commandTable[4].destLegalAddrs[1] = 2;
    commandTable[4].destLegalAddrs[2] = 3;

    commandTable[5].cmdName = "clr";
    commandTable[5].opCode = 5;
    commandTable[5].numOfOps = 1;
    commandTable[5].destLegalAddrs[0] = 1;
    commandTable[5].destLegalAddrs[1] = 2;
    commandTable[5].destLegalAddrs[2] = 3;

    commandTable[6].cmdName = "lea";
    commandTable[6].opCode = 6;
    commandTable[6].numOfOps = 2;
    commandTable[6].srcLegalAddrs[0] = 1;
    commandTable[6].srcLegalAddrs[1] = 2;
    commandTable[6].destLegalAddrs[0] = 1;
    commandTable[6].destLegalAddrs[1] = 2;
    commandTable[6].destLegalAddrs[2] = 3;

    commandTable[7].cmdName = "inc";
    commandTable[7].opCode = 7;
    commandTable[7].numOfOps = 1;
    commandTable[7].destLegalAddrs[0] = 1;
    commandTable[7].destLegalAddrs[1] = 2;
    commandTable[7].destLegalAddrs[2] = 3;

    commandTable[8].cmdName = "dec";
    commandTable[8].opCode = 8;
    commandTable[8].numOfOps = 1;
    commandTable[8].destLegalAddrs[0] = 1;
    commandTable[8].destLegalAddrs[1] = 2;
    commandTable[8].destLegalAddrs[2] = 3;

    commandTable[9].cmdName = "jmp";
    commandTable[9].opCode = 9;
    commandTable[9].numOfOps = 1;
    commandTable[9].destLegalAddrs[0] = 1;
    commandTable[9].destLegalAddrs[1] = 3;

    commandTable[10].cmdName = "bne";
    commandTable[10].opCode = 10;
    commandTable[10].numOfOps = 1;
    commandTable[10].destLegalAddrs[0] = 1;
    commandTable[10].destLegalAddrs[1] = 3;

    commandTable[11].cmdName = "red";
    commandTable[11].opCode = 11;
    commandTable[11].numOfOps = 1;
    commandTable[11].destLegalAddrs[0] = 1;
    commandTable[11].destLegalAddrs[1] = 2;
    commandTable[11].destLegalAddrs[2] = 3;

    commandTable[12].cmdName = "prn";
    commandTable[12].opCode = 12;
    commandTable[12].numOfOps = 1;
    commandTable[12].destLegalAddrs[0] = 0;
    commandTable[12].destLegalAddrs[1] = 1;
    commandTable[12].destLegalAddrs[2] = 2;
    commandTable[12].destLegalAddrs[3] = 3;

    commandTable[13].cmdName = "jsr";
    commandTable[13].opCode = 13;
    commandTable[13].numOfOps = 1;
    commandTable[13].destLegalAddrs[0] = 1;
    commandTable[13].destLegalAddrs[1] = 3;

    commandTable[14].cmdName = "rts";
    commandTable[14].opCode = 14;
    commandTable[14].numOfOps = 0;

    commandTable[15].cmdName = "hlt";
    commandTable[15].opCode = 15;
    commandTable[15].numOfOps = 0;
}

int getOpcode(char *token)
{
    int i;
    for (i = 0; i < MAX_COMMANDS; i++)
    {
        if (strcmp(token, commandTable[i].cmdName) == 0)
        {
            return commandTable[i].opCode;
        }
    }
    return -1;
}

void printIstruction(Instruction *instruction)
{
    int i;
    printf("Instruction: %s\n", instruction->name);
    printf("Opcode: %d\n", instruction->opcode);
    for (i = 0; i < MAX_OPERANDS; i++)
    {
        if (instruction->operands[i] != NULL)
        {
            printf("Operand %d: %s\n", i, instruction->operands[i]);
        }
    }
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

    printf("Inserting symbol %s to table with value %d\n", name, value);

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
    for (i = 0; i < IC + DC; i++)
    {
        printf("Memory[%d]: %d\n", i, memory[i]);
    }
}

void initMemoryLines()
{
    int i;
    for (i = 0; i < MAX_DATA; i++)
    {
        memoryLines[i].word = malloc(sizeof(Word));
        if (memoryLines[i].word == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        memoryLines[i].word->value = 0;
        memoryLines[i].type = -1;
        memoryLines[i].value = 0;
        memoryLines[i].symbol = NULL;
    }
}

void freeMemoryLines()
{
    int i;
    for (i = 0; i < MAX_DATA; i++)
    {
        free(memoryLines[i].word);
        memoryLines[i].word = NULL;
    }
}

void printMemoryLines()
{
    int i;
    printf("Memory Content:\n");
    for (i = 0; i < IC + DC; i++)
    {
        switch (memoryLines[i].type)
        {
        case IMMEDIATE_ADDRESSING:
            printf("MemoryLines[%d] IMMEDIATE: ", i);
            printWordAsBinary(*memoryLines[i].word);
            break;
        case INDEX_ADDRESSING:
            printf("MemoryLines[%d] INDEX: ", i);
            printWordAsBinary(*memoryLines[i].word);
            break;
        case REGISTER_ADDRESSING:
            printf("MemoryLines[%d] REGISTER: ", i);
            printWordAsBinary(*memoryLines[i].word);
            break;
        case DIRECT_ADDRESSING:
            printf("MemoryLines[%d] DIRECT: ", i);
            printWordAsBinary(*memoryLines[i].word);
            break;
        default:
            printf("MemoryLines[%d]: ", i);
            printAsBinary(memoryLines[i].value);
            break;
        }
    }
}

void printWordAsBinary(Word word)
{
    int i;
    printf("Binary: ");
    for (i = 13; i >= 0; i--)
    {
        printf("%d", (word.value >> i) & 1);
    }
    printf("\n");
}

void printAsBinary(int value)
{
    int i;
    printf("Binary: ");
    for (i = 13; i >= 0; i--)
    {
        printf("%d", (value >> i) & 1);
    }
    printf("\n");
}

void setImmediateValue(Word *word, int immediateValue, unsigned int areBits)
{
    word->value = 0;
    areBits &= 0x03;
    if (immediateValue < 0)
    {
        immediateValue = (1 << 12) + immediateValue;
    }
    immediateValue &= 0xFFF;

    word->value = (immediateValue << 2) | areBits;
}

void setRegisterValue(Word *word, int srcRegNum, int destRegNum, int hasSrc, int hasDest)
{
    word->value = 0;

    srcRegNum &= 0x07;
    destRegNum &= 0x07;

    if (hasSrc)
    {
        word->value |= (srcRegNum << 5);
    }

    if (hasDest)
    {
        word->value |= (destRegNum << 2);
    }
}

int computeFourteenBitValue(int value)
{
    if (value < 0)
    {
        value = ((1 << 14) + value) & ((1 << 14) - 1);
    }
    else
    {
        value &= ((1 << 14) - 1);
    }
    return value;
}