#include "file_builder.h"
#include "utils.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

/**
  @brief Get Memory address and build an '.ob' file from them.

  @param ob_filename The name of the '.ob' file.
  @param memory_address Address of first memory word.
 */
void createObFile(char *ob_filename, unsigned int memory_address[])
{
    FILE *ob_file;
    int i;
    int base4[BASE_4_DIGITS];
    strcat(ob_filename, DOT_OB_SUFFIX);
    ob_file = fopen(ob_filename, "w");
    if (ob_file == NULL)
    {
        fprintf(stderr, "Failed to open file.\n");
        return;
    }

    /* Write IC and DC counts to the first line */
    fprintf(ob_file, " %4d %-4d \n", IC, DC);

    /* Write memory addresses and contents to the file */
    for (i = 0; i < IC + DC; i++)
    {

        decimalToBase4(memory_address[i], base4);
        fprintf(ob_file, "%04d  ", i + 100);
        fprintf(ob_file, "%4s\n", base4ToEncoded(base4));
    }

    cutOffExtension(ob_filename);
    fclose(ob_file);
}

/**
 * @brief Get the entry symbols and their addresses and build an '.ent' file from them.
 *
 * @param ent_filename The name of the '.ent' file.
 */
void createEntryFile(char *ent_filename)
{
    FILE *ent_file = NULL; /* Declare file pointer */
    int i;

    /* Check each bucket in the symbol table */
    for (i = 0; i < MAX_SYMBOLS; i++)
    {
        Symbol *current = symbolTable[i];
        while (current != NULL)
        {
            /* If the symbol type is 'entry', we process it */
            if (current->symbolType == entry)
            {
                if (ent_file == NULL)
                { /* File is opened only if an entry symbol is found */
                    strcat(ent_filename, DOT_ENT_SUFFIX);
                    ent_file = fopen(ent_filename, "w");
                    if (ent_file == NULL)
                    {
                        fprintf(stderr, "Failed to open file.\n");
                        return;
                    }
                }
                fprintf(ent_file, "%s  %04u\n", current->symbolName, current->value);
            }
            current = current->next; /* Move to next symbol in the list */
        }
    }

    if (ent_file != NULL)
    {
        fclose(ent_file); /* Only close the file if it was opened */
    }
    cutOffExtension(ent_filename);
}
/**
 * @brief Get the external symbols and their addresses and build an '.ext' file from them.
 *
 * @param ext_filename The name of the '.ext' file.

*/
void createExtFile(char *ext_filename)
{
    FILE *ext_file = NULL; /* Declare file pointer */
    int i, j;

    /* Concatenate the filename with the extension only once */
    strcat(ext_filename, DOT_EXT_SUFFIX);

    /* Check each bucket in the symbol table */
    for (i = 0; i < MAX_SYMBOLS; i++)
    {
        Symbol *current = symbolTable[i];
        while (current != NULL)
        {
            /* If the symbol type is 'external', we process it */
            if (current->symbolType == external)
            {
                if (ext_file == NULL)
                { /* File is opened only if an external symbol is found */
                    ext_file = fopen(ext_filename, "w");
                    if (ext_file == NULL)
                    {
                        fprintf(stderr, "Failed to open file.\n");
                        return;
                    }
                }
                for (j = 0; j < externalUsageCount; j++)
                {
                    if (strcmp(externalUsages[j].symbolName, current->symbolName) == 0)
                    {
                        fprintf(ext_file, "%-4s  %04d\n", externalUsages[j].symbolName, externalUsages[j].address + 100);
                    }
                }
            }
            current = current->next; /* Move to next symbol in the list */
        }
    }
    if (ext_file != NULL)
    {
        fclose(ext_file); /* Only close the file if it was opened */
    }
    cutOffExtension(ext_filename);
}

/**
 * @brief Transform an decimal array to base4 array.
 *
 * @param decimal The decimal array.
 */
void decimalToBase4(int decimal, int base4[])
{
    int i;
    int index = BASE_4_DIGITS - 1; /* Start from the last element to match MSB on left */

    /* Initialize base4 array to zero */
    for (i = 0; i < BASE_4_DIGITS; i++)
    {
        base4[i] = 0;
    }

    /* Convert decimal to base 4 */
    while (decimal > 0 && index >= 0)
    {
        base4[index--] = decimal % 4;
        decimal /= 4;
    }
}
/**
 * @brief Transform an base4 array to encoded strings array.
 *
 * @param base4 The base4 array.
 */
char *base4ToEncoded(const int base4[])
{
    int i;
    static char encoded[BASE_4_DIGITS + 1]; /* Static allocation to avoid memory management */

    /* Convert base 4 digits to encoded characters */
    for (i = 0; i < BASE_4_DIGITS; i++)
    {
        switch (base4[i])
        {
        case 0:
            encoded[i] = '*';
            break;
        case 1:
            encoded[i] = '#';
            break;
        case 2:
            encoded[i] = '%';
            break;
        case 3:
            encoded[i] = '!';
            break;
        }
    }

    encoded[BASE_4_DIGITS] = '\0'; /* Null-terminate the string */
    return encoded;
}