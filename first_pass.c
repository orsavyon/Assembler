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
 * - encode word for instruction
 *
 */

void firstPass(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    IC = 0;
    DC = 0;

    /* Initialize data structures */
    initData();
    initSymbolTable();
    initMemoryLines();

    printf("\n --- in firstPass --- \n\n");
    /* Process each line of the source file */
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        lineNum++;

        /* Check if line exceeds the limit */
        if (strlen(line) == MAX_LINE_LENGTH - 1 && line[MAX_LINE_LENGTH - 2] != '\n')
        {
            handleError("Line length exceeds the limit", lineNum, line);
            continue;
        }

        /* Trimming line to remove possible trailing whitespaces */
        trimLine(line);
        printf("TEST --> Line %d: %s\n", lineNum, line);

        /* Determine the type of the line and process accordingly */
        switch (getLineType(line))
        {
        case LINE_BLANK:
        case LINE_COMMENT:
            /* Ignore blank and comment lines */
            break;

        case LINE_DEFINITION:
            printf("TEST --> Definition\n");
            /* Line detected with .define - Process constant definitions */
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

                if (getLineType(remainingLine) == LINE_BLANK)
                {
                    handleError("Missing instruction/action after label", lineNum, line);
                }
                else if (getLineType(remainingLine) == LINE_DIRECTIVE)
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
                    if (errorFlag == 0)
                    {
                        printf("TEST --> Processing directive in label\n");
                        processDirective(remainingLine);
                    }
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
                    if (errorFlag == 0 && getLineType(remainingLine) == LINE_INSTRUCTION)
                    {
                        printf("TEST --> Instruction in label\n");

                        processInstruction(remainingLine);
                        printf("TEST --> IC after label instruction: %d\n", IC);
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
            handleError("ERROR --> Invalid line", lineNum, line);
            break;
        default:
            break;
        }
        symbolFlag = 0;
    }
    updateSymbolValues();
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

/* ############################### start HELPERS code ############################### */

/**
 * Implements the getFirstWord function declared in first_pass.h.
 */
char *getFirstWord(const char *line)
{
    static char firstWord[MAX_LINE_LENGTH]; /* Static buffer to hold the first word */
    int i = 0;                              /* Index for placing characters into firstWord */

    /* Skip leading whitespace */
    while (isspace((unsigned char)*line))
    {
        line++;
    }
    /* Capture the first word until a space or string end is encountered */
    while (*line && !isspace((unsigned char)*line) && i < MAX_LINE_LENGTH - 1)
    {
        firstWord[i++] = *line++;
    }

    firstWord[i] = '\0';                            /* Null-terminate the extracted word */
    printf("TEST --> First word: %s\n", firstWord); /* Output the first word for debugging */
    return firstWord;                               /* Return the static buffer containing the first word */
}

/* Determines the type of a line */
LineType getLineType(char *line)
{

    /* Check if the line is blank */
    if (line[0] == '\n' || line[0] == '\0' || isspace((unsigned char)line[0]))
    {
        return LINE_BLANK;
    }
    /* Check if the line is a comment */
    if (line[0] == ';')
    {
        return LINE_COMMENT;
    }
    /* Check if the line is a directive */
    if (line[0] == '.')
    {
        DirectiveType directiveType = getDirectiveType(line);
        if (directiveType == INVALID_DIRECTIVE)
        {
            return INVALID_LINE;
        }
        if (directiveType == DEFINE_DIRECTIVE)
        {
            return LINE_DEFINITION;
        }
        else
        {
            return LINE_DIRECTIVE;
        }
    }

    /* Check if the line contains a symbol */
    if (isLabel(line))
    {
        return LINE_LABEL;
    }

    /* Check if the line represents an instruction */
    if (isInstruction(line))
    {
        return LINE_INSTRUCTION;
    }
    /* If none of the above conditions are met, the line is invalid */
    return INVALID_LINE;
}

/**
 * Implements the isLabel function declared in first_pass.h.
 */
int isLabel(char *line)
{
    char label[33]; /* Array to hold potential label */
    int i = 0;
    int errLabel = 0;
    char *colonPos = strchr(line, ':');  /* Pointer to the colon character in the line */
    errLabel = colonPos == NULL ? 0 : 1; /* Check if the colon character is present in the line */

    /* Ensure the first character is alphabetic */
    if (!isalpha((unsigned char)line[0]))
    {
        if (errLabel == 1)
        {
            handleError("ERROR --> Invalid label: Label must start with an alphabetic character", lineNum, line);
        }
        return 0;
    }

    /* Collect label characters */
    while (isalnum((unsigned char)line[i]) && i < 31)
    { /* Check for alphanumeric and limit length to 31 */
        label[i] = line[i];
        i++;
    }

    /* Check for the ':' immediately after the alphanumeric characters */
    if (line[i] != ':' || (i > 0 && line[i - 1] == ' '))
    {
        if (errLabel == 1)
        {
            handleError("ERROR --> Invalid label: Label must end with a colon", lineNum, line);
        }
        return 0;
    }

    label[i] = '\0'; /* Null-terminate the label string */

    /* Check if the label is a reserved word */
    if (isReservedWord(label))
    {
        if (errLabel == 1)
        {
            handleError("ERROR --> Invalid label: Label cannot be a reserved word", lineNum, line);
        }
        return 0;
    }
    /* If all checks are passed */
    return 1;
}

/* ############################### end HELPERS code ############################### */
/* ############################################################################################# */
/* ############################### start LINE_DEFINITION code ############################### */

/**
 * Implements the processDefinition function declared in first_pass.h.
 */

void processDefinition(char *line)
{
    if (isValidConstantDefinition(line))
    {
        /* Allocate memory for constant name with size based on MAX_LINE_LENGTH */
        char *constantName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
        int value;

        /* Parse the line to extract constant name and its integer value */
        sscanf(line, ".define %[^=]=%d", constantName, &value);
        trimLine(constantName);                  /* Remove any leading or trailing spaces from constant name */
        addSymbol(constantName, mdefine, value); /* Add the constant name and value to the symbol table */
        printf("TEST --> Valid constant definition\n");
    }
    else
    {
        /* Handle the error if the definition line is invalid */
        handleError("Invalid constant definition", lineNum, line);
    }
}

/**
 * Implements the isValidConstantDefinition function declared in first_pass.h.
 */
int isValidConstantDefinition(char *line)
{
    char lineCopy[MAX_LINE_LENGTH]; /* Buffer to hold a copy of the line */
    char *constantPart, *valuePart;
    int value;
    char *start;

    printf("TEST --> Checking constant definition\n");
    /* Copy the line to prevent modification of the original */
    strncpy(lineCopy, line, MAX_LINE_LENGTH);
    lineCopy[MAX_LINE_LENGTH - 1] = '\0'; /* Ensure null termination */
    start = lineCopy;

    if (strstr(start, ".define ") == start)
    {
        start += strlen(".define "); /*Move past ".define " and any space right after it*/
    }
    else
    {
        handleError("Error: Line does not start with '.define '", lineNum, line);
        return 0;
    }
    printf("TEST --> Line copy: %s\n", start);

    /* Use trimLine to handle any extraneous spaces at the start or end */
    trimLine(start);

    /* Attempt to split at the first occurrence of '=' */
    constantPart = strtok(start, "=");
    valuePart = strtok(NULL, "");

    if (constantPart == NULL || valuePart == NULL)
    {
        handleError("Invalid constant definition: Missing '=' or incomplete definition", lineNum, line);
        return 0;
    }

    /* Trim both parts to handle spaces around '=' */
    trimLine(constantPart);
    trimLine(valuePart);

    /* Ensure value part is numeric and convert it */
    if (!isNumeric(valuePart))
    {
        handleError("Invalid constant definition: Number format error", lineNum, line);
        return 0;
    }
    value = atoi(valuePart);

    /* Check constant name for validity */
    if (isReservedWord(constantPart) || lookupSymbol(constantPart) != NULL)
    {
        handleError("Invalid constant definition: Reserved word used or symbol already defined", lineNum, line);
        return 0;
    }

    /* Validate the numeric value range */
    if (value < MIN_12BIT_VALUE || value > MAX_12BIT_VALUE)
    {
        handleError("Invalid constant definition: Value out of range", lineNum, line);
        return 0;
    }
    printf("TEST --> Valid constant definition in isValidConstant\n");

    return 1; /* Valid constant definition */
}

/* ############################### end LINE_DEFINITION code ############################### */
/* ############################################################################################# */
/* ############################### start LINE_DIRECTIVE code ############################### */

/**
 * Implements the processDirective function declared in first_pass.h.
 */
void processDirective(char *line)
{
    /* Switch on the type of directive determined by getDirectiveType function */
    switch (getDirectiveType(line))
    {
    case DATA_DIRECTIVE:
        printf("TEST --> Data directive\n");
        processDataDirective(line); /* Call to process data directive */
        /* ! Refactor */
        break;
    case STRING_DIRECTIVE:
        printf("TEST --> String directive\n");
        processDataDirective(line); /* Call to process string directive */
        /* ! Refactor */
        break;
    case DEFINE_DIRECTIVE:
    case ENTRY_DIRECTIVE:
        printf("TEST --> Entry directive\n");
        break;
    case EXTERN_DIRECTIVE:
        if (symbolFlag == 1) /* Check condition based on symbolFlag */
        {
            break;
        }
        else
        {
            printf("TEST --> Extern directive\n");
            processExternDirective(line); /* Call to process extern directive */
            break;
        }
    case INVALID_DIRECTIVE:
        handleError("TEST --> Invalid directive", lineNum, line); /* Handle invalid directive */
        break;
    }
}

/**
 * Implements the processDataDirective function declared in first_pass.h.
 */
void processDataDirective(char *line)
{
    char *token; /* Token for parsing the data elements */
    printf("TEST --> Processing data directive\n\t----Line: %s\n", line);
    /* Check if the line starts with '.data' directive */

    if (strncmp(line, ".data", 5) == 0)
    {
        token = strtok(line + 6, ","); /* Start tokenizing the line after the directive */
        while (token != NULL)
        {
            Symbol *symbol; /* Pointer to a symbol structure */
            printf("TEST --> Token: %s\n", token);
            trimLine(token); /* Trim whitespace around the token */

            symbol = lookupSymbol(token); /* Check if the token is a known symbol */
            if (symbol)
            {
                printf("TEST --> Symbol Found: %s\n", symbol->symbolName);
            }
            else
            {
                printf("TEST --> Symbol Not Found: %s\n", token);
            }
            /* If the token is a defined symbol, store its value in memory */
            if (symbol && symbol->symbolType == mdefine)
            {
                printf("TEST --> Symbol: %s\n", symbol->symbolName);
                printf("TEST --> DC: %d\n", DC);
                memory[DC + IC] = symbol->value;

                memoryLines[DC + IC].value = computeFourteenBitValue(symbol->value);
                DC++;
            }
            else if (isdigit(token[0]) || token[0] == '-' || token[0] == '+') /* If the token is a numeric value, store it directly */
            {
                printf("TEST --> DC: %d\n", DC);

                memory[DC + IC] = atoi(token);

                memoryLines[DC + IC].value = computeFourteenBitValue(atoi(token));

                DC++;
            }
            else /* Handle the error case where the token is neither a defined symbol nor a valid number */
            {
                printf("Error: Undefined symbol or invalid number in .data directive: %s\n", token);
            }
            token = strtok(NULL, ","); /* Continue to the next token */
        }
    }
    else if (strncmp(line, ".string", 7) == 0) /* Handle the '.string' directive */
    {

        char *start = strchr(line, '\"'); /* Find the starting quote of the string */
        char *end = NULL;
        if (!start)
        {
            start = strstr(line, "“"); /* Handle alternative quote character */
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
            end = strchr(start, '\"'); /* Find the ending quote */
            if (!end)
            {
                end = strstr(start, "”");
            }
        }

        printf("TEST --> Processing string directive\n");

        if (start && end && start < end) /* If valid string boundaries are found, process the string */
        {
            char *c;
            printf("TEST --> String: %.*s\n", (int)(end - start), start);

            for (c = start; c < end; c++) /* Store each character of the string in memory */
            {
                printf("TEST --> Char: %c\n", *c);
                printf("TEST --> DC: %d\n", DC);
                memory[DC + IC] = (unsigned char)*c;

                memoryLines[DC + IC].value = computeFourteenBitValue((unsigned char)*c);

                DC++;
            }
            printf("TEST --> DC: %d\n", DC);
            memory[DC + IC] = '\0';

            memoryLines[DC + IC].value = computeFourteenBitValue('\0');
            DC++;
        }
        else /* Handle invalid string directive */
        {
            handleError("Invalid string directive", lineNum, line);
        }
    }
}

/**
 * Implements the getDirectiveType function declared in first_pass.h.
 */
DirectiveType getDirectiveType(char *line)
{
    char *directiveName; /* Pointer to store the first word from the line */

    directiveName = getFirstWord(line); /* Retrieve the first word from the line */

    printf("TEST --> Checking directive type: %s\n", directiveName);

    /* Check for '.data' directive and return corresponding type */
    if (strcmp(directiveName, ".data") == 0)
    {
        printf("TEST --> Data directive: %s found in line %d <--- Second TEST\n", directiveName, lineNum);
        return DATA_DIRECTIVE;
    }
    /* Check for '.string' directive and return corresponding type */
    if (strcmp(directiveName, ".string") == 0)
    {
        return STRING_DIRECTIVE;
    }
    /* Check for '.entry' directive and return corresponding type */
    if (strcmp(directiveName, ".entry") == 0)
    {
        return ENTRY_DIRECTIVE;
    }
    /* Check for '.extern' directive and return corresponding type */
    if (strcmp(directiveName, ".extern") == 0)
    {
        return EXTERN_DIRECTIVE;
    }
    if (strcmp(directiveName, ".define") == 0)
    {
        return DEFINE_DIRECTIVE;
    }
    /* If none of the known directives match, return invalid directive type */
    else
    {
        handleError("ERROR --> Invalid directive", lineNum, line);
        return INVALID_DIRECTIVE;
    }
}

/**
 * Implements the processExternDirective function declared in first_pass.h.
 */
void processExternDirective(char *line)
{

    char *token;                   /* Token for parsing symbols from the directive */
    token = strtok(line + 7, ","); /* Begin tokenizing after the directive keyword */
    printf("TEST --> Processing extern directive\n");
    printf("TEST --> Line exten directive: %s\n", line);

    /* Iterate through tokens representing symbols */
    while (token != NULL)
    {
        Symbol *symbol; /* Pointer to a symbol structure */
        printf("TEST --> Token: %s\n", token);
        trimLine(token); /* Trim whitespace around the token */

        symbol = lookupSymbol(token); /* Check if the symbol is already in the table */
        if (symbol)
        {
            printf("TEST --> Symbol Found: %s\n", symbol->symbolName); /* Existing symbol found */
        }
        else
        {
            printf("TEST --> Symbol Not Found: %s\n", token); /* New external symbol, add it to the table */
            addSymbol(token, external, 0);
        }
        token = strtok(NULL, ","); /* Continue to the next token */
    }
}
/* ############################### end LINE_DIRECTIVE code ############################### */

/* ############################### start LINE_INSTRUCTION code ############################### */

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

/**
 * Implements the isInstruction function declared in first_pass.h.
 */
int isInstruction(char *line)
{
    char *instructionName; /* Variable to store the first word of the line */
    int i;

    instructionName = getFirstWord(line); /* Retrieve the first word from the line */

    printf("TEST --> Checking if instruction\n");
    /* Loop through the command table to check for a match */
    for (i = 0; i < CMD_NUM; i++)
    {
        if (strCaseCmp(instructionName, commandTable[i].cmdName) == 0) /* Case-insensitive comparison */
        {
            if (strcmp(instructionName, commandTable[i].cmdName) != 0) /* Case-sensitive comparison */
            {
                printf("ERROR --> Instruction case mismatch: '%s' should be '%s'\n", instructionName, commandTable[i].cmdName);
            }
            printf("TEST --> Instruction found: %s\n", commandTable[i].cmdName);
            return 1; /* Return 1 if a match is found indicating an instruction */
        }
    }
    return 0; /* Return 0 if no match is found */
}

/**
 * Implements the processInstruction function declared in first_pass.h.
 */
void processInstruction(char *line)
{
    Instruction instruction;                /* Struct to store parsed instruction details */
    Word *firstWord = malloc(sizeof(Word)); /* Allocate memory for the first word of the instruction */
    if (!firstWord)
    {
        handleError("Memory allocation failed", lineNum, line); /* Handle memory allocation failure */
        return;
    }
    L = 0; /* Initialize the line count for this instruction */

    memset(&instruction, 0, sizeof(instruction)); /* Zero out the instruction struct */

    if ((parseInstruction(line, &instruction)) != NULL) /* Parse the instruction from the line */
    {

        printf("TEST --> Valid instruction in processInstruction\n");
        printIstruction(&instruction); /* Print the parsed instruction */

        setupFirstInstructionWord(firstWord, &instruction); /* Setup the first word based on the parsed instruction */

        memoryLines[IC].word = malloc(sizeof(Word)); /* Allocate memory for storing the instruction in memory lines */
        if (!memoryLines[IC].word)
        {
            handleError("Memory allocation failed", lineNum, line); /* Handle memory allocation failure */
            return;
        }
        else
        {
            memoryLines[IC].word = firstWord;              /* Assign the first word to the current instruction counter in memory lines */
            memoryLines[IC].type = INSTRUCTION_ADDRESSING; /* Set the memory line type to instruction addressing */
            memoryLines[IC].value = firstWord->value;      /* Set the value of the memory line to the first word's value */
            printWordAsBinary(*firstWord);                 /* Print the first word as binary for debugging */
        }

        memory[IC] = firstWord->bits.opcode; /* Store the opcode in the main memory at the current instruction counter */

        IC++; /* Increment the instruction counter */

        L = 1;                                     /* Set line count for the instruction */
        L += decodeOperands(instruction.operands); /* Decode and add operand sizes to L */
        printf("TEST --> L: %d\n", L);

        printf("TEST --> IC: %d\n", IC);
    }
    else
    {
        if (errorFlag == 0)
        {
            handleError("TEST --> Invalid instruction", lineNum, line); /* Handle invalid instruction format */
        }
    }
}

/**
 * Implements the isValidInstruction function declared in first_pass.h.
 */
int isValidInstruction(char *line)
{
    char *instructionName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char)); /* Allocate memory for instruction name */
    char *operands = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));        /* Allocate memory for storing operands */
    int i;

    printf("TEST --> Validating instruction\n");

    /* Parse the line to extract the instruction name and its operands */
    sscanf(line, "%s %[^\n]", instructionName, operands);

    printf("TEST --> Instruction name: %s\n", instructionName);
    printf("TEST --> Operands: %s\n", operands);

    /* Loop through the command table to check if the instruction name is valid */
    for (i = 0; i < CMD_NUM; i++)
    {
        if (strcmp(instructionName, commandTable[i].cmdName) == 0)
        {
            printf("TEST --> Instruction found: %s\n", commandTable[i].cmdName);
            free(instructionName); /* Free allocated memory for instruction name */
            free(operands);        /* Free allocated memory for operands */
            return 1;              /* Return 1 if a valid instruction name is found */
        }
    }
    free(instructionName); /* Free allocated memory if no valid instruction is found */
    free(operands);        /* Free allocated memory if no valid instruction is found */
    return 0;              /* Return 0 if no valid instruction name is found */
}

/**
 * Implements the setupFirstInstructionWord function declared in first_pass.h.
 */
void setupFirstInstructionWord(Word *firstWord, Instruction *instruction)
{
    int srcAddressing = 0;                        /* Variable to store the source addressing method */
    int destAddressing = 0;                       /* Variable to store the destination addressing method */
    memset(firstWord, 0, sizeof(Word));           /* Initialize the firstWord struct to zero */
    firstWord->bits.opcode = instruction->opcode; /* Set the opcode in the first word */
    firstWord->bits.ARE = 0;                      /* Set the ARE bits to 0 (Absolute by default) */

    /* Check if the instruction has operands and determine the addressing method for destination */
    if (instruction->operands[0] && commandTable[instruction->opcode].numOfOps >= 1)
    {
        destAddressing = getAddressingMethod(instruction->operands[0]);
    }

    /* Set the addressing modes based on the number of operands the command expects */
    switch (commandTable[instruction->opcode].numOfOps)
    {
    case 0:
        firstWord->bits.srcOp = 0; /* No source operand */
        firstWord->bits.desOp = 0; /* No destination operand */
        break;
    case 1:
        if (isValidAddressingMode(destAddressing, commandTable[instruction->opcode].destLegalAddrs))
        {
            firstWord->bits.desOp = destAddressing; /* Set destination operand addressing mode */
        }
        else
        {
            handleError("Invalid addressing mode for destination", lineNum, instruction->operands[0]); /* Handle invalid destination addressing mode */
        }
        firstWord->bits.srcOp = 0; /* No source operand */
        break;
    case 2:
        srcAddressing = getAddressingMethod(instruction->operands[0]);  /* Determine source addressing method */
        destAddressing = getAddressingMethod(instruction->operands[1]); /* Determine destination addressing method */
        if (isValidAddressingMode(srcAddressing, commandTable[instruction->opcode].srcLegalAddrs) &&
            isValidAddressingMode(destAddressing, commandTable[instruction->opcode].destLegalAddrs))
        {
            firstWord->bits.srcOp = srcAddressing;  /* Set source operand addressing mode */
            firstWord->bits.desOp = destAddressing; /* Set destination operand addressing mode */
        }
        else
        {
            handleError("Invalid addressing modes for operands", lineNum, "line"); /* Handle invalid operand addressing modes */
        }
        break;
    }
}

/**
 * Implements the isValidAddressingMode function declared in first_pass.h.
 */
int isValidAddressingMode(int mode, int allowedModes[])
{
    int i;
    /* Loop through the array of allowed addressing modes */
    for (i = 0; i < 4; i++)
    {
        if (mode == allowedModes[i])
        {
            return 1; /* Return 1 if the mode matches an allowed mode */
        }
    }
    return 0; /* Return 0 if no match is found, indicating the mode is not valid */
}

/**
 * Implements the parseInstruction function declared in first_pass.h.
 */
Instruction *parseInstruction(char *line, Instruction *instruction)
{
    char *buffer = strdup(line); /* Duplicate the line for manipulation */
    if (!buffer)
    {
        return NULL; /* Return NULL if memory allocation fails */
    }
    if (isValidInstruction(buffer))
    {
        int i = 0;
        int expectedOperands;
        int operandCount = 0;
        char *operands;
        char *token = strtok(buffer, " \t"); /* Tokenize the line to extract the instruction name */
                                             /* Counter for operands */
        printf("TEST --> Parsing instruction\n");
        if (!token)
        {
            free(buffer);
            return NULL; /* Return NULL if the first token (instruction name) is missing */
        }

        instruction->name = strdup(token); /* Store the instruction name */
        if (!instruction->name)
        {
            free(buffer);
            return NULL; /* Return NULL if memory allocation fails for the instruction name */
        }

        instruction->opcode = getOpcode(token); /* Set the opcode based on the instruction name */

        expectedOperands = commandTable[instruction->opcode].numOfOps; /* Get the number of expected operands */
        operands = strtok(NULL, "");

        if (operands)
        {
            token = strtok(operands, ","); /* Tokenize the operands */
            while (token && operandCount < MAX_OPERANDS)
            {
                trimLine(token); /* Trim whitespace around the token */

                if (*token == '\0')
                {
                    handleError("Invalid operand", lineNum, line);
                    return NULL;
                }

                instruction->operands[operandCount] = strdup(token); /* Store the operand */

                if (!instruction->operands[operandCount])
                {
                    handleError("Memory allocation failed", lineNum, line);
                    return NULL; /* Return NULL if memory allocation fails for an operand */
                }
                operandCount++;
                token = strtok(NULL, ","); /* Continue to the next operand */
            }
        }

        if (operandCount != expectedOperands)
        {
            handleError("Invalid number of operands", lineNum, line);
            return NULL;
        }

        printf("TEST --> Instruction name: %s\n", instruction->name);
        printf("TEST --> Opcode: %d\n", instruction->opcode);
        printf("TEST --> Operand 1: %s\n", instruction->operands[0]);
        printf("TEST --> Operand 2: %s\n", instruction->operands[1]);
        printf("TEST --> Number of operands: %d\n", i);
        free(buffer);
        return instruction; /* Return the filled instruction struct */
    }
    else
    {
        handleError("Invalid instruction format", lineNum, line);
        return NULL; /* Return NULL if the instruction is not valid */
    }
}

/**
 * Implements the decodeOperands function declared in first_pass.h.
 */
int decodeOperands(char *operands[])
{
    Word word;                      /* Temporary storage for operand values */
    int value;                      /* Numeric value of an operand */
    int totalMemoryLines = 0;       /* Counter for memory lines consumed */
    int isSrcReg = 0;               /* Flag to handle source register in dual operand instructions */
    int i;                          /* Loop counter */
    char *symbolName, *start, *end; /* Pointers for handling indexed addressing */
    char index[256];                /* Buffer for index in indexed addressing */
    char *copy;                     /* Copy of operand for manipulation */
    Addressing addrMethod;          /* Addressing method of current operand */

    for (i = 0; i < MAX_OPERANDS; i++)
    {
        if (operands[i] == NULL)
        {
            continue; /* Skip processing if the operand is NULL */
        }
        addrMethod = getAddressingMethod(operands[i]); /* Determine the addressing method */
        switch (addrMethod)
        {
        case IMMEDIATE:
            /* Immediate value may be a numeric or a symbol value */
            if (lookupSymbol(operands[i] + 1) != NULL)
            {
                value = lookupSymbol(operands[i] + 1)->value;
            }
            else
            {
                value = atoi(operands[i] + 1);
            }
            memset(&word, 0, sizeof(word)); /* Clear the word struct */

            setImmediateValue(&word, value, 0); /* Set the immediate value */

            printf("Operand %d (%s) uses Immediate addressing IC = %d\n", i, operands[i], IC);
            printf("TEST --> Encoded Word: ");
            printWordAsBinary(word); /* Output the binary format of the word */

            memory[IC] = atoi(operands[i] + 1); /* Store value directly in memory */

            memoryLines[IC].word->value = word.value;
            memoryLines[IC].type = IMMEDIATE_ADDRESSING;
            memoryLines[IC].value = value;
            IC++; /* Increment instruction counter */

            printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);

            totalMemoryLines += 1; /* Increment total memory lines used */
            break;
        case DIRECT:
            printf("Operand %d (%s) uses Direct addressing IC = %d\n", i, operands[i], IC);
            memory[IC] = -1; /* Placeholder for future linking */
            memoryLines[IC].type = DIRECT_ADDRESSING;
            memoryLines[IC].needEncoding = 1;
            memoryLines[IC].symbol = strdup(operands[i]);
            memoryLines[IC].value = -1;
            IC++; /* Increment instruction counter */

            printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);
            totalMemoryLines += 1; /* Increment total memory lines used */
            break;
        case INDEX:
            /* Handle indexed addressing */
            copy = strdup(operands[i]);
            symbolName = strtok(copy, "[");
            start = strchr(operands[i], '[');
            end = strchr(operands[i], ']');
            printf("Symbol name in index: %s\n", symbolName);

            printf("Operand %d (%s) uses Index addressing IC = %d\n", i, operands[i], IC);
            if (lookupSymbol(symbolName) == NULL)
            {
                memory[IC] = -1; /* Placeholder for future linking */
                memoryLines[IC].type = INDEX_ADDRESSING;
                memoryLines[IC].needEncoding = 1;
                memoryLines[IC].symbol = strdup(symbolName);
                memoryLines[IC].value = -1;
                IC++; /* Increment instruction counter */
            }
            else
            {
                memory[IC] = lookupSymbol(symbolName)->value;
                memoryLines[IC].type = INDEX_ADDRESSING_VALUE;
                memoryLines[IC].value = lookupSymbol(symbolName)->value;
                IC++; /* Increment instruction counter */
            }
            /* Handle the numeric index within brackets */
            if (start && end && (end > start))
            {
                int length = end - start - 1;
                if (length < 255)
                {
                    strncpy(index, start + 1, length);
                    index[length] = '\0';

                    if (isNumeric(index))
                    {
                        value = atoi(index);
                        memset(&word, 0, sizeof(word));
                        setImmediateValue(&word, value, 0);
                        printf("TEST --> Encoded Word: ");
                        printWordAsBinary(word);

                        memory[IC] = value;
                        memoryLines[IC].word->value = word.value;
                        memoryLines[IC].type = INDEX_ADDRESSING_VALUE;
                        memoryLines[IC].value = value;
                        IC++;
                    }
                    else if (lookupSymbol(index) == NULL)
                    {
                        memory[IC] = -1;
                        memoryLines[IC].type = INDEX_ADDRESSING;
                        memoryLines[IC].value = -1;
                        IC++;
                    }
                    else
                    {
                        value = lookupSymbol(index)->value;
                        memset(&word, 0, sizeof(word));
                        setImmediateValue(&word, value, 0);
                        printf("TEST --> Encoded Word: ");
                        printWordAsBinary(word);

                        memory[IC] = value;

                        memoryLines[IC].word->value = word.value;
                        memoryLines[IC].type = INDEX_ADDRESSING_VALUE;
                        memoryLines[IC].value = value;
                        IC++;
                    }
                }
            }

            printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);

            totalMemoryLines += 2;
            free(copy);
            break;
        case REGISTER:
            memset(&word, 0, sizeof(word));
            value = atoi(operands[i] + 1);
            memoryLines[IC].word = malloc(sizeof(Word));

            printf("Operand %d (%s) uses Register addressing IC = %d\n", i, operands[i], IC);
            if (memoryLines[IC].word)
            {
                if (isSrcReg == 0 && i == 0)
                {
                    setRegisterValue(memoryLines[IC].word, value, 0, 1, 0);
                    printWordAsBinary(*memoryLines[IC].word);
                    isSrcReg = 1;
                }
                else if (isSrcReg == 1 && i == 1)
                {
                    printf("TEST --> Setting register value src: %d, dest: %d\n", atoi(operands[i - 1] + 1), value);
                    setRegisterValue(memoryLines[IC - 1].word, atoi(operands[i - 1] + 1), value, 1, 1);
                    printWordAsBinary(*memoryLines[IC - 1].word);
                }
                else
                {

                    setRegisterValue(memoryLines[IC].word, 0, value, 0, 1);
                    printWordAsBinary(*memoryLines[IC].word);
                }

                memoryLines[IC].type = REGISTER_ADDRESSING;
                memoryLines[IC].value = memoryLines[IC].word->value;

                printf("TEST --> Encoded Memory[%d]: ", IC);
                printWordAsBinary(*(memoryLines[IC].word));
                if (isSrcReg == 1 && i == 1)
                {
                    ;
                }
                else
                {
                    IC++;
                    totalMemoryLines += 1;
                }

                printf("TEST --> Memory[%d] = %d\n", IC, memory[IC]);
            }
            else
            {
                printf("TEST --> Memory allocation failed\n");
                handleError("Memory allocation failed", lineNum, operands[i]);
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

/**
 * Implements the getAddressingMethod function declared in first_pass.h.
 */
Addressing getAddressingMethod(char *operand)
{
    /* Check for immediate addressing mode signified by a '#' */
    if (operand[0] == '#')
    {
        printf("TEST --> Immediate\n");
        return IMMEDIATE; /* Return immediate addressing type */
    }
    /* Check for index addressing mode signified by presence of '[' and ']' */
    if (strchr(operand, '[') && strchr(operand, ']'))
    {
        printf("TEST --> Index\n");
        return INDEX; /* Return index addressing type */
    }
    /* Check for register addressing mode signified by 'r' followed by a digit */
    if (operand[0] == 'r' && isdigit(operand[1]))
    {
        printf("TEST --> Register\n");
        return REGISTER; /* Return register addressing type */
    }
    /* If none of the above, assume direct addressing */
    printf("TEST --> Direct\n");
    return DIRECT; /* Return direct addressing type */
}

/* ############################### end LINE_INSTRUCTION code ############################### */
