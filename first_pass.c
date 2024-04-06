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
        printf("Line %d: %s\n", lineNum, line);

        if (isSymbol(line))
        {
            printf("Has symbol\n");
            symbolFlag = 1;
        }
        switch (getLineType(line))
        {
        case LINE_BLANK:
            break;
        case LINE_COMMENT:
            break;

        case LINE_DEFINITION:
            printf("Definition\n");
            processDefinition(line);
            break;

        case LINE_INSTRUCTION:
            printf("Instruction\n");
            processInstruction(line);
            break;

        case INVALID_LINE:
            handleError("Invalid line", lineNum, line);
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
    return firstWord;
}

LineType getLineType(char *line)
{
    if (line[0] == '\n' || line[0] == '\0' || line[0] == '\r' || line[0] == '\t' || line[0] == ' ')
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
    if (isInstruction(line))
    {
        return LINE_INSTRUCTION;
    }

    return INVALID_LINE;
}

int isSymbol(char *line)
{
    int i;
    printf(" --> Checking if symbol\n");
    for (i = 0; i < strlen(line); i++)
    {
        if (line[i] == ':')
        {
            return 1;
        }
    }
    return 0;
}
/* end HELPERS code */

/* ------ start LINE_DEFINITION code ------*/

int isConstantDefinition(char *line)
{
    printf(" --> Checking if constant definition\n");
    return strcmp(getFirstWord(line), ".define") == 0;
}

void processDefinition(char *line)
{
    if (isValidConstantDefinition(line))
    {
        char *constantName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
        int value;

        sscanf(line, ".define %[^=]=%d", constantName, &value);
        addSymbol(constantName, MDEFINE, value);
        printf("Valid constant definition\n");
    }
    else
    {
        handleError("Invalid constant definition", lineNum, line);
    }
}

int isValidConstantDefinition(char *line)
{
    char *constantName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    int value;

    printf("Validating constant definition\n");
    /* Parse the line to extract the constant name and value */
    if (strstr(line, "=") == NULL)
    {
        handleError("Invalid constant definition: Missing equal sign", lineNum, line);
        return 0;
    }

    sscanf(line, ".define %[^=]=%d", constantName, &value);

    printf("Constant name: %s\n", constantName);
    printf("Value: %d\n", value);

    /* Trim leading and trailing white spaces from the constant name and value */
    /*trim(constantName);*/
    /*trimValue(line);*/

    /* Check if the constant name is already defined */
    if (lookupSymbol(constantName) != NULL)
    {
        handleError("Symbol is already defined", lineNum, line);
        free(constantName);
        return 0;
    }

    /* Convert the value to an integer */

    /* Check if the value is within the range of a 12-bit integer */
    if (value < -2048 || value > 2047)
    {
        handleError("Value is out of range", lineNum, line);
        free(constantName);
        return 0;
    }

    free(constantName);

    return 1;
}

/* ------ end LINE_DEFINITION code ------ */

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
    int i;
    printf(" --> Checking if instruction\n");
    for (i = 0; i < CMD_NUM; i++)
    {
        if (strcmp(getFirstWord(line), commandTable[i].cmdName) == 0)
        {
            printf("Instruction found: %s\n", commandTable[i].cmdName);
            return 1;
        }
    }
    return 0;
}

void processInstruction(char *line)
{
    if (isValidInstruction(line))
    {
        printf("Valid instruction\n");
    }
    else
    {
        handleError("Invalid instruction", lineNum, line);
    }
}

int isValidInstruction(char *line)
{
    char *instructionName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    char *operands = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    int i;

    printf("Validating instruction\n");

    /* Parse the line to extract the instruction name and operands */
    sscanf(line, "%s %[^\n]", instructionName, operands);

    printf("Instruction name: %s\n", instructionName);
    printf("Operands: %s\n", operands);

    /* Check if the instruction name is valid */
    for (i = 0; i < CMD_NUM; i++)
    {
        if (strcmp(instructionName, commandTable[i].cmdName) == 0)
        {
            return 1;
            free(instructionName);
            free(operands);
        }
    }
    free(instructionName);
    free(operands);
    return 0;
}

/* end LINE_INSTRUCTION code */

/* start LINE_DIRECTIVE code */

/* end LINE_DIRECTIVE code */