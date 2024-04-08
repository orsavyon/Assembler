#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"
#include "first_pass.h"
#include "data.h"

void firstPass(FILE *fp)
{

    char line[MAX_LINE_LENGTH];
    int IC = 0; /* Initialize IC to 0 */
    int DC = 0; /* Initialize DC to 0 */

    initData();
    initSymbolTable();

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
                    if (getDirectiveType(remainingLine) == ENTRY_DIRECTIVE || getDirectiveType(remainingLine) == STRING_DIRECTIVE)
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
                        addSymbol(symbolName, code, IC + 100);
                    }
                    else
                    {
                        handleError("TEST --> Symbol already defined", lineNum, line);
                    }
                }
                else if (getLineType(remainingLine) == LINE_INSTRUCTION)
                {
                    printf("TEST --> Instruction in label\n");
                    processInstruction(remainingLine);
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
            break;

        case INVALID_LINE:
            handleError("TEST --> Invalid line", lineNum, line);
            break;
        default:
            break;
        }
        symbolFlag = 0;
        /*

        // Decode the instruction's operands and calculate L
        int L = decodeOperands(line);

        // Generate the binary code of the instruction
        generateBinaryCode(line);

        // Update IC to IC + L
        IC += L;
        */
    }
    printSymbolTable();
    printMemory();

    /* Update the value of each symbol characterized as data in the symbol table by adding IC + 100 */
    /*updateSymbolValues(IC + 100); */

    /* Start the second pass */
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
    printf("TEST --> Processing data directive\n");
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
                memory[DC++] = symbol->value;
            }
            else if (isdigit(token[0]) || token[0] == '-' || token[0] == '+')
            {
                memory[DC++] = atoi(token);
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
        char *start = strchr(line, '"') + 1;
        char *end = strrchr(line, '"');
        if (start && end && start < end)
        {
            char *c = start;
            for (; c < end; c++)
            {
                memory[DC++] = (unsigned char)*c;
            }
            memory[DC++] = '\0';
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
        return DATA_DIRECTIVE;
    }
    if (strcmp(directiveName, ".string") == 0)
    {
        return STRING_DIRECTIVE;
    }
    else
    {
        return INVALID_DIRECTIVE;
    }
}

void processExternDirective(char *line)
{
    printf("TEST --> Processing extern directive\n");
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
    if (isValidInstruction(line))
    {
        printf("TEST --> Valid instruction\n");
    }
    else
    {
        handleError("TEST --> Invalid instruction", lineNum, line);
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
            free(instructionName);
            free(operands);
            return 1;
        }
    }
    free(instructionName);
    free(operands);
    return 0;
}

/* end LINE_INSTRUCTION code */

/* start LINE_DIRECTIVE code */

/* end LINE_DIRECTIVE code */