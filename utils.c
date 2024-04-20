#include "utils.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/**
 * Removes all white spaces from the source_line and saves the result in dest_line.
 */
void remove_white_spaces(char *source_line, char *dest_line)
{
	int i = 0;
	int j = 0;

	while (source_line[i] != '\0')
	{
		if (isspace(source_line[i]) == 0)
		{
			dest_line[j] = source_line[i];
			j++;
		}
		i++;
	}

	dest_line[j] = '\0';
}

/**
 * Removes the prefix white spaces from the source_line and saves the result in dest_line.
 */
void remove_prefix_white_spaces(char *source_line, char *dest_line)
{
    int i = 0;
    int j = 0;
    int found_first_char = 0;

    while (source_line[i] != '\0')
    {
        if (!found_first_char && isspace(source_line[i]))
        {
            i++;
            continue;
        }

        found_first_char = 1;
        dest_line[j] = source_line[i];
        i++;
        j++;
    }

    dest_line[j - 2] = '\0';
}

/**
 * Removes the macro indentation from the sourceline and saves the result in dest_line.
 * The macro indentation is assumed to start at index 4 of sourceline.
 */
void remove_macro_indentation(char *sourceline, char *dest_line)
{
    strncpy(dest_line, sourceline + FOUR_CHARS_INDENTATION, MAX_LINE_LENGTH - FOUR_CHARS_INDENTATION);
}

/* Checks if string is an int. */
int is_integer(const char *str) 
{
    char *endptr;
    strtol(str, &endptr, 10);

    return *endptr == '\0';
}

int binaryToDecimal(int binary[]) {
    int decimal = 0;
    for (int i = 0; i < 14; i++) {
        decimal = decimal * 2 + binary[i];
    }
    return decimal;
}

int * decimalToBase4(int decimal) {
    int *base4 = (int *)malloc(BASE_4_DIGITS * sizeof(int));
    if (base4 == NULL) {
        return NULL; /*Memory allocation failed*/
    }

    int i = 0;
    /*Convert decimal to base 4*/
    while (decimal > 0) {
        base4[i] = decimal % 4;
        decimal = decimal / 4;
        i++;
    }

    // If decimal is 0, we need to handle that separately
    if (i == 0) {
        base4[0] = 0;
    }

    // Fill remaining digits with 0
    for (; i < BASE_4_DIGITS; i++) {
        base4[i] = 0;
    }

    return base4;
}

char * base4ToEncoded(int base4[]) {
    char *encoded = (char *)malloc(5 * sizeof(char)); // Allocate memory for encoded pattern

    if (encoded == NULL) {
        return NULL; // Memory allocation failed
    }

    for (int i = 0; i < 4; i++) {
        switch (base4[i]) {
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
            default:
                printf("Invalid base 4 digit.\n");
                free(encoded); // Free memory before returning
                return NULL;
        }
    }

    encoded[4] = '\0'; // Null-terminate the string

    // Concatenate characters into one string
    char *encodedString = (char *)malloc((strlen(encoded) + 1) * sizeof(char));
    if (encodedString == NULL) {
        free(encoded);
        return NULL; // Memory allocation failed
    }
    strcpy(encodedString, encoded);

    free(encoded); // Free memory for encoded pattern
    return encodedString;
}