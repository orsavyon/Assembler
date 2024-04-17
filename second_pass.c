#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first_pass.h"
#include "second_pass.h"
#include "data.h"
#include "utils.h"

void secondPass(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    IC = 0;
    lineNum = 0;

    printf("\n --- in secondPass --- \n\n");
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        lineNum++;
        trimLine(line);
        printf("line: %s\n", line);
    }
}