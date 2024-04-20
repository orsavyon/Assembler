#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constant definitions for assembler limits */
#define MAX_LABEL_LENGTH 31
#define MAX_SYMBOLS 100
#define MAX_DATA 1000
#define MAX_LABELS 100
#define MAX_COMMANDS 100
#define MAX_LINE_LENGTH 81
#define CMD_NUM 16
#define MAX_OPERANDS 2
#define MAX_SYMBOLS 100
#define MAX_RESERVED_WORDS 27

/* Global variables for assembler state */
extern int IC;               /* Instruction Counter */
extern int DC;               /* Data Counter */
extern int L;                /* Line number in the source code */
extern int symbolCount;      /* Number of symbols */
extern int dataCount;        /* Number of data entries */
extern int labelCount;       /* Number of labels */
extern int commandCount;     /* Number of commands */
extern int labelFlag;        /* Flag for label detection */
extern int dataFlag;         /* Flag for data detection */
extern int commandFlag;      /* Flag for command detection */
extern int symbolFlag;       /* Flag for symbol detection */
extern int entryFlag;        /* Flag for entry detection */
extern int externFlag;       /* Flag for extern detection */
extern int lineNum;          /* Current line number being processed */
extern int errorFlag;        /* Flag for error detection */
extern int memory[MAX_DATA]; /* Memory array for the assembler */

/* Array of saved words used by the assembler */
extern char *savedWords[];

/* Structure defining a command in the assembler */
typedef struct Command
{
    const char *cmdName;   /* Name of the command */
    int opCode;            /* Opcode of the command */
    int numOfOps;          /* Number of operands the command takes */
    int srcLegalAddrs[4];  /* Legal addressing methods for source operand */
    int destLegalAddrs[4]; /* Legal addressing methods for destination operand */
} Command;

extern Command commandTable[CMD_NUM]; /* Table of assembler commands */

typedef struct ParsedInstruction
{
    char *name;
    int opcode;
    char *operands[MAX_OPERANDS];
} Instruction;

typedef enum Addressings
{
    IMMEDIATE = 0,
    DIRECT = 1,
    INDEX = 2,
    REGISTER = 3,
    INVALID = -1
} Addressing;

/* Union defining a word in the assembler's memory */
typedef union Word
{
    struct
    {
        unsigned int na : 4;     /* Not used */
        unsigned int opcode : 4; /* Opcode part of the word */
        unsigned int srcOp : 2;  /* Source operand addressing mode */
        unsigned int desOp : 2;  /* Destination operand addressing mode */
        unsigned int ARE : 2;    /* A,R,E fields for assembler relocation */
    } bits;
    unsigned int value; /* The full word value */
} Word;

typedef enum AddressingMethod
{
    IMMEDIATE_ADDRESSING,
    DIRECT_ADDRESSING,
    INDEX_ADDRESSING,
    INDEX_ADDRESSING_VALUE,
    REGISTER_ADDRESSING,
    INSTRUCTION_ADDRESSING,
    INVALID_ADDRESSING
} AddressingMethod;

typedef struct MemoryEntry
{
    Word *word;
    AddressingMethod type;
    int needEncoding;
    int value;
    char *symbol;
    unsigned int address;
} MemoryEntry;

extern MemoryEntry memoryLines[MAX_DATA]; /* Memory array for the assembler */

extern unsigned int memoryAddress[MAX_DATA]; /* Memory array for the assembler */

/* Enumeration for different types of symbols */
typedef enum
{
    data,     /* Data symbol */
    code,     /* Code symbol */
    external, /* External symbol */
    entry,    /* Entry symbol */
    mdefine   /* Macro definition */
} SymbolType;

/* Structure defining a symbol in the symbol table */
typedef struct Symbol
{
    const char *symbolName; /* Name of the symbol */
    SymbolType symbolType;  /* Type of the symbol */
    unsigned int value;     /* Value of the symbol */
    struct Symbol *next;    /* Pointer to the next symbol in the table */
} Symbol;

extern Symbol *symbolTable[MAX_SYMBOLS]; /* The symbol table */

/* Enumeration for different types of directives */
typedef enum
{
    DATA_DIRECTIVE,
    STRING_DIRECTIVE,
    ENTRY_DIRECTIVE,
    EXTERN_DIRECTIVE,
    DEFINE_DIRECTIVE,
    INVALID_DIRECTIVE
} DirectiveType;

/* Structure defining a translation unit in the translation table */
typedef struct Translation
{
    int decimalAddress; /* Decimal address of the word */
    Word *word;         /* Pointer to the word being translated */
} Translation;

/* Function prototypes for operations on the assembler's data structures */
void initData();

int getOpcode(char *token); /* Get the opcode for a command */

void printIstruction(Instruction *instruction);

/* Initialize data for assembly processing */
void insertSymbolToTable(const char *symbolName, SymbolType symbolType, unsigned int value); /* Insert a symbol into the symbol table */
/**
 * Generates a hash value for a symbol name.
 *
 * @param name The symbol name to hash.
 * @return The hash value for the symbol name.
 */
unsigned int hashSymbolName(const char *name);

/**
 * Initializes the symbol table by setting all entries to NULL.
 */
void initSymbolTable();

/**
 * Looks up a symbol in the symbol table.
 *
 * @param name The name of the symbol to find.
 * @return A pointer to the found symbol or NULL if not found.
 */
struct Symbol *lookupSymbol(const char *name);

/**
 * Adds a symbol to the symbol table.
 *
 * @param name The name of the symbol to add.
 * @param type The type of the symbol.
 * @param value The value of the symbol.
 */
void addSymbol(const char *name, SymbolType type, unsigned int value);

/**
 * Prints the contents of the symbol table.
 */
void printSymbolTable();

void updateSymbolValues();

/**
 * Initializes the translation table with NULL values.
 */
void initTranslationTable();

/**
 * Adds a line to the translation table at the specified decimal address.
 *
 * @param decimalAddress The address in the table where the word will be added.
 * @param word The word to be added at the specified address.
 */
void insertTranslationToTable(int decimalAddress, Word *word);

int isReservedWord(char *word);

void printMemory();

void printMemoryLines();

void initMemoryLines();

void freeMemoryLines();

void printWordAsBinary(const Word word);

void setImmediateValue(Word *word, int immediateValue, unsigned int areBits);

void setRegisterValue(Word *word, int srcRegNum, int destRegNum, int hasSrc, int hasDest);

int computeFourteenBitValue(int value);
void printAsBinary(int value);
void printFirstWordAsBinary(Word word);
unsigned int getFirstWordAsBinary(Word word);
void printMemoryAddress();
void storeMemoryLine();
#endif /* DATA_H */
