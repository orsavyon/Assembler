#include "Preprocessor/preprocessor.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_NAME_LENGTH 256
#define MAX_LINE_LENGTH 1024

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++) 
    {
        FILE *am_file;
        char *am_filename = (char*)calloc(MAX_FILE_NAME_LENGTH, sizeof(char));
        if (am_filename) 
            am_filename = process_as_file(argv[i]);

        am_file = fopen(am_filename, "r");
        if (am_file == NULL) 
        {
            fprintf(stderr, "Error: Cannot open file %s\n", am_filename);
            continue; /*Continue to the next file*/ 
        }

        printf("Contents of %s:\n", am_filename);
        char line[MAX_LINE_LENGTH];
        while (fgets(line, MAX_LINE_LENGTH, am_file) != NULL) 
        {
            printf("%s", line); /*Print each line*/
        }

        fclose(am_file);
    }

    return 0;
}
