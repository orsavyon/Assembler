#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro_parser.h"
#include "assembler.h"
#include "utils.h"
#include "data.h"
#include "first_pass.h"

/* main: assembler*/

int main(int argc, char *argv[])
{
    FILE *fp, *cp, *mc;
    int i;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        exit(1);
    }

    for (i = 1; i < argc; i++)
    {
        /* create a copy */
        char *fileName = malloc(strlen(argv[i] + 4));
        char *copyName = malloc(strlen(argv[i] + 4));
        strcpy(fileName, argv[i]);
        strcpy(copyName, fileName);
        strcat(copyName, ".as");

        fp = fopen(fileName, "r");
        if (fp != NULL)
        {

            if ((cp = fopen(copyName, "w+")) == NULL)
            {
                fprintf(stderr, "Failed to create a copy of the file.\n");
                free(fileName);
                fclose(fp);
                exit(1);
            }
            else
            {
                skipAndCopy(fp, cp);
                rewind(cp);
                fclose(fp);
                rewind(cp);
                macroParser(cp);
                mc = fopen("parsedMacro.asm", "r");
                if (mc == NULL)
                {
                    fprintf(stderr, "couldn't open macro file\n");
                }
                printFile(mc);
                rewind(mc);
                firstPass(mc);
                fclose(mc);
            }
        }
        else
        {
            fprintf(stderr, "Couldn't open file: %s\n", fileName);
        }
    }

    return 0;

} /* end of main */
