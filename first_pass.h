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
 * Performs the first pass of the assembler.
 *
 * @param fp Pointer to the source file.
 */
void firstPass(FILE *fp);


/* ########## HELPERS ########## */

/**
 * Extracts and returns the first word from a given line of text.
 * This function skips any leading whitespace and captures the first sequence of non-whitespace characters.
 *
 * @param line A constant character pointer to the line from which the first word is to be extracted.
 * @return A pointer to a static buffer containing the first word of the line.
 */
char *getFirstWord(const char *line);



/**
 * Determines if a line of assembly code contains a symbol declaration.
 * A symbol in assembly is typically indicated by a colon followed by a space.
 *
 * @param line A character pointer to the line to be checked for a symbol.
 * @return An integer 1 if a symbol is present, otherwise 0.
 */
int isSymbol(char *line);



/**
 * Determines the type of a line.
 *
 * @param line The line to analyze.
 * @return The type of the line.
 */
LineType getLineType(char *line);




/* ########## LINE_DEFINITION ########## */

/**
 * Processes a definition line in assembly language input.
 * This function parses a definition line that specifies a constant,
 * and if valid, adds it to the symbol table with its associated value.
 *
 * @param line A character pointer to the definition line to process.
 */
void processDefinition(char *line);


/**
 * Determines if the provided line from the assembly source code defines a constant.
 * This function checks if the line starts with the ".define" keyword, which indicates a constant definition.
 *
 * @param line A character pointer to the string to be checked.
 * @return An integer 1 if the line defines a constant, otherwise 0.
 */
int isConstantDefinition(char *line);


/**
 * Validates whether a given line from the assembly source represents a valid constant definition.
 * This function checks the syntax and uniqueness of a constant definition within the assembly source.
 * It also validates that the defined value is within the permissible range for constants.
 *
 * @param line A character pointer to the string containing the potential constant definition.
 * @return An integer 1 if the constant definition is valid, otherwise 0.
 */
int isValidConstantDefinition(char *line);




/* ########## LINE_DIRECTIVE ########## */


/**
 * Identifies the type of directive based on the first word of a given line from an assembly source.
 * This function determines the directive type such as data, string, entry, extern, or invalid.
 *
 * @param line A character pointer to the directive line to identify.
 * @return An enumeration value of type DirectiveType corresponding to the directive found.
 */
DirectiveType getDirectiveType(char *line);


/**
 * Processes a directive line from an assembly language input.
 * Based on the type of directive identified by `getDirectiveType`, it executes the relevant processing function or handles errors.
 *
 * @param line A character pointer to the directive line to be processed.
 */
void processDirective(char *line);


/**
 * Processes a line designated as an extern directive in assembly source code.
 * This function tokenizes the line to extract and handle each symbol declared as external.
 *
 * @param line A character pointer to the extern directive line to be processed.
 */

void processExternDirective(char *line);

/**
 * Processes lines containing data or string directives in an assembly program.
 * This function parses the line to extract and handle numerical data or string literals based on the directive type.
 *
 * @param line The line containing the data directive to process.
 */
void processDataDirective(char *line);





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
