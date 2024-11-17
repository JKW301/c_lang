# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=gnu99 -g
SANITIZE_FLAGS = -fsanitize=address -fsanitize=undefined
LDFLAGS = 

# Define object files
OBJ = src/repl.o src/insert.o src/main.o src/create.o src/describe.o src/select.o src/delete.o src/btree.o src/persistance.o

# Define the final executable
TARGET = ViewDB

# Default target
all: $(TARGET)

# Build the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Rules to compile each .c file
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJ) $(TARGET)

# Build with sanitizers
sanitize: clean
	$(MAKE) CFLAGS="$(CFLAGS) $(SANITIZE_FLAGS)" LDFLAGS="$(LDFLAGS) $(SANITIZE_FLAGS)" all

# Build for Valgrind (without sanitizers)
valgrind_build: clean
	$(MAKE) CFLAGS="$(filter-out $(SANITIZE_FLAGS), $(CFLAGS))" LDFLAGS="$(filter-out $(SANITIZE_FLAGS), $(LDFLAGS))" all

# Run Valgrind
valgrind: valgrind_build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

.PHONY: all clean sanitize valgrind valgrind_build
