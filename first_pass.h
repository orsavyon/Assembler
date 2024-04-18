#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "data.h"

/* Enumeration for different types of lines */
typedef enum
{
    LINE_BLANK,       /* Blank Line */
    LINE_COMMENT,     /* Comment Line */
    LINE_DIRECTIVE,   /* Directive line */
    LINE_INSTRUCTION, /* Instruction line */
    LINE_DEFINITION,  /* Definition line */
    LINE_LABEL,       /* Label line */
    INVALID_LINE      /* Invalid line type */
} LineType;


/**
 * Determines the type of a line.
 *
 * @param line The line to analyze.
 * @return The type of the line.
 */
LineType getLineType(char *line);


/**
 * Performs the first pass of the assembler.
 *
 * @param fp Pointer to the source file.
 */
void firstPass(FILE *fp);


/**
 * Processes a definition line.
 *
 * @param line The definition line to process.
 */
void processDefinition(char *line);


/**
 * Checks if a line defines a constant.
 *
 * @param line The line to check.
 * @return 1 if the line defines a constant, otherwise 0.
 */
int isConstantDefinition(char *line);


/**
 * Checks if a constant definition is valid.
 *
 * @param line The line to check.
 * @return 1 if the constant definition is valid, otherwise 0.
 */
int isValidConstantDefinition(char *line);


/**
 * Processes an instruction line.
 *
 * @param line The instruction line to process.
 */
void processInstruction(char *line);


/**
 * Checks if a line represents an instruction.
 *
 * @param line The line to check.
 * @return 1 if the line represents an instruction, otherwise 0.
 */
int isInstruction(char *line);


/**
 * Checks if an instruction line is valid.
 *
 * @param line The line to check.
 * @return 1 if the instruction line is valid, otherwise 0.
 */
int isValidInstruction(char *line);


/**
 * Retrieves the first word from a line.
 *
 * @param line The line to extract the first word from.
 * @return The first word of the line.
 */
char *getFirstWord(const char *line);



/**
 * Checks if a line contains a symbol.
 *
 * @param line The line to check.
 * @return 1 if the line contains a symbol, otherwise 0.
 */
int isSymbol(char *line);


/**
 * Determines the type of a directive.
 *
 * @param line The directive line.
 * @return The type of the directive.
 */
DirectiveType getDirectiveType(char *line);


/**
 * Processes a directive line.
 *
 * @param line The directive line to process.
 */
void processDirective(char *line);


/**
 * Processes an extern directive line.
 *
 * @param line The extern directive line to process.
 */

void processExternDirective(char *line);

/**
 * Processes a data directive line.
 *
 * @param line The data directive line to process.
 */
void processDataDirective(char *line);

/**
 * Decodes the operands of an instruction line.
 *
 * @param operands The array to store the decoded operands.
 * @return The number of operands decoded.
 */
int decodeOperands(char *operands[]);


/**
 * Parses an instruction line and stores the parsed information in an Instruction struct.
 *
 * @param line The instruction line to parse.
 * @param instruction Pointer to the Instruction struct to store the parsed information.
 * @return Pointer to the parsed Instruction struct.
 */
Instruction *parseInstruction(char *line, Instruction *instruction);


/**
 * Determines the addressing method of an operand.
 *
 * @param operand The operand to analyze.
 * @return The addressing method of the operand.
 */
Addressing getAddressingMethod(char *operand);


/**
 * Checks if an addressing mode is valid.
 *
 * @param mode The addressing mode to check.
 * @param allowedModes Array of allowed addressing modes.
 * @return 1 if the addressing mode is valid, otherwise 0.
 */
int isValidAddressingMode(int mode, int allowedModes[]);


/**
 * Sets up the first word of an instruction.
 *
 * @param firstWord Pointer to the Word struct representing the first word of the instruction.
 * @param instruction Pointer to the Instruction struct containing the instruction information.
 */
void setupFirstInstructionWord(Word *firstWord, Instruction *instruction);

#endif /* FIRSTPASS_H */
