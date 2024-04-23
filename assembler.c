#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro_parser.h"
#include "assembler.h"
#include "utils.h"
#include "data.h"
#include "first_pass.h"
#include "second_pass.h"
#include "file_builder.h"

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
    char *fileName, *copyName, *dot;

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
        fileName = malloc(MAX_FILENAME_LEN);
        copyName = malloc(MAX_FILENAME_LEN);
        if (!fileName || !copyName)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        strcpy(fileName, argv[i]);
        strcpy(copyName, argv[i]);
        strcat(copyName, ".as"); /* Append file extension */

        /* Open the source file for reading */
        fp = fopen(fileName, "r");
        if (fp == NULL)
        {
            fprintf(stderr, "Couldn't open file: %s\n", fileName);
            free(fileName);
            free(copyName);
            continue;
        }
        /* Open a new file for writing the preprocessed code */
        cp = fopen(copyName, "w+");
        if (cp == NULL)
        {
            fprintf(stderr, "Failed to create a copy of the file.\n");
            free(fileName);
            fclose(fp);
            continue;
        }

        /** Copy the content of the source file to the new file after preprocessing */
        skipAndCopy(fp, cp);
        rewind(cp);
        fclose(fp); /** Close the original file after copying */

        /** Perform macro processing on the copied file */
        macroParser(cp, fileName);

        /** Open the file containing the macro-expanded code */
        mc = fopen(fileName, "r");
        if (mc == NULL)
        {
            fprintf(stderr, "couldn't open macro file %s\n", fileName);
            free(fileName);
            free(copyName);
            fclose(cp); /** Ensure all files are closed */
            continue;
        }

        /** Print the content of the macro-expanded file for verification/debugging */
        printFile(mc);
        rewind(mc);
        /** Perform the first pass of the assembler */
        firstPass(mc);
        if (errorFlag)
        {
            fprintf(stderr, "Errors detected in the first pass. Exiting...\n");
            free(fileName);
            free(copyName);
            fclose(mc);
            fclose(cp);
            continue;
        }
        rewind(mc);
        /* Perform the second pass of the assembler */
        secondPass(mc);
        /** Perform the second pass of the assembler */
        secondPass(mc);
        if (errorFlag)
        {
            fprintf(stderr, "Errors detected in the second pass. Exiting...\n");
            free(fileName);
            free(copyName);
            fclose(mc);
            fclose(cp);
            continue;
        }

        dot = strrchr(fileName, '.');
        if (dot)
        {
            *dot = '\0'; /** Cut off the ".am" from the file name */
        }

        printf("No errors detected. Output files created.\n");
        printf("TEST --> End of ob file in assembler\n");
        createObFile(fileName, memoryAddress);
        printf("TEST --> End of ob file in assembler\n");
        dot = strrchr(fileName, '.');
        if (dot)
        {
            *dot = '\0'; /** Cut off the ".am" from the file name */
        }
        createEntryFile(fileName);
        printf("TEST --> End of ent file in assembler\n");
        if (dot)
        {
            *dot = '\0'; /** Cut off the ".am" from the file name */
        }
        createExtFile(fileName);
        printf("TEST --> End of ext file in assembler\n");
        fclose(mc);
        free(fileName);
        free(copyName);
    }

    return 0; /* Successful termination of the program */
}
