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
 * Performs the first pass of the assembler over the source file.
 * This pass initializes the necessary data structures and processes each line to build the symbol table and
 * set the initial code (IC) and data counters (DC). It handles different types of lines such as labels,
 * directives, and instructions, and performs error checking on line lengths and definitions.
 *
 * @param fp Pointer to the source file being read.
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
 * Checks if the provided line of assembly code begins with a valid label.
 * A label starts with an alphabet character, followed by alphanumeric characters, up to 31 characters long,
 * and ends with a ':' without any preceding spaces.
 *
 * @param line A character pointer to the line to be checked for a label.
 * @return Returns 1 if a valid label is present and it is not a reserved word, otherwise returns 0.
 */
int isLabel(char *line);

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
 * Processes a line designated as an entry directive in assembly source code.
 * It also ensures that duplicate entry declarations are handled appropriately by checking
 * against the existing list of entry labels.
 *
 * @param line A character pointer to the entry directive line to be processed.
 */
void processEntryDirective(char *line);
/**
 * Processes lines containing data or string directives in an assembly program.
 * This function parses the line to extract and handle numerical data or string literals based on the directive type.
 *
 * @param line The line containing the data directive to process.
 */
void processDataDirective(char *line);

/* ########## LINE_INSTRUCTION ########## */

/**
 * Checks if the first word of the line is a valid assembly instruction.
 * Issues a notice if an instruction matches case-insensitively but not case-sensitively.
 *
 * @param line A string containing the assembly line to check.
 * @return Returns 1 if a valid instruction is found, otherwise returns 0.
 */
int isInstruction(char *line);

/**
 * Processes a line that contains an assembly instruction.
 * This function parses the instruction, allocates memory for its components, and stores them appropriately.
 *
 * @param line A character pointer to the instruction line to be processed.
 */
void processInstruction(char *line);

/**
 * Validates whether a line from the assembly source code represents a syntactically correct instruction.
 * This function extracts the instruction name and its operands from the line and checks if the instruction name exists in the command table.
 *
 * @param line A character pointer to the line to be checked.
 * @return An integer 1 if the instruction is valid, otherwise 0.
 */
int isValidInstruction(char *line);

/**
 * Initializes the first word of an instruction based on its opcode and addressing modes.
 * This function sets up the opcode, ARE (Absolute, Relocatable, External), and addressing modes for source and destination operands.
 *
 * @param firstWord Pointer to the Word struct representing the first word of the instruction.
 * @param instruction Pointer to the Instruction struct containing the instruction information.
 */
void setupFirstInstructionWord(Word *firstWord, Instruction *instruction);

/**
 * Determines if a given addressing mode is valid based on an array of allowed modes.
 * This function checks if the specified mode is among the allowed addressing modes.
 *
 * @param mode The addressing mode to validate.
 * @param allowedModes An array of integers representing the allowed addressing modes.
 * @return An integer 1 if the addressing mode is valid, otherwise 0.
 */
int isValidAddressingMode(int mode, int allowedModes[]);

/**
 * Parses an instruction line into its component parts and populates an Instruction struct with the parsed data.
 * The function extracts the instruction name, opcode, and operands from the line and verifies the instruction's validity.
 *
 * @param line The instruction line to parse.
 * @param instruction Pointer to the Instruction struct to store the parsed information.
 * @return Pointer to the Instruction struct if parsing is successful, otherwise NULL.
 */
Instruction *parseInstruction(char *line, Instruction *instruction);

/**
 * Decodes the operands of an instruction line based on their addressing modes.
 * It handles different addressing modes such as immediate, direct, index, and register, and updates the instruction counter.
 *
 * @param operands The array of operand strings to decode.
 * @return The number of memory lines used by the decoded operands.
 */
int decodeOperands(char *operands[]);

/**
 * Determines the addressing method used by an operand in assembly language instruction.
 * This function identifies whether the operand uses immediate, index, register, or direct addressing.
 *
 * @param operand The operand string to analyze.
 * @return The addressing method as an enumeration value of type Addressing.
 */
Addressing getAddressingMethod(char *operand);

#endif /* FIRSTPASS_H */
