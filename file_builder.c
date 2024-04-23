#include "file_builder.h"
#include "utils.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

/* Get the instructions counter, data counter, first binary memory address and build an object file. */
void build_ob_file(char *ob_filename, unsigned int memory_address[])
{
    FILE *ob_file;
    int i;
    int base4[BASE_4_DIGITS];
    printf("TEST --> Building ob file\n");
    strcat(ob_filename, DOT_OB_SUFFIX);
    ob_file = fopen(ob_filename, "w");
    if (ob_file == NULL)
    {
        printf("Failed to open ob_file\n");
        return;
    }

    /* Write IC and DC counts to the first line */
    fprintf(ob_file, " %4d %-4d \n", IC, DC);

    /* Write memory addresses and contents to the file */
    for (i = 0; i < IC + DC; i++)
    {

        printf("TEST --> Decimal: %d\n", memory_address[i]);
        decimalToBase4(memory_address[i], base4);
        fprintf(ob_file, "%-4d  ", i + 100);
        fprintf(ob_file, "%4s\n", base4ToEncoded(base4));
    }

    printf("TEST --> End of ob file\n");
    fclose(ob_file);
    printf("TEST --> File %s created successfully\n", ob_filename);
}

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