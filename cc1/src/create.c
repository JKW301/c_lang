#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#include <stddef.h>

// Fonction pour créer une table dans le fichier CSV
void create_table(const char* filename, const char* table_name, const char** columns, int num_columns) {
    // Vérification si la table existe déjà
    if (table_exists_in_file(filename, table_name)) {
        printf("Erreur : La table '%s' existe déjà dans le fichier %s.\n", table_name, filename);
        return;
    }

    // Ouverture du fichier en mode append
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    // Écriture de la nouvelle table
    fprintf(file, "#### Table: %s\n", table_name);
    for (int i = 0; i < num_columns; i++) {
        fprintf(file, "%s", columns[i]);
        if (i < num_columns - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n\n");

    fclose(file);
    printf("Table '%s' créée avec succès dans le fichier %s.\n", table_name, filename);
}

// Exemple d'utilisation dans le main ou la boucle REPL
void execute_createtable(Statement* statement, const char* filename) {
    // Exemple de colonnes pour la nouvelle table
    const char* columns[] = {"id::INT", "nom::string", "email::string"};
    int num_columns = 3;

    create_table(filename, statement->table_name, columns, num_columns);
}
