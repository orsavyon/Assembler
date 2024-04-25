# Assembler Project

## Authors

- Raz Yehiel
- Or Savyon

## Overview

This assembler is the final project for the 2024 Semester A System Programming Lab.

- **Macro Processing:** Expands macros within the assembly files.
- **First Pass:** Builds a symbol table and determines memory addresses.
- **Second Pass:** Uses the symbol table and memory addresses to generate machine code.

## Requirements

The assembler is implemented in C and is intended to be run on Unix/Linux environments.

- Each input file must have a `.as` extension.

## Usage

To run the assembler, use the following command format. Exclude the `.as` extension from the filenames when passing them as arguments:

./assembler <file1>, <file2>

For example, if your assembly file is named `program.as`, you would run:

./assembler program

## Output

The assembler generates several types of files depending on the contents of the input files:

- `.ob` - Object code file containing the machine code.
- `.ent` - Entry file listing all entry labels along with their addresses.
- `.ext` - External file listing all external labels used in the assembly file.
- `.am` - Error file (if applicable) detailing any issues found during the assembly process.
  Error messages and line numbers related to syntax or semantic issues are outputted following the `.am` file format.
