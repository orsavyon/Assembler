#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constant definitions for assembler limits */
#define MAX_LABEL_LENGTH 31
#define MAX_SYMBOLS 100
#define MAX_DATA 4097
#define MIN_12BIT_VALUE -2048
#define MAX_12BIT_VALUE 2047
#define MAX_LABELS 100
#define MAX_COMMANDS 100
#define MAX_LINE_LENGTH 81
#define CMD_NUM 16
#define MAX_OPERANDS 2
#define MAX_SYMBOLS 100
#define MAX_RESERVED_WORDS 27
#define MAX_FILENAME_LEN 260
#define MAX_EXTERNAL_USAGES 1000

/* Global variables for assembler state */
extern int IC;                 /* Instruction Counter */
extern int DC;                 /* Data Counter */
extern int L;                  /* Line number in the source code */
extern int symbolCount;        /* Number of symbols */
extern int dataCount;          /* Number of data entries */
extern int labelCount;         /* Number of labels */
extern int commandCount;       /* Number of commands */
extern int labelFlag;          /* Flag for label detection */
extern int dataFlag;           /* Flag for data detection */
extern int commandFlag;        /* Flag for command detection */
extern int symbolFlag;         /* Flag for symbol detection */
extern int entryFlag;          /* Flag for entry detection */
extern int externFlag;         /* Flag for extern detection */
extern int lineNum;            /* Current line number being processed */
extern int errorFlag;          /* Flag for error detection */
extern int externalUsageCount; /* Number of external symbols used in the program */
extern int memory[MAX_DATA];   /* Memory array for the assembler */

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

typedef struct externalSymbolUsage
{
    char *symbolName;
    int address;
} ExternalSymbolUsage;
extern ExternalSymbolUsage externalUsages[MAX_EXTERNAL_USAGES];

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

void printExternSymbolUsage();

void updateSymbolValues();

void recordExternalSymbolUsage(char *symbolName, int address);

void updateSymbolType(char *symbolName, int type);

/**
 * Initializes the translation table with NULL values.
 */
void initTranslationTable();
/**
 * @brief Checks if a given word is a reserved word in the assembler.
 *
 * This function iterates through an array of reserved words and compares each with the provided
 * word. If a match is found, the function returns 1, indicating the word is reserved.
 *
 * @param word The word to check against the list of reserved words.
 * @return 1 if the word is a reserved word, 0 otherwise.
 */
int isReservedWord(char *word);
/**
 * @brief Displays the content of memory up to the combined count of instruction and data counters.
 *
 * Iterates through the memory array from 0 to the sum of IC (Instruction Counter) and DC (Data Counter),
 * printing each memory cell's index and value.
 */
void printMemory();
/**
 * @brief Displays the content of memory lines based on their addressing types.
 *
 * Iterates through the memory lines up to the sum of IC (Instruction Counter) and DC (Data Counter),
 * printing each memory line's type and value or binary representation depending on the addressing type.
 */
void printMemoryLines();
/**
 * @brief Initializes the memory lines array used in the assembler.
 *
 * Allocates memory for each 'Word' in the memoryLines array and sets initial values.
 * If memory allocation fails, the function will terminate the program.
 */
void initMemoryLines();
/**
 * @brief Frees allocated memory for each word in the memoryLines array.
 *
 * Iterates through the memoryLines array and frees the memory allocated for each word,
 * then sets the pointer to NULL to avoid dangling references.
 */
void freeMemoryLines();

void printWordAsBinary(const Word word);
/**
 * @brief Sets the immediate value and ARE bits in a Word's value field.
 *
 * This function configures a Word structure by setting its value field with an immediate
 * value shifted by two positions and combined with the ARE (Assembler Relocation Entries) bits.
 * It processes the immediate value to handle negative numbers appropriately within a 12-bit limit.
 *
 * @param word Pointer to the Word structure to be modified.
 * @param immediateValue The immediate value to encode into the word.
 * @param areBits The A, R, E bits to set in the lower two bits of the word.
 */
void setImmediateValue(Word *word, int immediateValue, unsigned int areBits);
/**
 * @brief Configures a Word structure to encode register numbers and their presence.
 *
 * This function sets up the Word's value to represent source and destination register numbers
 * based on provided flags indicating their presence. It positions the register numbers in the
 * word value based on predefined bit positions: source register at bits 5-7 and destination register
 * at bits 2-4.
 *
 * @param word Pointer to the Word structure to be modified.
 * @param srcRegNum Source register number.
 * @param destRegNum Destination register number.
 * @param hasSrc Flag indicating if the source register is to be encoded.
 * @param hasDest Flag indicating if the destination register is to be encoded.
 */
void setRegisterValue(Word *word, int srcRegNum, int destRegNum, int hasSrc, int hasDest);
/**
 * @brief Computes a fourteen-bit representation of an integer value.
 *
 * This function adjusts a given integer to fit within a fourteen-bit range. If the value is negative,
 * it is converted using two's complement within the 14-bit limit. For positive values, it simply masks
 * the value to ensure it does not exceed fourteen bits.
 *
 * @param value The integer value to be converted.
 * @return The fourteen-bit adjusted integer.
 */
int computeFourteenBitValue(int value);

/**
 * @brief Prints the binary representation of the full word value from a Word structure.
 *
 * This function outputs the 14-bit binary representation of the 'value' field in the Word structure,
 * displaying it as a binary string from the most significant bit to the least significant bit.
 *
 * @param word The Word structure whose full word value is to be printed in binary.
 */
void printAsBinary(int value);
/**
 * @brief Prints the binary representation of a Word structure.
 *
 * This function sequentially prints the binary bits of each field within a Word's bit-fields:
 * 'na', 'opcode', 'srcOp', 'desOp', and 'ARE', displaying the complete binary format of the word.
 *
 * @param word The Word structure whose binary representation is to be printed.
 */
void printFirstWordAsBinary(Word word);
/**
 * @brief Converts the components of a Word structure into a binary format.
 *
 * This function takes each field of the Word's bit-fields and positions them into a single
 * unsigned integer according to their respective bit positions in the assembler's binary format.
 *
 * @param word The Word structure to convert.
 * @return The unsigned integer representing the binary format of the Word.
 */
unsigned int getFirstWordAsBinary(Word word);
/**
 * @brief Displays the content of the memory address array.
 *
 * Iterates through the memory address array from 0 to the sum of the Instruction Counter (IC)
 * and Data Counter (DC), printing each address and its corresponding value in binary format.
 */
void printMemoryAddress();
/**
 * @brief Stores the values from memory lines into the memory address array based on addressing type.
 *
 * Iterates through combined memory lines determined by the Instruction Counter (IC) and Data Counter (DC),
 * storing values in the memory address array. The type of value stored depends on the addressing method of the line.
 */
void storeMemoryLine();
#endif /* DATA_H */
