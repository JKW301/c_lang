#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#include <stddef.h>

void execute_insert(Statement* statement, const char* filename) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;
    long insertion_point = 0;

    // Find the correct table in the CSV file
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table:", 11) == 0) {
            char table_name[255];
            sscanf(line, "#### Table: %s", table_name);
            if (strcmp(table_name, statement->table_name) == 0) {
                table_found = 1;

                if (fgets(line, sizeof(line), file)) {
                    insertion_point = ftell(file);
                    break;
                }
            }
        }
    }

    if (!table_found) {
        printf("Erreur : Table '%s' non trouvée.\n", statement->table_name);
        fclose(file);
        return;
    }

    // Read the rest of the file content after the insertion point
    fseek(file, insertion_point, SEEK_SET);
    char remaining_content[10000] = "";  // Adjust size as needed
    char temp_line[MAX_LINE_LENGTH];
    while (fgets(temp_line, sizeof(temp_line), file)) {
        strcat(remaining_content, temp_line);
    }

    // Move the cursor back to the insertion point and add a newline before the new row
    fseek(file, insertion_point, SEEK_SET);
    fprintf(file, "\n%s\n", statement->formatted_values);  // Add the new row
    fprintf(file, "%s", remaining_content);

    fclose(file);
    printf("Ligne insérée avec succès dans la table '%s'.\n", statement->table_name);
}
