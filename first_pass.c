#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"
#include "first_pass.h"
#include "data.h"

/**
 * Performs the first pass of the assembler over the source file.
 * This pass initializes the necessary data structures and processes each line to build the symbol table and
 * set the initial code (IC) and data counters (DC). It handles different types of lines such as labels,
 * directives, and instructions, and performs error checking on line lengths and definitions.
 *
 * @param fp Pointer to the source file being read.
 */

void firstPass(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    IC = 0; /* Instruction Counter initialized */
    DC = 0; /* Data Counter initialized */

    /* Initialize necessary data structures for assembling process */
    initData();
    initSymbolTable();
    initMemoryLines();

    /* Process each line of the source file */
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        lineErrorFlag = 0; /* Reset line-specific error flag for the new line */
        lineNum++;

        /* Check if line exceeds the limit */
        if (strlen(line) == MAX_LINE_LENGTH - 1 && line[MAX_LINE_LENGTH - 2] != '\n')
        {
            int ch;
            handleError("Line length exceeds the limit", lineNum, line);
            while ((ch = fgetc(fp)) != '\n' && ch != EOF)
                ;

            continue;
        }

        /* Trimming line to remove possible trailing whitespaces */
        trimLine(line);

        /* Determine the type of the line and process accordingly */
        switch (getLineType(line))
        {
        case LINE_BLANK:
        case LINE_COMMENT:
            /* Ignore blank and comment lines */
            break;

        case LINE_DEFINITION:
            /* Line detected with .define - Process constant definitions */
            processDefinition(line);
            break;

        case LINE_LABEL:
        {
            char symbolName[MAX_LINE_LENGTH];     /* Buffer to store the extracted symbol name */
            char *symbolPos;                      /* Pointer to locate the symbol in the line */
            sscanf(line, "%[^:]", symbolName);    /* Extract the symbol name from the label, stopping at the colon */
            symbolPos = strstr(line, symbolName); /* Find the position of the symbol name in the line */
            symbolFlag = 1;                       /* Flag to indicate that a symbol is being processed */
            if (symbolPos)
            {
                char *remainingLine = symbolPos + strlen(symbolName) + 1; /* Move past the symbol name in the line */

                trimLine(remainingLine); /* Trim the remaining line to remove leading/trailing whitespace */

                if (getLineType(remainingLine) == LINE_BLANK)
                {
                    handleError("Missing instruction/action after label", lineNum, line); /* Error for labels without instructions */
                }
                else if (getLineType(remainingLine) == LINE_DIRECTIVE)
                {
                    if (getDirectiveType(remainingLine) == DATA_DIRECTIVE || getDirectiveType(remainingLine) == STRING_DIRECTIVE)
                    {
                        if (lookupSymbol(symbolName) == NULL) /* Check if symbol is not yet defined */
                        {
                            addSymbol(symbolName, data, DC); /* Add symbol as a data type if not defined */
                        }
                        else
                        {

                            handleError("Symbol already exists", lineNum, line); /* Error if symbol is already defined */
                        }
                    }
                    if (lineErrorFlag == 0) /* Process directive if no previous errors */
                    {
                        processDirective(remainingLine);
                    }
                }
                else if (symbolFlag == 1)
                {

                    if (lookupSymbol(symbolName) == NULL)
                    {
                        addSymbol(symbolName, code, IC + 100); /* Add symbol as code type with an offset */
                    }
                    else
                    {

                        handleError("Symbol already defined", lineNum, line);
                    }
                    if (lineErrorFlag == 0 && getLineType(remainingLine) == LINE_INSTRUCTION) /* Process instruction if no errors */
                    {

                        if (isValidInstruction(remainingLine))
                        {

                            processInstruction(remainingLine);
                        }
                    }
                    else if (getLineType(remainingLine) != LINE_INSTRUCTION)
                    {
                        handleError("Invalid instruction", lineNum, line);
                    }
                }
                else
                {

                    handleError("Invalid line", lineNum, line);
                }
            }
            break;
        }

        case LINE_DIRECTIVE:
            /* Handle directives such as .data, .string, etc., */
            processDirective(line);
            break;

        case LINE_INSTRUCTION:
            /* Handle instructions that need to be translated into machine code */
            processInstruction(line);
            break;

        case INVALID_LINE:
            handleError("Invalid line", lineNum, line);
            break;
        default:
            break;
        }
        symbolFlag = 0; /* Reset symbol flag for next line processing */
    }
    lineErrorFlag = 0;    /* Reset line-specific error flag */
    updateSymbolValues(); /* Update symbol values based on accumulated data and instruction counts */
}

/* ############################### start HELPERS code ############################### */

/**
 * Extracts and returns the first word from a given line of text.
 * This function skips any leading whitespace and captures the first sequence of non-whitespace characters.
 *
 * @param line A constant character pointer to the line from which the first word is to be extracted.
 * @return A pointer to a static buffer containing the first word of the line.
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

    firstWord[i] = '\0'; /* Null-terminate the extracted word */
    return firstWord;    /* Return the static buffer containing the first word */
}

/**
 * Determines the type of a line.
 *
 * @param line The line to analyze.
 * @return The type of the line.
 */
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
 * Checks if the provided line of assembly code begins with a valid label.
 * A label starts with an alphabet character, followed by alphanumeric characters, up to 31 characters long,
 * and ends with a ':' without any preceding spaces.
 *
 * @param line A character pointer to the line to be checked for a label.
 * @return Returns 1 if a valid label is present and it is not a reserved word, otherwise returns 0.
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
            handleError("Invalid label: Label must start with an alphabetic character", lineNum, line);
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
            handleError("Invalid label: Label must end with a colon", lineNum, line);
        }
        return 0;
    }

    label[i] = '\0'; /* Null-terminate the label string */

    /* Check if the label is a reserved word */
    if (isReservedWord(label))
    {
        if (errLabel == 1)
        {
            handleError("Invalid label: Label cannot be a reserved word", lineNum, line);
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
 * Processes a definition line in assembly language input.
 * This function parses a definition line that specifies a constant,
 * and if valid, adds it to the symbol table with its associated value.
 *
 * @param line A character pointer to the definition line to process.
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
    }
    else
    {
        /* Handle the error if the definition line is invalid */
        handleError("Invalid constant definition", lineNum, line);
    }
}

/**
 * Validates whether a given line from the assembly source represents a valid constant definition.
 * This function checks the syntax and uniqueness of a constant definition within the assembly source.
 * It also validates that the defined value is within the permissible range for constants.
 *
 * @param line A character pointer to the string containing the potential constant definition.
 * @return An integer 1 if the constant definition is valid, otherwise 0.
 */
int isValidConstantDefinition(char *line)
{
    char lineCopy[MAX_LINE_LENGTH]; /* Buffer to hold a copy of the line */
    char *constantPart, *valuePart;
    int value;
    char *start;

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
        if (lineErrorFlag == 0)
        {
            handleError("Invalid constant definition: Number format error", lineNum, line);
        }
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

    return 1; /* Valid constant definition */
}

/* ############################### end LINE_DEFINITION code ############################### */
/* ############################################################################################# */
/* ############################### start LINE_DIRECTIVE code ############################### */

/**
 * Processes a directive line from an assembly language input.
 * Based on the type of directive identified by `getDirectiveType`, it executes the relevant processing function or handles errors.
 *
 * @param line A character pointer to the directive line to be processed.
 */
void processDirective(char *line)
{
    /* Switch on the type of directive determined by getDirectiveType function */
    switch (getDirectiveType(line))
    {
    case DATA_DIRECTIVE:
        processDataDirective(line); /* Call to process data directive */
        /* ! Refactor */
        break;
    case STRING_DIRECTIVE:
        processDataDirective(line); /* Call to process string directive */
        /* ! Refactor */
        break;
    case DEFINE_DIRECTIVE:
    case ENTRY_DIRECTIVE:
        /* Add symbol declared as an entry to the record list */
        processEntryDirective(line);
        break;
    case EXTERN_DIRECTIVE:
        if (symbolFlag == 1) /* Check condition based on symbolFlag */
        {
            break;
        }
        else
        {
            processExternDirective(line); /* Call to process extern directive */
            break;
        }
    case INVALID_DIRECTIVE:
        handleError("Invalid directive", lineNum, line); /* Handle invalid directive */
        break;
    }
}

/**
 * Processes lines containing data or string directives in an assembly program.
 * This function parses the line to extract and handle numerical data or string literals based on the directive type.
 *
 * @param line The line containing the data directive to process.
 */
void processDataDirective(char *line)
{
    char *token; /* Token for parsing the data elements */
    /* Check if the line starts with '.data' directive */

    if (strncmp(line, ".data", 5) == 0)
    {
        char buffer[MAX_LINE_LENGTH]; /* Ensure the buffer is large enough for your lines */
        char *checkCommas;
        int lastCharIndex;
        strcpy(buffer, line);
        /* Validate correct comma usage */
        checkCommas = buffer + 6; /* Start checking after .data directive */
        trimLine(checkCommas);    /* Trim whitespace around the line */
        lastCharIndex = strlen(checkCommas) - 1;

        if (checkCommas[0] == ',' || checkCommas[lastCharIndex] == ',' || strstr(checkCommas, ",,") != NULL)
        {
            handleError("Improper use of commas in .data directive", lineNum, line);
            return; /* Exit if comma validation fails */
        }
        token = strtok(checkCommas, ","); /* Start tokenizing the line after the directive */
        if (token == NULL)
        {
            handleError("Missing data in .data directive", lineNum, line);
        }
        while (token != NULL)
        {
            Symbol *symbol;               /* Pointer to a symbol structure */
            trimLine(token);              /* Trim whitespace around the token */
            symbol = lookupSymbol(token); /* Check if the token is a known symbol */
            /* If the token is a defined symbol, store its value in memory */
            if (symbol && symbol->symbolType == mdefine)
            {
                memory[DC + IC] = symbol->value;

                memoryLines[DC + IC].value = computeFourteenBitValue(symbol->value);
                DC++;
            }
            else if (isNumeric(token)) /* If the token is a numeric value, store it directly */
            {
                memory[DC + IC] = atoi(token);

                memoryLines[DC + IC].value = computeFourteenBitValue(atoi(token));

                DC++;
            }
            else /* Handle the error case where the token is neither a defined symbol nor a valid number */
            {
                handleError("Undefined symbol or invalid number in .data directive", lineNum, line);
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

        if (start && end && start < end) /* If valid string boundaries are found, process the string */
        {
            char *c;

            for (c = start; c < end; c++) /* Store each character of the string in memory */
            {
                if (!isLegalCharacter(*c))
                {

                    handleError("Illegal character found in string", lineNum, line);
                    return; /* Exit the function if illegal character is found */
                }

                memory[DC + IC] = (unsigned char)*c;

                memoryLines[DC + IC].value = computeFourteenBitValue((unsigned char)*c);

                DC++;
            }
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
 * Identifies the type of directive based on the first word of a given line from an assembly source.
 * This function determines the directive type such as data, string, entry, extern, or invalid.
 *
 * @param line A character pointer to the directive line to identify.
 * @return An enumeration value of type DirectiveType corresponding to the directive found.
 */
DirectiveType getDirectiveType(char *line)
{
    char *directiveName; /* Pointer to store the first word from the line */

    directiveName = getFirstWord(line); /* Retrieve the first word from the line */

    /* Check for '.data' directive and return corresponding type */
    if (strcmp(directiveName, ".data") == 0)
    {
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
        handleError("Invalid directive", lineNum, line);
        return INVALID_DIRECTIVE;
    }
}

/**
 * Processes a line designated as an extern directive in assembly source code.
 * This function tokenizes the line to extract and handle each symbol declared as external.
 *
 * @param line A character pointer to the extern directive line to be processed.
 */

void processExternDirective(char *line)
{
    char buffer[MAX_LINE_LENGTH];    /* Buffer to store the line */
    char *token;                     /* Token for parsing symbols from the directive */
    strcpy(buffer, line);            /* Copy the line to the buffer */
    token = strtok(buffer + 7, ","); /* Begin tokenizing after the directive keyword */
    if (token == NULL)
    {
        handleError("Missing symbol in .extern directive", lineNum, line);
    }
    /* Iterate through tokens representing symbols */
    while (token != NULL)
    {
        Symbol *symbol;  /* Pointer to a symbol structure */
        trimLine(token); /* Trim whitespace around the token */
        /* Check if the token is already marked as an entry label */
        if (isEntryLabel(token))
        {
            /* Handle the error and exit processing for this line */
            handleError("Cannot declare entry label as external", lineNum, line);
            return; /* Exit the function early */
        }
        symbol = lookupSymbol(token); /* Check if the symbol is already in the table */
        if (symbol)
        {
            updateSymbolType(token, external); /* Update the symbol type to external */
        }
        else
        {
            addSymbol(token, external, 0);
        }
        token = strtok(NULL, ","); /* Continue to the next token */
    }
}

/**
 * Processes a line designated as an entry directive in assembly source code.
 * It also ensures that duplicate entry declarations are handled appropriately by checking
 * against the existing list of entry labels.
 *
 * @param line A character pointer to the entry directive line to be processed.
 */
void processEntryDirective(char *line)
{
    char buffer[MAX_LINE_LENGTH]; /* Buffer to store the line */
    char *token;
    strcpy(buffer, line); /* Copy the line to the buffer */
    trimLine(buffer);     /* Trim whitespace around the line */
    token = strtok(buffer + 7, ", \t");

    /* Begin tokenizing after the directive keyword */
    if (token == NULL) /* Check if the token is missing */
    {
        handleError("Missing symbol in .entry directive", lineNum, line);
    }

    /* Iterate through tokens representing symbols */
    while (token != NULL)
    {
        trimLine(token); /* Trim whitespace around the token */
        if (!isEntryLabel(token))
        {
            addEntryLabel(token); /* Add the symbol as an entry label */
        }
        else
        {
            handleError("Entry label already declared", lineNum, line);
        }
        token = strtok(NULL, ","); /* Continue to the next token */
    }
}

/* ############################### end LINE_DIRECTIVE code ############################### */

/* ############################### start LINE_INSTRUCTION code ############################### */

/**
 * Checks if the first word of the line is a valid assembly instruction.
 * Issues a notice if an instruction matches case-insensitively but not case-sensitively.
 *
 * @param line A string containing the assembly line to check.
 * @return Returns 1 if a valid instruction is found, otherwise returns 0.
 */
int isInstruction(char *line)
{
    char *instructionName; /* Variable to store the first word of the line */
    int i;

    instructionName = getFirstWord(line); /* Retrieve the first word from the line */

    /* Loop through the command table to check for a match */
    for (i = 0; i < CMD_NUM; i++)
    {
        if (strCaseCmp(instructionName, commandTable[i].cmdName) == 0) /* Case-insensitive comparison */
        {
            if (strcmp(instructionName, commandTable[i].cmdName) != 0) /* Case-sensitive comparison */
            {
                handleError("Instruction case mismatch\n", lineNum, line); /* Handle case mismatch */
                return 0;                                                  /* Return 0 if case mismatch is found */
            }
            return 1; /* Return 1 if a match is found indicating an instruction */
        }
    }
    return 0; /* Return 0 if no match is found */
}

/**
 * Processes a line that contains an assembly instruction.
 * This function parses the instruction, allocates memory for its components, and stores them appropriately.
 *
 * @param line A character pointer to the instruction line to be processed.
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
        }

        memory[IC] = firstWord->bits.opcode; /* Store the opcode in the main memory at the current instruction counter */

        IC++; /* Increment the instruction counter */

        L = 1;                                     /* Set line count for the instruction */
        L += decodeOperands(instruction.operands); /* Decode and add operand sizes to L */
    }
    else
    {
        if (lineErrorFlag == 0)
        {
            handleError("Invalid instruction", lineNum, line); /* Handle invalid instruction format */
        }
    }
}

/**
 * Validates whether a line from the assembly source code represents a syntactically correct instruction.
 * This function extracts the instruction name and its operands from the line and checks if the instruction name exists in the command table.
 *
 * @param line A character pointer to the line to be checked.
 * @return An integer 1 if the instruction is valid, otherwise 0.
 */
int isValidInstruction(char *line)
{
    char *instructionName = (char *)malloc(MAX_LINE_LENGTH * sizeof(char)); /* Allocate memory for instruction name */
    char *operands = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));        /* Allocate memory for storing operands */
    int i;

    /* Parse the line to extract the instruction name and its operands */
    sscanf(line, "%s %[^\n]", instructionName, operands);

    /* Loop through the command table to check if the instruction name is valid */
    for (i = 0; i < CMD_NUM; i++)
    {
        if (strcmp(instructionName, commandTable[i].cmdName) == 0)
        {
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
 * Initializes the first word of an instruction based on its opcode and addressing modes.
 * This function sets up the opcode, ARE (Absolute, Relocatable, External), and addressing modes for source and destination operands.
 *
 * @param firstWord Pointer to the Word struct representing the first word of the instruction.
 * @param instruction Pointer to the Instruction struct containing the instruction information.
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
 * Determines if a given addressing mode is valid based on an array of allowed modes.
 * This function checks if the specified mode is among the allowed addressing modes.
 *
 * @param mode The addressing mode to validate.
 * @param allowedModes An array of integers representing the allowed addressing modes.
 * @return An integer 1 if the addressing mode is valid, otherwise 0.
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
 * Parses an instruction line into its component parts and populates an Instruction struct with the parsed data.
 * The function extracts the instruction name, opcode, and operands from the line and verifies the instruction's validity.
 *
 * @param line The instruction line to parse.
 * @param instruction Pointer to the Instruction struct to store the parsed information.
 * @return Pointer to the Instruction struct if parsing is successful, otherwise NULL.
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

        int expectedOperands;
        int operandCount = 0;
        char *operands;
        int len;
        char *token = strtok(buffer, " \t"); /* Tokenize the line to extract the instruction name */
                                             /* Counter for operands */
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

            trimLine(operands); /* Trim whitespace around the operands */
            len = strlen(operands);
            /* Check for leading, trailing commas or double commas */
            if (*operands == ',' || operands[len - 1] == ',' || strstr(operands, ",,") != NULL)
            {
                handleError("Improper use of commas in operands", lineNum, line);
                free(buffer);
                return NULL;
            }
            /* Extra check for missing comma if exactly two operands are expected */
            if (expectedOperands == 2 && (strchr(operands, ',') == NULL))
            {
                handleError("Missing comma between operands", lineNum, line);
                free(buffer);
                return NULL;
            }

            token = strtok(operands, ","); /* Tokenize the operands */
            while (token)
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
 * Decodes the operands of an instruction line based on their addressing modes.
 * It handles different addressing modes such as immediate, direct, index, and register, and updates the instruction counter.
 *
 * @param operands The array of operand strings to decode.
 * @return The number of memory lines used by the decoded operands.
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

            memory[IC] = atoi(operands[i] + 1); /* Store value directly in memory */

            memoryLines[IC].word->value = word.value;
            memoryLines[IC].type = IMMEDIATE_ADDRESSING;
            memoryLines[IC].value = value;
            IC++; /* Increment instruction counter */

            totalMemoryLines += 1; /* Increment total memory lines used */
            break;
        case DIRECT:
            memory[IC] = -1; /* Placeholder for future linking */
            memoryLines[IC].type = DIRECT_ADDRESSING;
            memoryLines[IC].needEncoding = 1;
            memoryLines[IC].symbol = strdup(operands[i]);
            memoryLines[IC].value = -1;
            recordExternalSymbolUsage(operands[i], IC);
            IC++; /* Increment instruction counter */

            totalMemoryLines += 1; /* Increment total memory lines used */
            break;
        case INDEX:
            /* Handle indexed addressing */
            copy = strdup(operands[i]);
            symbolName = strtok(copy, "[");
            start = strchr(operands[i], '[');
            end = strchr(operands[i], ']');

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

                        memory[IC] = value;
                        memoryLines[IC].word->value = word.value;
                        memoryLines[IC].type = INDEX_ADDRESSING_VALUE;
                        memoryLines[IC].value = value;
                        IC++;
                    }
                    else if (lookupSymbol(index) != NULL)
                    {
                        value = lookupSymbol(index)->value;
                        memset(&word, 0, sizeof(word));
                        setImmediateValue(&word, value, 0);

                        memory[IC] = value;

                        memoryLines[IC].word->value = word.value;
                        memoryLines[IC].type = INDEX_ADDRESSING_VALUE;
                        memoryLines[IC].value = value;
                        IC++;
                    }
                    else
                    {
                        handleError("Invalid index value", lineNum, index);
                    }
                }
            }

            totalMemoryLines += 2;
            free(copy);
            break;
        case REGISTER:
            memset(&word, 0, sizeof(word));
            value = atoi(operands[i] + 1);
            memoryLines[IC].word = malloc(sizeof(Word));

            if (memoryLines[IC].word)
            {
                if (isSrcReg == 0 && i == 0)
                {
                    setRegisterValue(memoryLines[IC].word, value, 0, 1, 0);
                    isSrcReg = 1;
                }
                else if (isSrcReg == 1 && i == 1)
                {
                    setRegisterValue(memoryLines[IC - 1].word, atoi(operands[i - 1] + 1), value, 1, 1);
                }
                else
                {

                    setRegisterValue(memoryLines[IC].word, 0, value, 0, 1);
                }

                memoryLines[IC].type = REGISTER_ADDRESSING;
                memoryLines[IC].value = memoryLines[IC].word->value;

                if (isSrcReg == 1 && i == 1)
                {
                    ;
                }
                else
                {
                    IC++;
                    totalMemoryLines += 1;
                }
            }
            else
            {
                handleError("Memory allocation failed", lineNum, operands[i]);
            }

            break;
        case INVALID:
            break;
        }
    }
    return totalMemoryLines;
}

/**
 * Determines the addressing method used by an operand in assembly language instruction.
 * This function identifies whether the operand uses immediate, index, register, or direct addressing.
 *
 * @param operand The operand string to analyze.
 * @return The addressing method as an enumeration value of type Addressing.
 */
Addressing getAddressingMethod(char *operand)
{
    /* Check for immediate addressing mode signified by a '#' */
    if (operand[0] == '#')
    {
        if (isspace((unsigned char)operand[1]) || operand[1] == '\0')
        {
            handleError("Invalid immediate value", lineNum, operand);
            return INVALID;
        }
        return IMMEDIATE; /* Return immediate addressing type */
    }
    /* Check for index addressing mode signified by presence of '[' and ']' */
    if (strchr(operand, '[') && strchr(operand, ']'))
    {
        return INDEX; /* Return index addressing type */
    }
    /* Check for register addressing mode signified by 'r' followed by a digit */
    if (operand[0] == 'r')
    {
        if (operand[1] >= '0' && operand[1] <= '7' && (operand[2] == '\0' || isspace((unsigned char)operand[2]) || operand[2] == ','))
        {
            return REGISTER; /* Return register addressing type */
        }
        else
        {
            handleError("Invalid register value", lineNum, operand);
            return INVALID;
        }
    }
    /* If none of the above, assume direct addressing */
    return DIRECT; /* Return direct addressing type */
}

/* ############################### end LINE_INSTRUCTION code ############################### */
