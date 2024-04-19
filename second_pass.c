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
    char *instructionPart;
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
            /*encodeRemainingInstruction(line);*/
        case LINE_LABEL:
            /*
                instructionPart = extractInstruction(line);
                printf("LINE_LABEL\n");
                if (instructionPart != NULL && isValidInstruction(instructionPart))
                {
                    printf("instructionPart: %s\n", instructionPart);
                    encodeRemainingInstruction(instructionPart);
                }
                */
            break;
        case INVALID_LINE:
            break;
        }
    }
    encodeRemainingInstruction2();
    printSymbolTable();
    printMemoryLines();
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

void encodeRemainingInstruction2()
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

void encodeRemainingInstruction(char *line)
{
    Instruction instruction;
    memset(&instruction, 0, sizeof(Instruction));

    printf("TEST --> In encodeRemainingInstruction\n");
    if (parseInstruction(line, &instruction) != NULL)
    {
        int i;
        printf("TEST --> in encodeRemainingInstruction Validated\n");
        for (i = 0; i < MAX_OPERANDS; i++)
        {
            Addressing addressingMethod;
            if (instruction.operands[i] != NULL)
            {
                addressingMethod = getAddressingMethod(instruction.operands[i]);
                printf("addressingMethod: %d\n", addressingMethod);
                printf("instruction.operands[i]: %s\n", instruction.operands[i]);

                if (addressingMethod == IMMEDIATE || addressingMethod == REGISTER)
                {
                    continue;
                }
                else if (addressingMethod == DIRECT || addressingMethod == INDEX)
                {
                    if (addressingMethod == INDEX)
                    {
                        char *symbol = strtok(instruction.operands[i], "[");
                        encodeSymbol(symbol);
                    }
                    else
                    {
                        encodeSymbol(instruction.operands[i]);
                    }
                }
                else
                {
                    handleError("Invalid addressing mode", lineNum, line);
                }
            }
            else
            {
                continue;
            }
        }
    }
    else
    {
        handleError("Error parsing instruction", lineNum, line);
    }
}

char *extractInstruction(char *line)
{
    char *colon = strchr(line, ':');
    if (colon != NULL)
    {
        char *instructionStart = colon + 1;
        while (isspace((unsigned char)*instructionStart))
        {
            instructionStart++;
        }
        if (*instructionStart != '\0')
        {
            return instructionStart;
        }
    }
    return NULL;
}

int encodeSymbol(char *symbol)
{
    Symbol *sym = lookupSymbol(symbol);
    unsigned int areBits;
    int encodedValue;
    if (sym != NULL)
    {

        printf("encodeSymbol: %s\n", symbol);
        areBits = (sym->symbolType == 1) ? 0x01 : 0x02; /* ARE bits: 01 for external, 10 for relocatable */
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