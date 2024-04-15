#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"
#include "first_pass.h"
#include "data.h"

/*
 * This code implements the first pass of a two-pass assembler. It reads an assembly source file line-by-line,
 * categorizes each line, and processes it accordingly. The major functions include:
 * - Initializing data structures for symbols and machine instructions.
 * - Categorizing lines as blank, comment, definition, label, directive, or instruction, and processing them.
 * - Handling labels and adding them to a symbol table with appropriate attributes based on the context (data or code).
 * - Processing directives for data allocation, string handling, and extern definitions.
 * - Parsing assembly instructions, generating machine code, and updating program counters.
 * - Robust error handling for various syntax and semantic issues to ensure the assembler's stability and reliability.
 * - Final steps include printing the symbol table and updating symbol values based on data and instruction sizes.
 * This structured approach ensures the source code is prepared for the second pass where actual binary translation occurs.
 */

/*
 * TODO: need to implement encoding to binary and changing memory structure to translate
 */

void firstPass(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    IC = 0;
    DC = 0;

    initData();
    initSymbolTable();
    initMemoryLines();

    printf("\n --- in firstPass --- \n\n");
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        lineNum++;
        trimLine(line);
        printf("TEST --> Line %d: %s\n", lineNum, line);

        switch (getLineType(line))
        {
        case LINE_BLANK:
            break;
        case LINE_COMMENT:
            break;

        case LINE_DEFINITION:
            printf("TEST --> Definition\n");
            processDefinition(line);
            break;

        case LINE_LABEL:
        {
            char symbolName[MAX_LINE_LENGTH];
            char *symbolPos;
            sscanf(line, "%[^:]", symbolName);
            symbolPos = strstr(line, symbolName);
            printf("TEST --> Label: %s\n", symbolName);
            symbolFlag = 1;
            if (symbolPos)
            {
                char *remainingLine = symbolPos + strlen(symbolName) + 1;

                trimLine(remainingLine);
                printf("TEST --> Remaining line: %s\n", remainingLine);

                if (getLineType(remainingLine) == LINE_DIRECTIVE)
                {
                    printf("TEST --> Directive in label\n");
                    if (getDirectiveType(remainingLine) == DATA_DIRECTIVE || getDirectiveType(remainingLine) == STRING_DIRECTIVE)
                    {
                        if (lookupSymbol(symbolName) == NULL)
                        {
                            addSymbol(symbolName, data, DC);
                        }
                        else
                        {
                            handleError("TEST --> Symbol already exists", lineNum, line);
                        }
                    }
                    processDirective(remainingLine);
                }
                else if (symbolFlag == 1)
                {

                    if (lookupSymbol(symbolName) == NULL)
                    {
                        printf("TEST --> Inserting symbol %s with code attribute and IC = %d\n", symbolName, IC);
                        addSymbol(symbolName, code, IC + 100);
                    }
                    else
                    {
                        handleError("TEST --> Symbol already defined", lineNum, line);
                    }
                    if (getLineType(remainingLine) == LINE_INSTRUCTION)
                    {
                        printf("TEST --> Instruction in label\n");
                        processInstruction(remainingLine);
                        printf("TEST --> IC after label instruction: %d\n", IC);
                    }
                    else
                    {
                        handleError("TEST --> Invalid line", lineNum, line);
                    }
                }
                else
                {
                    handleError("TEST --> Invalid line", lineNum, line);
                }
            }
            break;
        }

        case LINE_DIRECTIVE:
            printf("TEST --> Directive\n");
            processDirective(line);
            break;

        case LINE_INSTRUCTION:
            printf("TEST --> Instruction\n");
            processInstruction(line);
            printf("TEST --> IC after process instruction: %d\n", IC);
            break;

        case INVALID_LINE:
            handleError("TEST --> Invalid line", lineNum, line);
            break;
        default:
            break;
        }
        symbolFlag = 0;
    }
    printSymbolTable();
    printMemory();
    printf(" --- memory lines --- \n");
    printMemoryLines();

    if (errorFlag == 1)
    {
        printf("TEST --> Error flag is set\n");
    }
    else
    {
        printf("TEST --> Error flag is not set\n");
    }
}

/* start HELPERS code */

char *getFirstWord(const char *line)
{
    static char firstWord[MAX_LINE_LENGTH];
    int i = 0;

    while (isspace((unsigned char)*line))
    {
        line++;
    }

    while (*line && !isspace((unsigned char)*line) && i < MAX_LINE_LENGTH - 1)
    {
        firstWord[i++] = *line++;
    }

    firstWord[i] = '\0';
    printf("TEST --> First word: %s\n", firstWord);
    return firstWord;
}

LineType getLineType(char *line)
{
    if (line[0] == '\n' || line[0] == '\0' || isspace((unsigned char)line[0]))
    {
        return LINE_BLANK;
    }
    if (line[0] == ';')
    {
        return LINE_COMMENT;
    }
    if (isConstantDefinition(line))
    {
        return LINE_DEFINITION;
    }
    if (isSymbol(line))
    {
        return LINE_LABEL;
    }
    if (line[0] == '.')
    {
        return LINE_DIRECTIVE;
    }
    if (isInstruction(line))
    {
        return LINE_INSTRUCTION;
    }

    return INVALID_LINE;
}

int isSymbol(char *line)
{
    int i;
    printf("TEST --> Checking if symbol\n");
    for (i = 0; i < strlen(line); i++)
    {
        if (line[i] == ':' && line[i + 1] == ' ')
        {
            printf("TEST --> Symbol found\n");
            return 1;
        }
    }
    return 0;
}
/* end HELPERS code */

/* ------ start LINE_DEFINITION code ------*/

int isConstantDefinition(char *line)
{
    printf("TEST --> Checking if constant definition\n");
    return strcmp(getFirstWord(line), ".define") == 0;
}

void processDefinition(char *line)
{
    if (isValidConstantDefinition(line))
    {
        char *constantName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
        int value;

        sscanf(line, ".define %[^=]=%d", constantName, &value);
        trimLine(constantName);
        addSymbol(constantName, mdefine, value);
        printf("TEST --> Valid constant definition\n");
    }
    else
    {
        handleError("TEST --> Invalid constant definition", lineNum, line);
    }
}

int isValidConstantDefinition(char *line)
{
    char *constantName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    int value;

    printf("TEST --> Validating constant definition\n");
    /* Parse the line to extract the constant name and value */
    if (strstr(line, "=") == NULL)
    {
        handleError("TEST --> Invalid constant definition: Missing equal sign", lineNum, line);
        return 0;
    }

    sscanf(line, ".define %[^=]=%d", constantName, &value);
    trimLine(constantName);
    printf("TEST --> Constant name: '%s'\n", constantName);
    printf("TEST --> Value: %d\n", value);

    /* Trim leading and trailing white spaces from the constant name and value */
    /*trim(constantName);*/
    /*trimValue(line);*/

    /* Check if the constant name is already defined */
    if (lookupSymbol(constantName) != NULL)
    {
        handleError("TEST --> Symbol is already defined", lineNum, line);
        free(constantName);
        return 0;
    }

    /* Convert the value to an integer */

    /* Check if the value is within the range of a 12-bit integer */
    if (value < -2048 || value > 2047)
    {
        handleError("TEST --> Value is out of range", lineNum, line);
        free(constantName);
        return 0;
    }

    free(constantName);

    return 1;
}

/* ------ end LINE_DEFINITION code ------ */

/* ------ start LINE_DIRECTIVE code ------ */

void processDirective(char *line)
{
    switch (getDirectiveType(line))
    {
    case DATA_DIRECTIVE:
        printf("TEST --> Data directive\n");
        processDataDirective(line);

        break;
    case STRING_DIRECTIVE:
        printf("TEST --> String directive\n");
        processDataDirective(line);

        break;
    case ENTRY_DIRECTIVE:
        printf("TEST --> Entry directive\n");
        break;
    case EXTERN_DIRECTIVE:
        if (symbolFlag == 1)
        {
            break;
        }
        else
        {
            printf("TEST --> Extern directive\n");
            processExternDirective(line);
            break;
        }
    case INVALID_DIRECTIVE:
        handleError("TEST --> Invalid directive", lineNum, line);
        break;
    }
}

void processDataDirective(char *line)
{
    char *token;
    printf("TEST --> Processing data directive\n\t----Line: %s\n", line);

    if (strncmp(line, ".data", 5) == 0)
    {
        token = strtok(line + 6, ",");
        while (token != NULL)
        {
            Symbol *symbol;
            printf("TEST --> Token: %s\n", token);
            trimLine(token);

            symbol = lookupSymbol(token);
            if (symbol)
            {
                printf("TEST --> Symbol Found: %s\n", symbol->symbolName);
            }
            else
            {
                printf("TEST --> Symbol Not Found: %s\n", token);
            }

            if (symbol && symbol->symbolType == mdefine)
            {
                printf("TEST --> Symbol: %s\n", symbol->symbolName);
                printf("TEST --> DC: %d\n", DC);
                memory[DC + IC] = symbol->value;
                memoryLines[DC + IC].type = data;
                memoryLines[DC + IC].value = symbol->value;
                DC++;
            }
            else if (isdigit(token[0]) || token[0] == '-' || token[0] == '+')
            {
                printf("TEST --> DC: %d\n", DC);

                memory[DC + IC] = atoi(token);
                memoryLines[DC + IC].type = data;
                memoryLines[DC + IC].value = atoi(token);

                DC++;
            }
            else
            {
                printf("Error: Undefined symbol or invalid number in .data directive: %s\n", token);
            }
            token = strtok(NULL, ",");
        }
    }
    else if (strncmp(line, ".string", 7) == 0)
    {

        char *start = strchr(line, '\"');
        char *end = NULL;
        if (!start)
        {
            start = strstr(line, "“");
            if (start)
            {
                start += 3;
            }
        }
        else
        {
            start += 1;
        }

        if (start)
        {
            end = strchr(start, '\"');
            if (!end)
            {
                end = strstr(start, "”");
            }
        }

        printf("TEST --> Processing string directive\n");

        if (start && end && start < end)
        {
            char *c;
            printf("TEST --> String: %.*s\n", (int)(end - start), start);

            for (c = start; c < end; c++)
            {
                printf("TEST --> Char: %c\n", *c);
                printf("TEST --> DC: %d\n", DC);
                memory[DC + IC] = (unsigned char)*c;
                memoryLines[DC + IC].type = data;
                memoryLines[DC + IC].value = (unsigned char)*c;

                DC++;
            }
            printf("TEST --> DC: %d\n", DC);
            memory[DC + IC] = '\0';
            memoryLines[DC + IC].type = data;
            memoryLines[DC + IC].value = '\0';
            DC++;
        }
        else
        {
            handleError("Invalid string directive", lineNum, line);
        }
    }
}

DirectiveType getDirectiveType(char *line)
{
    char *directiveName;

    directiveName = getFirstWord(line);

    printf("TEST --> Checking directive type: %s\n", directiveName);
    if (strcmp(directiveName, ".data") == 0)
    {
        printf("TEST --> Data directive: %s found in line %d <--- Second TEST\n", directiveName, lineNum);
        return DATA_DIRECTIVE;
    }
    if (strcmp(directiveName, ".string") == 0)
    {
        return STRING_DIRECTIVE;
    }
    if (strcmp(directiveName, ".entry") == 0)
    {
        return ENTRY_DIRECTIVE;
    }
    if (strcmp(directiveName, ".extern") == 0)
    {
        return EXTERN_DIRECTIVE;
    }
    else
    {
        return INVALID_DIRECTIVE;
    }
}

void processExternDirective(char *line)
{

    char *token;
    token = strtok(line + 7, ",");
    printf("TEST --> Processing extern directive\n");
    printf("TEST --> Line exten directive: %s\n", line);
    while (token != NULL)
    {
        Symbol *symbol;
        printf("TEST --> Token: %s\n", token);
        trimLine(token);

        symbol = lookupSymbol(token);
        if (symbol)
        {
            printf("TEST --> Symbol Found: %s\n", symbol->symbolName);
        }
        else
        {
            printf("TEST --> Symbol Not Found: %s\n", token);
            addSymbol(token, external, 0);
        }
        token = strtok(NULL, ",");
    }
}
/* ------ end LINE_DIRECTIVE code  */

/* ------ start LINE_INSTRUCTION code ------ */
/*
* TODO: Implement the following functions:
* - isInstruction
* - processInstruction
* - isValidInstruction

* isInstruction: Check if the line is an instruction
* processInstruction: Process the instruction
    - Check if the instruction is valid
    - Parse the line to extract the instruction name and operands
    - Decode the operands
    - Generate the binary code of the instruction
    - Update IC
* isValidInstruction: Validate the instruction
    - Parse the line to extract the instruction name and operands
    - Check if the instruction name is valid
    - Check if the operands are valid

*/

/*
 * TODO: robust check
 * ! check if the operand is a valid register
 * ! check if the operand is a valid number
 * ! check if the operand is a valid label
 * ! check if the operand is a valid index
 * ! check if the operand is a valid immediate value
 * ! check if the operand is a valid direct value
 * ! check if the operand is a valid register value
 * ! check if the operand is a valid register index
 * ! check if the operand is a valid register direct
 * ! check if the operand is a valid register immediate
 * ! check for commas
 */

int isInstruction(char *line)
{
    char *instructionName;
    int i;

    instructionName = getFirstWord(line);

    printf("TEST --> Checking if instruction\n");
    for (i = 0; i < CMD_NUM; i++)
    {
        if (strcmp(instructionName, commandTable[i].cmdName) == 0)
        {
            printf("TEST --> Instruction found: %s\n", commandTable[i].cmdName);
            return 1;
        }
    }
    return 0;
}

void processInstruction(char *line)
{
    Instruction instruction;
    L = 0;
    memset(&instruction, 0, sizeof(instruction));
    if ((parseInstruction(line, &instruction)) != NULL)
    {

        printf("TEST --> Valid instruction in processInstruction\n");
        printIstruction(&instruction);
        memory[IC++] = instruction.opcode;
        L = 1;
        /* Decode the instruction's operands and calculate L */
        L += decodeOperands(instruction.operands);
        printf("TEST --> L: %d\n", L);

        printf("TEST --> IC: %d\n", IC);
    }
    else
    {
        handleError("TEST --> Invalid instruction", lineNum, line);
    }
}

Instruction *parseInstruction(char *line, Instruction *instruction)
{
    char *buffer = strdup(line);
    if (!buffer)
    {
        return NULL;
    }
    if (isValidInstruction(buffer))
    {

        char *token = strtok(buffer, " \t");
        int i = 0;
        printf("TEST --> Parsing instruction\n");
        if (!token)
        {
            free(buffer);
            return NULL; /* code */
        }

        instruction->name = strdup(token);
        if (!instruction->name)
        {
            free(buffer);
            return NULL;
        }

        instruction->opcode = getOpcode(token);
        token = strtok(NULL, " \t,");
        printf("TEST --> Parsing instruction again\n");

        while (token && i < MAX_OPERANDS)
        {
            printf("TEST --> Token of instruction: %s\n", token);
            instruction->operands[i] = strdup(token);
            if (!instruction->operands[i])
            {
                int j;
                for (j = 0; j < i; j++)
                {
                    free(instruction->operands[j]);
                }

                free(instruction->name);
                free(buffer);
                return NULL;
            }
            i++;
            token = strtok(NULL, " \t,");
        }
        printf("TEST --> Instruction name: %s\n", instruction->name);
        printf("TEST --> Opcode: %d\n", instruction->opcode);
        printf("TEST --> Operand 1: %s\n", instruction->operands[0]);
        printf("TEST --> Operand 2: %s\n", instruction->operands[1]);
        printf("TEST --> Number of operands: %d\n", i);
        free(buffer);

        return instruction;
    }
    else
    {
        handleError("TEST --> Invalid instruction", lineNum, line);
        return NULL;
    }
}

int isValidInstruction(char *line)
{
    char *instructionName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    char *operands = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    int i;

    printf("TEST --> Validating instruction\n");

    /* Parse the line to extract the instruction name and operands */
    sscanf(line, "%s %[^\n]", instructionName, operands);

    printf("TEST --> Instruction name: %s\n", instructionName);
    printf("TEST --> Operands: %s\n", operands);

    /* Check if the instruction name is valid */
    for (i = 0; i < CMD_NUM; i++)
    {
        if (strcmp(instructionName, commandTable[i].cmdName) == 0)
        {
            printf("TEST --> Instruction found: %s\n", commandTable[i].cmdName);
            free(instructionName);
            free(operands);
            return 1;
        }
    }
    free(instructionName);
    free(operands);
    return 0;
}

int decodeOperands(char *operands[])
{
    int totalMemoryLines = 0;
    int isSrcReg = 0;
    int i;
    char *symbolName, *start, *end;
    char index[256];
    char *copy;
    Addressing addrMethod;

    for (i = 0; i < MAX_OPERANDS; i++)
    {
        if (operands[i] == NULL)
        {
            continue;
        }
        addrMethod = getAddressingMethod(operands[i]);
        switch (addrMethod)
        {
        case IMMEDIATE:
            printf("Operand %d (%s) uses Immediate addressing IC = %d\n", i, operands[i], IC);
            memory[IC] = atoi(operands[i] + 1);
            memoryLines[IC].type = data;
            memoryLines[IC].value = atoi(operands[i] + 1);
            IC++;

            printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);

            totalMemoryLines += 1;
            break;
        case DIRECT:
            printf("Operand %d (%s) uses Direct addressing IC = %d\n", i, operands[i], IC);
            memory[IC] = -1;
            memoryLines[IC].type = data;
            memoryLines[IC].value = -1;
            IC++;

            printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);
            totalMemoryLines += 1;
            break;
        case INDEX:

            copy = strdup(operands[i]);
            symbolName = strtok(copy, "[");
            start = strchr(operands[i], '[');
            end = strchr(operands[i], ']');
            printf("Symbol name in index: %s\n", symbolName);

            printf("Operand %d (%s) uses Index addressing IC = %d\n", i, operands[i], IC);
            if (lookupSymbol(symbolName) == NULL)
            {
                memory[IC] = -1;
                memoryLines[IC].type = data;
                memoryLines[IC].value = -1;
                IC++;
            }
            else
            {
                memory[IC] = lookupSymbol(symbolName)->value;
                memoryLines[IC].type = data;
                memoryLines[IC].value = lookupSymbol(symbolName)->value;
                IC++;
            }

            if (start && end && (end > start))
            {
                int length = end - start - 1;
                if (length < 255)
                {
                    strncpy(index, start + 1, length);
                    index[length] = '\0';

                    if (isNumeric(index))
                    {
                        memory[IC] = atoi(index);
                        memoryLines[IC].type = data;
                        memoryLines[IC].value = atoi(index);
                        IC++;
                    }
                    else if (lookupSymbol(index) == NULL)
                    {
                        memory[IC] = -1;
                        memoryLines[IC].type = data;
                        memoryLines[IC].value = -1;
                        IC++;
                    }
                    else
                    {
                        memory[IC] = lookupSymbol(index)->value;
                        memoryLines[IC].type = data;
                        memoryLines[IC].value = lookupSymbol(index)->value;
                        IC++;
                    }
                }
            }

            printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);

            totalMemoryLines += 2;
            free(copy);
            break;
        case REGISTER:
            printf("Operand %d (%s) uses Register addressing IC = %d\n", i, operands[i], IC);
            if (isSrcReg == 0)
            {
                memory[IC] = atoi(operands[i] + 1);
                memoryLines[IC].type = data;
                memoryLines[IC].value = atoi(operands[i] + 1);
                IC++;
                printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);

                totalMemoryLines += 1;
                isSrcReg = 1;
            }
            else
            {
                memory[IC] += atoi(operands[i] + 1) << 2;
                memoryLines[IC].type = data;
                memoryLines[IC].value = atoi(operands[i] + 1) << 2;
                printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);
                totalMemoryLines += 0;
            }
            break;
        case INVALID:
            printf("Operand %d (%s) uses Invalid addressing IC = %d\n", i, operands[i], IC);
            break;
        }
    }
    printf("TEST --> Total memory lines: %d\n", totalMemoryLines);
    return totalMemoryLines;
}

Addressing getAddressingMethod(char *operand)
{
    if (operand[0] == '#')
    {
        printf("TEST --> Immediate\n");
        return IMMEDIATE;
    }
    if (strchr(operand, '[') && strchr(operand, ']'))
    {
        printf("TEST --> Index\n");
        return INDEX;
    }
    if (operand[0] == 'r' && isdigit(operand[1]))
    {
        printf("TEST --> Register\n");
        return REGISTER;
    }
    printf("TEST --> Direct\n");
    return DIRECT;
}

/* end LINE_INSTRUCTION code */

/* start LINE_DIRECTIVE code */

/* end LINE_DIRECTIVE code */