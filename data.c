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

unsigned int memoryAddress[MAX_DATA];
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

/**
 * @brief Retrieves the opcode corresponding to a given command name.
 *
 * This function searches the command table for a command matching the provided token
 * and returns the associated opcode if found.
 *
 * @param token The command name to search for in the command table.
 * @return The opcode of the command if found, or -1 if the command does not exist.
 */
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

/**
 * @brief Prints the details of an instruction to the standard output.
 *
 * This function displays the name and opcode of the instruction, along with any operands
 * associated with it, if they are not NULL.
 *
 * @param instruction Pointer to the Instruction structure to be printed.
 */
void printIstruction(Instruction *instruction)
{
    int i;
    printf("Instruction: %s\n", instruction->name); /* Print the name of the instruction */
    printf("Opcode: %d\n", instruction->opcode);    /* Print the opcode of the instruction */
    for (i = 0; i < MAX_OPERANDS; i++)              /* Loop through possible operands */
    {
        if (instruction->operands[i] != NULL) /* Check if operand is not NULL */
        {
            printf("Operand %d: %s\n", i, instruction->operands[i]); /* Print operand */
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
/* Update a the given symbol's type */

void updateSymbolType(char *symbolName, int type)
{
    Symbol *sym;
    sym = lookupSymbol(symbolName);
    if (sym != NULL)
    {
        sym->symbolType = type;
    }
    else
    {
        fprintf(stderr, "Symbol '%s' not found\n", symbolName);
    }
}

/* Update a symbol's value */
void updateSymbolValues()
{
    int i;
    Symbol *sym;
    for (i = 0; i < MAX_SYMBOLS; i++)
    {
        sym = symbolTable[i];
        while (sym != NULL)
        {
            if (sym->symbolType == data)
            {
                sym->value += IC + 100;
            }
            sym = sym->next;
        }
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
/**
 * @brief Checks if a given word is a reserved word in the assembler.
 *
 * This function iterates through an array of reserved words and compares each with the provided
 * word. If a match is found, the function returns 1, indicating the word is reserved.
 *
 * @param word The word to check against the list of reserved words.
 * @return 1 if the word is a reserved word, 0 otherwise.
 */
int isReservedWord(char *word)
{
    int i;
    for (i = 0; i < MAX_RESERVED_WORDS; i++)
    {
        if (strcmp(word, savedWords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Displays the content of memory up to the combined count of instruction and data counters.
 *
 * Iterates through the memory array from 0 to the sum of IC (Instruction Counter) and DC (Data Counter),
 * printing each memory cell's index and value.
 */
void printMemory()
{
    int i;
    printf("Memory Content:\n");
    for (i = 0; i < IC + DC; i++) /* Loop through memory array up to IC + DC */
    {
        printf("Memory[%d]: %d\n", i, memory[i]);
    }
}

/**
 * @brief Initializes the memory lines array used in the assembler.
 *
 * Allocates memory for each 'Word' in the memoryLines array and sets initial values.
 * If memory allocation fails, the function will terminate the program.
 */
void initMemoryLines()
{
    int i;
    for (i = 0; i < MAX_DATA; i++) /* Iterate over the memoryLines array up to MAX_DATA */
    {
        memoryLines[i].word = malloc(sizeof(Word)); /* Allocate memory for each word */
        if (memoryLines[i].word == NULL)            /* Check if memory allocation was successful */
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE); /* Terminate program on failed memory allocation */
        }
        memoryLines[i].word->value = 0; /* Initialize the value of the word */
        memoryLines[i].type = -1;       /* Set default type to -1 indicating unused or invalid */
        memoryLines[i].value = 0;       /* Set default value to 0 */
        memoryLines[i].symbol = NULL;   /* No associated symbol initially */
    }
}

/**
 * @brief Frees allocated memory for each word in the memoryLines array.
 *
 * Iterates through the memoryLines array and frees the memory allocated for each word,
 * then sets the pointer to NULL to avoid dangling references.
 */
void freeMemoryLines()
{
    int i;
    for (i = 0; i < MAX_DATA; i++) /* Loop through each entry in the memoryLines array */
    {
        free(memoryLines[i].word);  /* Free the allocated memory for the word */
        memoryLines[i].word = NULL; /* Set the pointer to NULL after freeing */
    }
}

/**
 * @brief Displays the content of memory lines based on their addressing types.
 *
 * Iterates through the memory lines up to the sum of IC (Instruction Counter) and DC (Data Counter),
 * printing each memory line's type and value or binary representation depending on the addressing type.
 */
void printMemoryLines()
{
    int i;
    printf("Memory Content:\n");
    for (i = 0; i < IC + DC; i++)
    {
        switch (memoryLines[i].type) /* Determine the type of addressing used */
        {
        case IMMEDIATE_ADDRESSING:
            printf("MemoryLines[%d] IMMEDIATE: ", i);
            printWordAsBinary(*memoryLines[i].word);
            break;
        case INDEX_ADDRESSING:
            printf("MemoryLines[%d] INDEX: ", i);

            printAsBinary(memoryLines[i].value);
            break;
        case INDEX_ADDRESSING_VALUE:
            printf("MemoryLines[%d] INDEX VALUE: ", i);
            printWordAsBinary(*memoryLines[i].word);
            break;
        case REGISTER_ADDRESSING:
            printf("MemoryLines[%d] REGISTER: ", i);
            printWordAsBinary(*memoryLines[i].word);
            break;
        case DIRECT_ADDRESSING:
            printf("MemoryLines[%d] DIRECT: ", i);
            printAsBinary(memoryLines[i].value);
            break;
        case INSTRUCTION_ADDRESSING:
            printf("MemoryLines[%d] INSTRUCTION: ", i);
            printFirstWordAsBinary(*memoryLines[i].word);
            break;
        default:
            printf("MemoryLines[%d]: ", i);
            printAsBinary(memoryLines[i].value);
            break;
        }
    }
}
/**
 * @brief Stores the values from memory lines into the memory address array based on addressing type.
 *
 * Iterates through combined memory lines determined by the Instruction Counter (IC) and Data Counter (DC),
 * storing values in the memory address array. The type of value stored depends on the addressing method of the line.
 */
void storeMemoryLine()
{
    int i;
    unsigned int binaryValue;
    for (i = 0; i < IC + DC; i++)
    {
        switch (memoryLines[i].type)
        {
        case IMMEDIATE_ADDRESSING:
        case INDEX_ADDRESSING_VALUE:
        case REGISTER_ADDRESSING:
            memoryAddress[i] = memoryLines[i].word->value;
            break;
        case DIRECT_ADDRESSING:
        case INDEX_ADDRESSING:
            memoryAddress[i] = memoryLines[i].value;
            break;
        case INSTRUCTION_ADDRESSING:
            binaryValue = getFirstWordAsBinary(*memoryLines[i].word);
            memoryAddress[i] = binaryValue;
            break;
        default:
            memoryAddress[i] = memoryLines[i].value;
            break;
        }
    }
}

/**
 * @brief Converts the components of a Word structure into a binary format.
 *
 * This function takes each field of the Word's bit-fields and positions them into a single
 * unsigned integer according to their respective bit positions in the assembler's binary format.
 *
 * @param word The Word structure to convert.
 * @return The unsigned integer representing the binary format of the Word.
 */
unsigned int getFirstWordAsBinary(Word word)
{
    unsigned int result = 0;         /* Initialize result to zero */
    result |= word.bits.na << 10;    /* Shift 'na' bits and add to result */
    result |= word.bits.opcode << 6; /* Shift 'opcode' bits and add to result */
    result |= word.bits.srcOp << 4;  /* Shift 'source operand' bits and add to result */
    result |= word.bits.desOp << 2;  /* Shift 'destination operand' bits and add to result */
    result |= word.bits.ARE;         /* Add 'ARE' bits directly */
    return result;                   /* Return the composed binary result */
}

/**
 * @brief Displays the content of the memory address array.
 *
 * Iterates through the memory address array from 0 to the sum of the Instruction Counter (IC)
 * and Data Counter (DC), printing each address and its corresponding value in binary format.
 */
void printMemoryAddress()
{
    int i;
    printf("Memory Address Content:\n");
    for (i = 0; i < IC + DC; i++)
    {
        printf("%d : %d : ", i + 100, memoryAddress[i]);
        printAsBinary(memoryAddress[i]); /* Print the binary representation of the memory address */
    }
}

/**
 * @brief Prints the binary representation of a Word structure.
 *
 * This function sequentially prints the binary bits of each field within a Word's bit-fields:
 * 'na', 'opcode', 'srcOp', 'desOp', and 'ARE', displaying the complete binary format of the word.
 *
 * @param word The Word structure whose binary representation is to be printed.
 */
void printFirstWordAsBinary(Word word)
{
    int i;
    printf("Binary: ");

    for (i = 3; i >= 0; i--)
    {
        printf("%d", (word.bits.na >> i) & 1);
    }

    for (i = 3; i >= 0; i--)
    {
        printf("%d", (word.bits.opcode >> i) & 1);
    }

    for (i = 1; i >= 0; i--)
    {
        printf("%d", (word.bits.srcOp >> i) & 1);
    }

    for (i = 1; i >= 0; i--)
    {
        printf("%d", (word.bits.desOp >> i) & 1);
    }

    for (i = 1; i >= 0; i--)
    {
        printf("%d", (word.bits.ARE >> i) & 1);
    }

    printf("\n");
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

/**
 * @brief Prints the binary representation of the full word value from a Word structure.
 *
 * This function outputs the 14-bit binary representation of the 'value' field in the Word structure,
 * displaying it as a binary string from the most significant bit to the least significant bit.
 *
 * @param word The Word structure whose full word value is to be printed in binary.
 */
void printAsBinary(int value)
{
    int i;
    printf("Binary: ");
    for (i = 13; i >= 0; i--) /* Iterate over each bit in the 14-bit word */
    {
        printf("%d", (value >> i) & 1); /* Print each bit of the word value */
    }
    printf("\n");
}
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
void setImmediateValue(Word *word, int immediateValue, unsigned int areBits)
{
    word->value = 0; /* Reset the word value */
    areBits &= 0x03; /* Ensure ARE bits are within the lowest two bits range */
                     /* Adjust immediateValue for negative inputs within a 12-bit representation */
    if (immediateValue < 0)
    {
        immediateValue = (1 << 12) + immediateValue; /* Two's complement within 12 bits */
    }
    immediateValue &= 0xFFF; /* Mask to ensure the value fits in 12 bits */

    word->value = (immediateValue << 2) | areBits; /* Set word value with immediate value and ARE bits */
}

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

void setRegisterValue(Word *word, int srcRegNum, int destRegNum, int hasSrc, int hasDest)
{
    word->value = 0; /* Initialize word value to zero */

    srcRegNum &= 0x07;  /* Mask source register number to ensure it is within 0 to 7 */
    destRegNum &= 0x07; /* Mask destination register number similarly */

    if (hasSrc)
    {
        word->value |= (srcRegNum << 5); /* Encode source register number starting at bit 5 */
    }

    if (hasDest)
    {
        word->value |= (destRegNum << 2); /* Encode destination register number starting at bit 2 */
    }
}

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
int computeFourteenBitValue(int value)
{
    if (value < 0)
    {
        value = ((1 << 14) + value) & ((1 << 14) - 1); /* Convert negative number to two's complement within 14 bits */
    }
    else
    {
        value &= ((1 << 14) - 1); /* Ensure value does not exceed 14 bits */
    }
    return value; /* Return the adjusted value */
}