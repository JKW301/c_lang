#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <stddef.h>

void execute_insert(Statement* statement, const char* filename) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        printf(COLOR_RED "Erreur : Impossible d'ouvrir le fichier %s.\n" COLOR_RESET, filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;
    long insertion_point = 0;

    
    while (fgets(line, sizeof(line), file)) {
        int tableid;
        if (strncmp(line, "#### Table:", 11) == 0) {
            char table_name[255];
            sscanf(line, "#### Table::%d: %s", &tableid, table_name);
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
        printf(COLOR_RED "Erreurrrr : Table '%s' non trouvée.\n" COLOR_RESET, statement->table_name);
        fclose(file);
        return;
    }

    
    fseek(file, insertion_point, SEEK_SET);
    char remaining_content[10000] = "";  
    char temp_line[MAX_LINE_LENGTH];
    while (fgets(temp_line, sizeof(temp_line), file)) {
        strcat(remaining_content, temp_line);
    }

    
    fseek(file, insertion_point, SEEK_SET);
    fprintf(file, "\n%s\n", statement->formatted_values);  
    fprintf(file, "%s", remaining_content);

    fclose(file);
    printf(COLOR_GREEN "Ligne insérée avec succès dans la table '%s'.\n" COLOR_RESET, statement->table_name);
}
