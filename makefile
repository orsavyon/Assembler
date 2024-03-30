# Define the compiler
CC=gcc
# Define the flags for the compiler
CFLAGS=-ansi -Wall -pedantic

# Default target
all: main

# Main program executable
main: main.c utils.o preprocessor.o macro_table.o
	$(CC) $(CFLAGS) main.c utils.o preprocessor.o macro_table.o -o main

# Object file for utils.c
utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o

# Object file for preprocessor.c
preprocessor.o: preprocessor.c preprocessor.h macro_table.o
	$(CC) $(CFLAGS) -c preprocessor.c -o preprocessor.o

# Object file for macro_table.c
macro_table.o: macro_table.c macro_table.h
	$(CC) $(CFLAGS) -c macro_table.c -o macro_table.o

# Clean target to remove all compiled objects and the executable
clean:
	rm -f *.o main
