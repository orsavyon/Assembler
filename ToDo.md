# Assembler Documentation

## Pre-Processor Parser

### Overview

The pre-processor parser checks line numbers and characters to identify errors, necessitating a separate file tracker and an error handling function.

### Error Function

- Develop an error function to handle parsing errors and inconsistencies.

### TODO

- Continue with the macro algorithm:
  1. Read the next line.
  2. If the field is a macro name, switch to content.
  3. If the field is "mcr", switch "has mcr" flag on (else skip to step 6).
  4. There's a macro definition - enter the macro name into the macro table.
  5. Read the next line. If EOF, skip to step 9. If "has macro flag" is on and no "endmcr" detected, enter the line to macro content and delete the line; else, go back to step 1.
  6. If "endmcr" detected, delete the tag from the file and continue; if not, go back to step 5.
  7. Turn off "has mcr" flag and go back to step 1 (finish macro definition save).
  8. Finish: save the spread macro file.

### Notes

- A function is needed to add a macro when processing "mcr".
- A sub-function might be required for handling outside of the macroParser.
- A saved words list should be added to check macro validation.

### Implementation Details

- An insert macro function has been implemented but is not inserting yet. Check if "endmcr" can be assumed to be in a new line; if not, implement accordingly.
- After implementing the insert macro and ensuring the table works fine, a copy file mechanism was created.
- The consumed macro line needs to be deleted, and the insert macro and copy file saved.

## First Pass

- The first pass involves reading and interpreting the source code without generating machine code.
- Identify labels, constants, and macro definitions to fill the symbol and macro tables.
- Resolve symbolic references to memory addresses and prepare the groundwork for the actual code generation in the second pass.

### TODO

- First Pass Algorithm
  Need to do step 9.
  Need to do step 14.

## Second Pass

- The second pass generates the machine code or the final output of the assembler.
- Translate assembly language instructions into machine code, using the data from the first pass.
- Perform error checking and optimization, if any, during this phase.
- Output the final machine code, ready for execution or further processing.

## Macro Parser
