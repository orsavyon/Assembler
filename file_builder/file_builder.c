#include "file_builder.h"
#include "../utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Get the instructions counter, data counter, first binary memory address and build an object file. */
void build_ob_file( char * ob_filename, int* memory_address)
{
    FILE * ob_file;
    int i;

    ob_filename = strcat(ob_filename, DOT_OB_SUFFIX); /*the filename before adding .ob*/
    ob_file = fopen(ob_filename, "w");

    if (ob_file == NULL) 
    {
        printf("Failed to open ob_file\n");
        return;
    }

    /*Write IC and DC counts to the first line*/
    fprintf(ob_file, "%d %d\n", IC, DC);

    /*Write memory addresses and contents to the file*/ 
    for (int i = 0; i < IC + DC; i++) {
        /*Print decimal address*/
        printf("%4d ",i+100); 
        /*Transform decimal address content to encoded and print it*/
        printf("%4s\n",base4ToEncoded(decimalToBase4(binaryToDecimal(memory_address))));
    }

    fclose(ob_file);
}

/* @brief Get entry_table and build '.ent' file with all the entries defenitions' lines.*/
void build_entry_file(char * entry_filename, Symbol * entry_table)
{
    FILE * entry_file;

    entry_filename = strcat(entry_filename, DOT_ENT_SUFFIX);
    entry_file = fopen(entry_filename, "w");

    if (entry_file == NULL)
    {
        printf("Failed to open entry_file\n");
        return;
    }

    while (entry_table != NULL) 
    {
        fprintf(entry_file, "%s\t%d\n", entry_table->symbol_name, entry_table->address);
        entry_table = entry_table->next_symbol;
    }

    fclose(entry_file);
}

/* Get extern_table and build '.ext' file with all the address where extern symbols are called. */
void build_extern_file(char * extern_filename, Symbol * extern_table)
{
    FILE * extern_file;

    extern_filename = strcat(extern_filename, DOT_EXT_SUFFIX);
    extern_file = fopen(extern_filename, "w");

    if (extern_file == NULL)
    {
        printf("Failed to open extern_file\n");
        return;
    }

    while (extern_table != NULL) 
    {
        fprintf(extern_file, "%s\t%d\n", extern_table->symbol_name, extern_table->address);
        extern_table = extern_table->next_symbol;
    }

    fclose(extern_file);
}

