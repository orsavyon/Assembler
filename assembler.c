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
 * Processes each file provided as a command-line argument by performing the following steps:
 * - Macro processing: Preprocesses the file to expand macros.
 * - First Pass: Generates a symbol table and calculates memory addresses.
 * - Second Pass: Generates machine code using the symbol table and addresses determined in the first pass.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments including program name and input files.
 * @return int Returns EXIT_SUCCESS if all files are processed without errors; otherwise, may exit with EXIT_FAILURE upon severe errors.
 */

int main(int argc, char *argv[])
{
    FILE *fp, *cp, *mc;
    int i;
    char *fileName, *copyName;

    /* Check if the correct number of arguments is provided */
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file1> <file2> ... <fileN>\n", argv[0]);
        exit(EXIT_FAILURE);
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
            return EXIT_FAILURE;
        }

        strcpy(fileName, argv[i]);
        strcpy(copyName, argv[i]);
        strcat(fileName, EXTENTION);      /* Append file extension */
        strcat(copyName, COPY_EXTENTION); /* Append file extension */

        /* Open the source file for reading. If the file cannot be opened, skip to the next file. */
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
        cutOffExtension(fileName);

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
        rewind(mc);
        /** Perform the first pass of the assembler */
        firstPass(mc);
        if (errorFlag)
        {
            fprintf(stderr, "Errors detected in the first pass. Exiting...\n");
            fclose(mc);
            fclose(cp);
            remove(copyName);
            free(fileName);
            free(copyName);

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
            fclose(mc);
            fclose(cp);
            remove(copyName);
            free(fileName);
            free(copyName);

            continue;
        }
        cutOffExtension(fileName);
        createObFile(fileName, memoryAddress); /* Create the object file */
        createEntryFile(fileName);             /* Create the entry file */
        createExtFile(fileName);               /* Create the external file */
        fclose(mc);                            /* Close the macro file */
        remove(copyName);                      /* Remove the copy file */
        freeMemoryLines();                     /* Free memory allocated for memory lines */
        free(fileName);
        free(copyName);
    }

    return EXIT_SUCCESS; /* Successful termination of the program */
}
