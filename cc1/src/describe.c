#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#include <stddef.h>

// Fonction pour décrire les colonnes d'une table
void describe_table(const char* filename, const char* table_name) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;

    // Parcourir le fichier pour trouver la table
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0) {
            // Vérifier si c'est la table recherchée
            char existing_table_name[255];
            sscanf(line, "#### Table: %254[^\n]", existing_table_name);
            if (strcmp(existing_table_name, table_name) == 0) {
                table_found = 1;
                // Lire la ligne suivante contenant les colonnes
                if (fgets(line, sizeof(line), file)) {
                    printf("Colonnes de la table '%s':\n", table_name);
                    char* token = strtok(line, ",");
                    while (token != NULL) {
                        printf("  %s\n", token);  // Affiche chaque colonne
                        token = strtok(NULL, ",");
                    }
                }
                break;
            }
        }
    }

    fclose(file);

    if (!table_found) {
        printf("Erreur : Table '%s' non trouvée dans le fichier %s.\n", table_name, filename);
    }
}

// Exemple d'utilisation dans le REPL
void execute_describe(Statement* statement, const char* filename) {
    describe_table(filename, statement->table_name);
}