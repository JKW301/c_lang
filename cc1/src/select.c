#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#include <stddef.h>

// Fonction pour sélectionner et afficher les valeurs d'une colonne donnée dans une table
void select_from_table(const char* filename, const char* table_name, const char* column_name) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;
    int column_index = -1;
    char columns[MAX_LINE_LENGTH][255];  // Stockage des noms de colonnes
    int num_columns = 0;

    // Étape 1 : Trouver la table
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0) {
            char existing_table_name[255];
            sscanf(line, "#### Table: %254[^\n]", existing_table_name);

            if (strcmp(existing_table_name, table_name) == 0) {
                table_found = 1;

                // Étape 2 : Lire les colonnes et trouver l'index de la colonne demandée
                if (fgets(line, sizeof(line), file)) {
                    char* token = strtok(line, ",");
                    while (token != NULL) {
                        strcpy(columns[num_columns], token);  // Stocker le nom de chaque colonne
                        if (strncmp(token, column_name, strlen(column_name)) == 0) {
                            column_index = num_columns;  // Trouver l'index de la colonne demandée
                        }
                        num_columns++;
                        token = strtok(NULL, ",");
                    }

                    if (column_index == -1) {
                        printf("Erreur : Colonne '%s' non trouvée dans la table '%s'.\n", column_name, table_name);
                        fclose(file);
                        return;
                    }

                    // Étape 3 : Lire les lignes de données et afficher la valeur de la colonne demandée
                    printf("Valeurs de la colonne '%s' dans la table '%s':\n", column_name, table_name);
                    while (fgets(line, sizeof(line), file)) {
                        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0 || line[0] == '\n') {
                            break;  // Fin de la table
                        }
                        
                        // Extraire les valeurs de chaque colonne dans la ligne
                        token = strtok(line, ",");
                        int current_column = 0;
                        while (token != NULL) {
                            if (current_column == column_index) {
                                printf("  %s\n", token);  // Afficher la valeur de la colonne sélectionnée
                            }
                            current_column++;
                            token = strtok(NULL, ",");
                        }
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
void execute_select(Statement* statement, const char* filename) {
    select_from_table(filename, statement->table_name, statement->values[0]);  // Le nom de la colonne est dans values[0]
}