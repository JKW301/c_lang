TARGET = ViewDB

CC = gcc

CFLAGS = -Wall -Werror -Wextra -std=gnu99
# -WALL pour Warning All = affiche erreurs
# -Werror = affiche les warning comme eds erreurs
# -Wextra = affiche des "erreurs", optionnel
# c11 = version de C standard

SRCS = src/main.c src/repl.c

OBJS = $(SRCS:.c=.o)

# Règle principale pour créer l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Règle pour compiler chaque fichier source en fichier objet
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJS) $(TARGET)