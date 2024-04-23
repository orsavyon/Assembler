all: assembler

assembler: assembler.o macro_parser.o first_pass.o second_pass.o utils.o data.o file_builder.o
	gcc -ansi -Wall -pedantic assembler.o macro_parser.o first_pass.o second_pass.o file_builder.o utils.o data.o -o assembler

assembler.o: assembler.c assembler.h utils.h
	gcc -ansi -Wall -pedantic -c assembler.c -o assembler.o

macro_parser.o: macro_parser.c macro_parser.h utils.h
	gcc -ansi -Wall -pedantic -c macro_parser.c -o macro_parser.o

first_pass.o: first_pass.c first_pass.h utils.h data.h
	gcc -ansi -Wall -pedantic -c first_pass.c -o first_pass.o

second_pass.o: second_pass.c second_pass.h utils.h data.h first_pass.h
	gcc -ansi -Wall -pedantic -c second_pass.c -o second_pass.o

file_builder.o: file_builder.c file_builder.h data.h utils.h
	gcc -ansi -Wall -pedantic -c file_builder.c -o file_builder.o

utils.o: utils.c utils.h
	gcc -ansi -Wall -pedantic -c utils.c -o utils.o

data.o: data.c data.h
	gcc -ansi -Wall -pedantic -c data.c -o data.o

clean:
	rm -f *.o assembler