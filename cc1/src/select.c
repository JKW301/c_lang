#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#include <stddef.h>

void execute_select(Statement* statement, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;
    int column_indices[MAX_SELECTED_COLUMNS];
    int is_wildcard = (statement->num_selected_columns == 1 && strcmp(statement->selected_columns[0], "*") == 0);

    // Initialize column indices to -1 (indicating not found)
    for (int i = 0; i < MAX_SELECTED_COLUMNS; i++) {
        column_indices[i] = -1;
    }

    // Find the table in the CSV file
    while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "#### Table::", 12) == 0) {
        int table_id;
        char table_name[255];
        
        // Modifier le sscanf pour extraire l'ID de la table et le nom
        sscanf(line, "#### Table::%d: %s", &table_id, table_name);

        // Affichage pour le débogage
        printf("Table ID: %d, Table Name: %s\n", table_id, table_name);

        // Comparer le nom de la table avec celui de la commande
        if (strcmp(table_name, statement->table_name) == 0) {
            table_found = 1;
            break;
        }
    }
}


    if (!table_found) {
        printf("Erreur : Table '%s' non trouvée.\n", statement->table_name);
        fclose(file);
        return;
    }

    // Read column names from the table
    if (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        int index = 0;
        char columns[MAX_COLUMNS][MAX_VALUE_LEN];

        while (token != NULL) {
            trim_whitespace(token);
            trim_type(token);
            strcpy(columns[index], token);

            // Determine column indices for each selected column
            if (is_wildcard) {
                column_indices[index] = index;  // Select all columns for wildcard
            } else {
                for (int i = 0; i < statement->num_selected_columns; i++) {
                    if (strcmp(token, statement->selected_columns[i]) == 0) {
                        column_indices[i] = index;
                    }
                }
            }

            index++;
            token = strtok(NULL, ",");
        }

        // Verify all selected columns are found
        for (int i = 0; i < statement->num_selected_columns; i++) {
            if (column_indices[i] == -1 && !is_wildcard) {
                printf("Erreur : Colonne '%s' non trouvée dans la table '%s'.\n", statement->selected_columns[i], statement->table_name);
                fclose(file);
                return;
            }
        }
    }

    // Read and print rows from the table
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table:", 11) == 0) {
            break; // Stop reading at the next table
        }

        char* token = strtok(line, ",");
        int index = 0;
        int print_needed = 0;

        while (token != NULL) {
            trim_whitespace(token);

            // Print selected columns or all columns if wildcard
            for (int i = 0; i < statement->num_selected_columns; i++) {
                if (is_wildcard || index == column_indices[i]) {
                    if (print_needed > 0) {
                        printf(", ");
                    }
                    printf("%s", token);
                    print_needed = 1;
                }
            }

            index++;
            token = strtok(NULL, ",");
        }

        if (print_needed) {
            printf("\n");
        }
    }

    fclose(file);
}
