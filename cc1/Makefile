CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=gnu99

# Define object files
OBJ = src/repl.o src/insert.o src/main.o src/create.o src/describe.o src/select.o

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

#rule to compile describe.c
src/describe.o: src/describe.c
	$(CC) $(CFLAGS) -c src/describe.c -o src/describe.o

#rule to compile select.c
src/select.o: src/select.c
	$(CC) $(CFLAGS) -c src/select.c -o src/select.o


# Clean up object files and the executable
clean:
	rm -f $(OBJ) ViewDB
