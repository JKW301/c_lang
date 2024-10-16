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
    int column_index = -1;
    int is_wildcard = strcmp(statement->column_name, "*") == 0;

    // Find the table in the CSV file
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table:", 11) == 0) {
            char table_name[255];
            sscanf(line, "#### Table: %s", table_name);
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

    // Read column names
    if (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        int index = 0;
        char columns[MAX_COLUMNS][MAX_VALUE_LEN];

        while (token != NULL) {
            trim_whitespace(token);
            strcpy(columns[index], token);

            // Determine the column index if it's not a wildcard select
            if (!is_wildcard && strcmp(token, statement->column_name) == 0) {
                column_index = index;
            }

            index++;
            token = strtok(NULL, ",");
        }

        // If column index wasn't found for a non-wildcard select
        if (column_index == -1 && !is_wildcard) {
            printf("Erreur : Colonne '%s' non trouvée dans la table '%s'.\n", statement->column_name, statement->table_name);
            fclose(file);
            return;
        }
    }

    // Read and print rows
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table:", 11) == 0) {
            break; // Stop reading at the next table
        }

        char* token = strtok(line, ",");
        int index = 0;

        while (token != NULL) {
            trim_whitespace(token);

            // Print all columns or only the selected column
            if (is_wildcard || index == column_index) {
                printf("%s ", token);
            }

            index++;
            token = strtok(NULL, ",");
        }

        if (is_wildcard || column_index != -1) {
            printf("\n");
        }
    }

    fclose(file);
}
