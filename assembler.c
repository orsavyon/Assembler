#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro_parser.h"
#include "assembler.h"
#include "utils.h"
#include "data.h"
#include "first_pass.h"
#include "second_pass.h"

/**
 * @brief Entry point of the assembler program.
 *
 * Processes each file provided as command-line argument by performing macro processing
 * and the first pass of the assembler.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Program exit status.
 */

int main(int argc, char *argv[])
{
    FILE *fp, *cp, *mc;
    int i;

    /* Check if the correct number of arguments is provided */
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        exit(1);
    }

    /* Process each file provided as argument */
    for (i = 1; i < argc; i++)
    {
        /* Allocate memory for file names */
        char *fileName = malloc(strlen(argv[i]) + 4); /* +4 for the extension and null terminator */
        char *copyName = malloc(strlen(argv[i]) + 4);

        strcpy(fileName, argv[i]);
        strcpy(copyName, fileName);
        strcat(copyName, ".as"); /* Append file extension */

        /* Open the source file for reading */
        fp = fopen(fileName, "r");
        if (fp != NULL)
        {
            /* Open a new file for writing the preprocessed code */
            if ((cp = fopen(copyName, "w+")) == NULL)
            {
                fprintf(stderr, "Failed to create a copy of the file.\n");
                free(fileName);
                fclose(fp);
                exit(1);
            }

            else
            {
                /* Copy the content of the source file to the new file after preprocessing */
                skipAndCopy(fp, cp);
                rewind(cp);
                fclose(fp);

                /* Perform macro processing on the copied file */
                macroParser(cp);

                /* Open the file containing the macro-expanded code */
                mc = fopen("parsedMacro.asm", "r");
                if (mc == NULL)
                {
                    fprintf(stderr, "couldn't open macro file\n");
                }

                /* Print the content of the macro-expanded file (for verification/debugging) */
                printFile(mc);
                rewind(mc);
                /* Perform the first pass of the assembler */
                firstPass(mc);
                if (errorFlag)
                {
                    fprintf(stderr, "Errors detected in the first pass. Exiting...\n");
                    free(fileName);
                    fclose(mc);
                    fclose(cp);
                    exit(1);
                }
                rewind(mc);
                /* Perform the second pass of the assembler */
                secondPass(mc);
                /*Create output files ob ent ext*/
                if (errorFlag)
                {
                    fprintf(stderr, "Errors detected in the second pass. Exiting...\n");
                    free(fileName);
                    fclose(mc);
                    fclose(cp);
                    exit(1);
                }
                else
                {
                    printf("No errors detected. Output files created.\n");
                }

                fclose(mc);
            }
        }
        else
        {
            fprintf(stderr, "Couldn't open file: %s\n", fileName);
        }
    }

    return 0; /* Successful termination of the program */
}
