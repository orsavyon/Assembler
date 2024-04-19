#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "data.h"

void secondPass(FILE *fp);

void handleDirective(char *line);

void encodeRemainingInstruction(char *line);

char *extractInstruction(char *line);

int encodeSymbol(char *symbol);

int setValue(int value, unsigned int areBits);

void encodeRemainingInstruction2();

#endif