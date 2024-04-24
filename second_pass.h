#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "data.h"
/**
 * Performs the second pass of the assembler over the source file.
 * This pass processes each line of the assembly source code to resolve symbols and finalize instruction encoding.
 * It handles directives and instructions specifically, ignoring blank lines and comments.
 *
 * @param fp Pointer to the source file being read.
 */
void secondPass(FILE *fp);
/**
 * Processes assembly language directives based on the first token of a given line.
 * This function handles different types of directives: data, string, extern, define, and entry.
 * It delegates processing to other functions based on the type of directive encountered.
 *
 * @param line The line from the assembly source code to process for directives.
 */
void handleDirective(char *line);
/**
 * Encodes a symbol's value with additional addressing bits based on its type.
 * This function looks up the symbol in the symbol table and, if found, encodes it based on its type:
 * - External symbols get ARE bits set to 01.
 * - Relocatable symbols get ARE bits set to 10.
 * If the symbol is not found, an error is handled and -1 is returned.
 *
 * @param symbol The name of the symbol to encode.
 * @return The encoded value of the symbol, or -1 if the symbol cannot be found.
 */
int encodeSymbol(char *symbol);
/**
 * Encodes an integer value with Additional Relocation Encoding (ARE) bits.
 * This function adjusts a given value to fit within 12 bits, and then appends 2 ARE bits at the lowest order.
 *
 * @param value The integer value to encode.
 * @param areBits The 2-bit ARE value to append to the encoded value. Only the lowest 2 bits are used.
 * @return The encoded value with ARE bits appended.
 */
int setValue(int value, unsigned int areBits);
/**
 * Encodes all remaining instructions in the memory that require encoding.
 * This function iterates through memory lines and encodes any symbols that have not been previously encoded.
 * It updates the value of each memory line with the new encoded value if needed.
 */
void encodeRemainingInstruction();

#endif