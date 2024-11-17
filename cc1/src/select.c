#include "repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <stddef.h>

void print_boxed_line(const char* line) {
    printf("| %s |\n", line);
}

void print_boxed_header_footer(int length) {
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");
}

void execute_select(Statement* statement, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf(COLOR_RED "Erreur : Impossible d'ouvrir le fichier %s.\n" COLOR_RESET, filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;
    int column_indices[MAX_SELECTED_COLUMNS];
    int is_wildcard = (statement->num_selected_columns == 1 && strcmp(statement->selected_columns[0], "*") == 0);

    
    for (int i = 0; i < MAX_SELECTED_COLUMNS; i++) {
        column_indices[i] = -1;
    }

    
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table::", 12) == 0) {
            int table_id;
            char table_name[255];

            
            sscanf(line, "#### Table::%d: %s", &table_id, table_name);

            
            printf("Table ID: %d, Table Name: %s\n", table_id, table_name);

            
            if (strcmp(table_name, statement->table_name) == 0) {
                table_found = 1;
                break;
            }
        }
    }

    if (!table_found) {
        printf(COLOR_RED "Erreur : Table '%s' non trouvée.\n" COLOR_RESET, statement->table_name);
        fclose(file);
        return;
    }

    
    if (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        int index = 0;
        char columns[MAX_COLUMNS][MAX_VALUE_LEN];

        while (token != NULL) {
            trim_whitespace(token);
            trim_type(token);
            strcpy(columns[index], token);

            
            if (is_wildcard) {
                column_indices[index] = index;  
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

        
        for (int i = 0; i < statement->num_selected_columns; i++) {
            if (column_indices[i] == -1 && !is_wildcard) {
                printf(COLOR_RED "Erreur : Colonne '%s' non trouvée dans la table '%s'.\n" COLOR_RESET, statement->selected_columns[i], statement->table_name);
                fclose(file);
                return;
            }
        }

        
        char header_line[MAX_LINE_LENGTH] = "";
        for (int i = 0; i < statement->num_selected_columns; i++) {
            if (is_wildcard || column_indices[i] != -1) {
                if (strlen(header_line) > 0) {
                    strcat(header_line, ", ");
                }
                strcat(header_line, columns[column_indices[i]]);
            }
        }
        print_boxed_header_footer(strlen(header_line));
        print_boxed_line(header_line);
        print_boxed_header_footer(strlen(header_line));
    }

    
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table:", 11) == 0) {
            break; 
        }

        char* token = strtok(line, ",");
        int index = 0;
        int print_needed = 0;
        char row_line[MAX_LINE_LENGTH] = "";

        while (token != NULL) {
            trim_whitespace(token);

            
            for (int i = 0; i < statement->num_selected_columns; i++) {
                if (is_wildcard || index == column_indices[i]) {
                    if (strlen(row_line) > 0) {
                        strcat(row_line, ", ");
                    }
                    strcat(row_line, token);
                    print_needed = 1;
                }
            }

            index++;
            token = strtok(NULL, ",");
        }

        if (print_needed) {
            print_boxed_line(row_line);
        }
    }

    print_boxed_header_footer(strlen(line));
    fclose(file);
}
