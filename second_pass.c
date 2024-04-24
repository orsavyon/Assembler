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

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        lineNum++;
        trimLine(line);

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
        case LINE_LABEL:

            break;
        case INVALID_LINE:
            break;
        }
    }
    encodeRemainingInstruction();
    storeMemoryLine();
}

void handleDirective(char *line)
{
    char *token, *symbolName;
    token = strtok(line, " \t\n");
    if (token == NULL)
    {
        handleError("Invalid line format", lineNum, line);
        return;
    }
    switch (getDirectiveType(line))
    {
    case DATA_DIRECTIVE:
    case STRING_DIRECTIVE:
    case EXTERN_DIRECTIVE:
    case DEFINE_DIRECTIVE:
        break;
    case ENTRY_DIRECTIVE:
        while ((symbolName = strtok(NULL, " \t\n")) != NULL) /*Fetch next tokens as symbol names*/
        {
            if (lookupSymbol(symbolName))
            {
                updateSymbolType(symbolName, entry);
            }
            else
            {
                handleError("Symbol not found", lineNum, symbolName);
            }
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
            newValue = encodeSymbol(memoryLines[i].symbol);
            memoryLines[i].value = newValue;
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

        areBits = (sym->symbolType == external) ? 0x01 : 0x02; /* ARE bits: 01 for external, 10 for relocatable */
        encodedValue = setValue(sym->value, areBits);
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