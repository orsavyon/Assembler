#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include the necessary header files */
#include "Preprocessor/preprocessor.h"

/* Define MAX_FILE_NAME_LENGTH if not already defined */
#define MAX_FILE_NAME_LENGTH 256

/* Define the test function */
int test_process_as_file(const char* input_filename, const char* expected_output_filename) {
    /* Call process_as_file function with input file */
    char* generated_output_filename = process_as_file(input_filename);
    if (generated_output_filename == NULL) {
        printf("Error: Failed to process input file: %s\n", input_filename);
        return 0; /* Test failed*/
    }

    /* Compare generated output file with expected output file */
    FILE* generated_output_file = fopen(generated_output_filename, "r");
    FILE* expected_output_file = fopen(expected_output_filename, "r");
    if (generated_output_file == NULL || expected_output_file == NULL) {
        printf("Error: Failed to open output files for comparison\n");
        return 0; /*Test failed */
    }

    /* Compare contents of generated output file with expected output file */
    int match = 1;
    int ch1, ch2;
    while ((ch1 = fgetc(generated_output_file)) != EOF && (ch2 = fgetc(expected_output_file)) != EOF) {
        if (ch1 != ch2) {
            match = 0;
            break;
        }
    }

    fclose(generated_output_file);
    fclose(expected_output_file);

    /* Print test result */
    if (match) {
        printf("Test passed: %s\n", input_filename);
    } else {
        printf("Test failed: %s\n", input_filename);
    }

    free(generated_output_filename);
    return match; /* Return 1 for pass, 0 for fail */
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <input_file.as> <expected_output_file.am>\n", argv[0]);
        return 1;
    }

    const char* input_filename = argv[1];
    const char* expected_output_filename = argv[2];

    printf("Attempting to process input file: %s\n", input_filename);

    char* generated_output_filename = process_as_file(input_filename);
    if (generated_output_filename == NULL) {
        printf("Error: Failed to process input file: %s\n", input_filename);
        return 1;
    } else {
        printf("Output filename generated: %s\n", generated_output_filename);
    }

    return 0;
}