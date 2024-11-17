#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <stddef.h>


void describe_table(const char* filename, const char* table_name) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf(COLOR_RED "Erreur : Impossible d'ouvrir le fichier %s.\n" COLOR_RESET, filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;

    
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0) {
            
            char existing_table_name[255];
            int tableid;
            sscanf(line, "#### Table::%d: %254[^\n]", &tableid, existing_table_name);
            if (strcmp(existing_table_name, table_name) == 0) {
                table_found = 1;
                
                if (fgets(line, sizeof(line), file)) {
                    printf("Colonnes de la table '%s':\n", table_name);
                    char* token = strtok(line, ",");
                    while (token != NULL) {
                        printf("  %s\n", token);  
                        token = strtok(NULL, ",");
                    }
                }
                break;
            }
        }
    }

    fclose(file);

    if (!table_found) {
        printf(COLOR_RED "Erreur : Table '%s' non trouvée dans le fichier %s.\n" COLOR_RESET, table_name, filename);
    }
}


void execute_describe(Statement* statement, const char* filename) {
    describe_table(filename, statement->table_name);
}