#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "data.h"

typedef enum
{
    LINE_BLANK,
    LINE_COMMENT,
    LINE_DIRECTIVE,
    LINE_INSTRUCTION,
    LINE_DEFINITION,
    LINE_LABEL,
    INVALID_LINE
} LineType;

LineType getLineType(char *line);

void firstPass(FILE *fp);

void processDefinition(char *line);

int isConstantDefinition(char *line);

int isValidConstantDefinition(char *line);

void processInstruction(char *line);

int isInstruction(char *line);

int isValidInstruction(char *line);

char *getFirstWord(const char *line);

int isSymbol(char *line);

DirectiveType getDirectiveType(char *line);

void processDirective(char *line);

void processExternDirective(char *line);

void processDataDirective(char *line);

#endif