#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "preprocessor.h"

/* main: assembler */
int main(int argc, char *argv[])
{

    /* This is a simple test to check the preprocessor */
    int i;

    for (i = 1; i < argc; i++)
    {
        process_as_file(argv[i]); /* run the preprocesspr with the filename */
    }
    return 0;
}