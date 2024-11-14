CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=gnu99 -g -fsanitize=address -fsanitize=undefined
LDFLAGS = -fsanitize=address -fsanitize=undefined
# Define object files
OBJ = src/repl.o src/insert.o src/main.o src/create.o src/describe.o src/select.o src/delete.o src/btree.o

# Define the final executable
ViewDB: $(OBJ)
	$(CC) $(CFLAGS) -o ViewDB $(OBJ)

# Rule to compile repl.c
src/repl.o: src/repl.c
	$(CC) $(CFLAGS) -c src/repl.c -o src/repl.o

# Rule to compile insert.c
src/insert.o: src/insert.c
	$(CC) $(CFLAGS) -c src/insert.c -o src/insert.o

# Rule to compile main.c
src/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

# Rule to compile create.c
src/create.o: src/create.c
	$(CC) $(CFLAGS) -c src/create.c -o src/create.o

# Rule to compile describe.c
src/describe.o: src/describe.c
	$(CC) $(CFLAGS) -c src/describe.c -o src/describe.o

# Rule to compile select.c
src/select.o: src/select.c
	$(CC) $(CFLAGS) -c src/select.c -o src/select.o

# Rule to compile delete.c
src/delete.o: src/delete.c
	$(CC) $(CFLAGS) -c src/delete.c -o src/delete.o

# Rule to compile btree.c
src/btree.o: src/btree.c
	$(CC) $(CFLAGS) -c src/btree.c -o src/btree.o

# Clean up object files and the executable
clean:
	rm -f $(OBJ) ViewDB

# Build for valgrind
valgrind_build: $(OBJ)
	$(CC) $(CFLAGS) -o ViewDB $(OBJ)

# Run valgrind
valgrind: valgrind_build
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ViewDB

# Test target (runs valgrind)
test: valgrind

.PHONY: all clean valgrind valgrind_build test
