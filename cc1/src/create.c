#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"


#include "create.h"
#include "btree.h"


extern BTreeNode* btree_root;


int generate_table_id(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return 1;  
    }

    int max_id = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table::", 12) == 0) {
            int table_id;
            sscanf(line, "#### Table::%d:", &table_id);
            if (table_id > max_id) {
                max_id = table_id;
            }
        }
    }
    fclose(file);
    return max_id + 1;  
}

int create_table(const char* table_name, ColumnNode* columns_head, int column_count) {
    (void) column_count;
    FILE* file = fopen("database.csv", "a");
    if (file == NULL) {
        perror(COLOR_RED "Erreur lors de l'ouverture du fichier database.csv" COLOR_RESET);
        return -1;
    }

    int table_id = generate_table_id("database.csv");
    fprintf(file, "#### Table::%d: %s\n", table_id, table_name);

    ColumnNode* current = columns_head;
    while (current != NULL) {
        fprintf(file, "%s::%s", current->name, current->type == TYPE_INT ? "INT" : "STRING");
        if (current->next != NULL) {
            fprintf(file, ",");
        }
        current = current->next;
    }
    fprintf(file, "\n\n");

    fclose(file);
    printf("Table créée : %s avec ID %d\n", table_name, table_id);
    return 0;
}
