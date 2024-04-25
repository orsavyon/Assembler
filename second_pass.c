#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "first_pass.h"
#include "second_pass.h"
#include "data.h"
#include "utils.h"
/**
 * Performs the second pass of the assembler over the source file.
 * This pass processes each line of the assembly source code to resolve symbols and finalize instruction encoding.
 * It handles directives and instructions specifically, ignoring blank lines and comments.
 *
 * @param fp Pointer to the source file being read.
 */
void secondPass(FILE *fp)
{
    char line[MAX_LINE_LENGTH]; /* Buffer to store each line from the file */
    lineNum = 0;                /* Reset line number counter for accurate error reporting */
                                /* Reset line error flag */

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) /* Read each line until the end of the file */
    {
        lineErrorFlag = 0;
        lineNum++;      /* Increment line number with each new line */
        trimLine(line); /* Remove leading and trailing whitespace */

        switch (getLineType(line)) /* Determine the type of the current line */
        {
        case LINE_BLANK:
        case LINE_COMMENT:
        case LINE_DEFINITION:
            /* Ignore blank, comment, and definition lines */
            break;
        case LINE_DIRECTIVE:
            handleDirective(line); /* Handle directives */
            break;

        case LINE_INSTRUCTION:
        case LINE_LABEL:

            break;
        case INVALID_LINE:
            handleError("Invalid line format", lineNum, line);
            break;
        }
    }
    encodeRemainingInstruction(); /* Encode any remaining instructions that need it */
    storeMemoryLine();            /* Finalize storage of memory lines */
}
/**
 * Processes assembly language directives based on the first token of a given line.
 * This function handles different types of directives: data, string, extern, define, and entry.
 * It delegates processing to other functions based on the type of directive encountered.
 *
 * @param line The line from the assembly source code to process for directives.
 */
void handleDirective(char *line)
{
    char *token, *symbolName;
    token = strtok(line, " \t\n"); /* Extract the first token which is expected to be the directive */
    if (token == NULL)
    {
        handleError("Invalid line format", lineNum, line); /* Handle error if line is empty or malformed */
        return;
    }
    switch (getDirectiveType(line)) /* Determine the type of directive based on the first token */
    {
    case DATA_DIRECTIVE:
    case STRING_DIRECTIVE:
    case EXTERN_DIRECTIVE:
    case DEFINE_DIRECTIVE:
        break;
    case ENTRY_DIRECTIVE:
        /* Process each symbol declared as an entry */
        while ((symbolName = strtok(NULL, " \t\n")) != NULL) /*Fetch next tokens as symbol names*/
        {
            Symbol *sym = lookupSymbol(symbolName); /* Look up the symbol in the symbol table */
            if (sym)                                /* Check if symbol is already defined */
            {
                if (sym->symbolType == external)
                {
                    handleError("Cannot declare external symbol as entry", lineNum, symbolName); /* Handle error if symbol is external */
                }
                else
                {
                    updateSymbolType(symbolName, entry);
                }
            }
            else
            {
                handleError("Symbol not found", lineNum, symbolName); /* Handle error if symbol not found */
            }
        }
        break;
    case INVALID_DIRECTIVE: /* Handle invalid directive error */
        break;
    }
}

/**
 * Encodes all remaining instructions in the memory that require encoding.
 * This function iterates through memory lines and encodes any symbols that have not been previously encoded.
 * It updates the value of each memory line with the new encoded value if needed.
 */
void encodeRemainingInstruction()
{
    int i;
    int newValue;
    for (i = 0; i < IC + DC; i++) /* Iterate over all memory lines */
    {
        if (memoryLines[i].needEncoding) /* Check if the current memory line needs encoding */
        {
            newValue = encodeSymbol(memoryLines[i].symbol); /* Encode the symbol associated with the memory line */
            memoryLines[i].value = newValue;                /* Update the memory line's value with the encoded symbol */
            memoryLines[i].needEncoding = 0;                /* Mark the memory line as encoded */
        }
    }
}
/**
 * Encodes a symbol's value with additional addressing bits based on its type.
 * This function looks up the symbol in the symbol table and, if found, encodes it based on its type:
 * - External symbols get ARE bits set to 01.
 * - Relocatable symbols get ARE bits set to 10.
 * If the symbol is not found, an error is handled and -1 is returned.
 *
 * @param symbol The name of the symbol to encode.
 * @return The encoded value of the symbol, or -1 if the symbol cannot be found.
 */
int encodeSymbol(char *symbol)
{
    Symbol *sym = lookupSymbol(symbol); /* Look up the symbol in the symbol table */
    unsigned int areBits;
    int encodedValue;
    if (sym != NULL)
    {
        /* Set ARE bits based on symbol type */
        areBits = (sym->symbolType == external) ? 0x01 : 0x02; /* ARE bits: 01 for external, 10 for relocatable */
        encodedValue = setValue(sym->value, areBits);          /* Combine the symbol's value with ARE bits */
        return encodedValue;
    }
    else
    {
        handleError("Symbol not found", lineNum, symbol); /* Handle error if symbol is not found */
        return -1;
    }
}
/**
 * Encodes an integer value with Additional Relocation Encoding (ARE) bits.
 * This function adjusts a given value to fit within 12 bits, and then appends 2 ARE bits at the lowest order.
 *
 * @param value The integer value to encode.
 * @param areBits The 2-bit ARE value to append to the encoded value. Only the lowest 2 bits are used.
 * @return The encoded value with ARE bits appended.
 */
int setValue(int value, unsigned int areBits)
{
    areBits &= 0x03; /* Mask to ensure only the lowest two bits of areBits are used */
    if (value < 0)
    {
        value = (1 << 12) + value; /* Adjust negative values to fit in 12-bit range */
    }
    value &= 0xFFF; /* Mask value to 12 bits */

    return (value << 2) | areBits; /* Shift the value left by 2 bits and append ARE bits at the lower end */
}