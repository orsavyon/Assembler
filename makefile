CC			= gcc
CFLAGS		= -ansi -pedantic -Wall
PROG_NAME	= main
BUILD_DIR 	= build
OBJ_DIR 	= $(BUILD_DIR)/obj
BIN_DIR		= $(BUILD_DIR)/bin

ifdef DEBUG
CFLAGS += -g
endif

.PHONY: clean build_dir all

all: build_dir $(PROG_NAME)

$(PROG_NAME): build_dir main.o preprocessor.o macro_table.o utils.o
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o -o $(BIN_DIR)/$@

main.o: main.c Preprocessor/preprocessor.h Preprocessor/macro_table.h utils.h 
preprocessor.o: Preprocessor/preprocessor.c Preprocessor/preprocessor.h Preprocessor/macro_table.h utils.h
macro_table.o: Preprocessor/macro_table.c Preprocessor/macro_table.h utils.h
utils.o: utils.c utils.h

%.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@

clean:
	rm -rf $(BUILD_DIR)

build_dir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)