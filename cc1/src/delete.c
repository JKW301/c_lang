#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "delete.h"
#include "repl.h"

void execute_delete_table(Statement* statement, const char* filename) {
    FILE* file = fopen(filename, "r");
    FILE* temp_file = fopen("temp_database.csv", "w");

    if (!file || !temp_file) {
        perror(COLOR_RED "Erreur d'ouverture du fichier" COLOR_RESET);
        if (file) fclose(file);
        if (temp_file) fclose(temp_file);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;
    int skip_table = 0; 
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table::", 12) == 0) {
            char existing_table_name[255];
            sscanf(line, "#### Table::%*d: %254[^\n]", existing_table_name);

            printf("Checking table: '%s' against '%s'\n", existing_table_name, statement->table_name);

            if (strcmp(existing_table_name, statement->table_name) == 0) {
                table_found = 1;
                skip_table = 1; 
                continue;       
            } else {
                skip_table = 0; 
            }
        }

        if (!skip_table) {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    if (!table_found) {
        printf(COLOR_RED "Erreur : La table '%s' n'a pas été trouvée.\n" COLOR_RESET, statement->table_name);
        remove("temp_database.csv");
        return;
    }

    if (rename("temp_database.csv", filename) != 0) {
        perror(COLOR_RED "Erreur lors du renommage du fichier temporaire" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "Succès : La table '%s' a été supprimée avec succès.\n" COLOR_RESET, statement->table_name);
    }
}
