#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "first_pass.h"
#include "second_pass.h"
#include "data.h"
#include "utils.h"

void secondPass(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    lineNum = 0;

    printf("\n --- in secondPass --- \n\n");
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        lineNum++;
        trimLine(line);
        printf("line: %s\n", line);

        switch (getLineType(line))
        {
        case LINE_BLANK:
        case LINE_COMMENT:
        case LINE_DEFINITION:
            break;
        case LINE_DIRECTIVE:
            handleDirective(line);
            break;

        case LINE_INSTRUCTION:
            printf("LINE_INSTRUCTION\n");
        case LINE_LABEL:

            break;
        case INVALID_LINE:
            break;
        }
    }
    encodeRemainingInstruction();
    printSymbolTable();
    printMemoryLines();
    storeMemoryLine();
    printMemoryAddress();
    printf("IC = %d\n", IC);
    printf("DC = %d\n", DC);
}

void handleDirective(char *line)
{
    char *symbolName;

    switch (getDirectiveType(line))
    {
    case DATA_DIRECTIVE:
    case STRING_DIRECTIVE:
    case EXTERN_DIRECTIVE:
        break;
    case ENTRY_DIRECTIVE:
        printf("ENTRY_DIRECTIVE\n");
        symbolName = strtok(line, " \t\n");
        printf("symbolName: %s\n", symbolName);
        while (symbolName != NULL)
        {
            if (isSymbol(symbolName))
            {
                printf("symbolName: %s\n", symbolName);
            }
            symbolName = strtok(NULL, " \t\n");
        }
        break;
    case INVALID_DIRECTIVE:
        break;
    }
}

void encodeRemainingInstruction()
{
    int i;
    int newValue;
    for (i = 0; i < IC + DC; i++)
    {
        if (memoryLines[i].needEncoding)
        {
            printf("memoryLines[%d].symbol: %s\n", i, memoryLines[i].symbol);
            newValue = encodeSymbol(memoryLines[i].symbol);
            printf("newValue: %d\n", newValue);
            memoryLines[i].value = newValue;
            printf("memoryLines[%d].value: %d\n", i, memoryLines[i].value);
            memoryLines[i].needEncoding = 0;
        }
    }
}

int encodeSymbol(char *symbol)
{
    Symbol *sym = lookupSymbol(symbol);
    unsigned int areBits;
    int encodedValue;
    if (sym != NULL)
    {

        printf("encodeSymbol: %s\n", symbol);
        areBits = (sym->symbolType == external) ? 0x01 : 0x02; /* ARE bits: 01 for external, 10 for relocatable */
        encodedValue = setValue(sym->value, areBits);
        printf("encodedValue: %d \n", encodedValue);
        printAsBinary(encodedValue);
        return encodedValue;
    }
    else
    {
        handleError("Symbol not found", lineNum, symbol);
        return -1;
    }
}

int setValue(int value, unsigned int areBits)
{
    areBits &= 0x03;
    if (value < 0)
    {
        value = (1 << 12) + value;
    }
    value &= 0xFFF;

    return (value << 2) | areBits;
}