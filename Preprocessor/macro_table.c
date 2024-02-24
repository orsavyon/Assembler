#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro_table.h"


/* Creates a new Macro struct and initializes it with the macro_name. */
Macro * create_macro(char macro_name[MAX_LINE_LENGTH])
{
    Macro *macro = NULL;
    macro = (Macro *)calloc(1, sizeof(Macro));
    if (macro == NULL)
    {
        printf("%s\n", "Allocation failed for a new macro");
        exit(0);
    }