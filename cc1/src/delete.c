#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "delete.h"
#include "repl.h"


void execute_delete_table(Statement* statement, const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open the file %s.\n", filename);
        return;
    }

    FILE *temp_file = fopen("temp_database.csv", "w");
    if (temp_file == NULL) {
        printf("Error: Unable to create a temporary file.\n");
        fclose(file);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;
    int within_table = 0;

    // Loop through each line, copying content except for the target table
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table:", 11) == 0) {
            char table_name[255];
            sscanf(line, "#### Table: %s", table_name);

            if (strcmp(table_name, statement->table_name) == 0) {
                table_found = 1;
                within_table = 1;
                continue;  // Skip the table header line
            } else {
                within_table = 0;
            }
        }

        // Skip lines within the table to be deleted
        if (within_table) {
            // Stop skipping if a new table header is encountered
            if (strncmp(line, "#### Table:", 11) == 0) {
                within_table = 0;
            } else {
                continue;
            }
        }

        // Write the current line to the temporary file
        fputs(line, temp_file);
    }

    fclose(file);
    fclose(temp_file);

    // Check if the table was found
    if (!table_found) {
        printf("Error: Table '%s' not found.\n", statement->table_name);
        remove("temp_database.csv");  // Remove temp file if table not found
        return;
    }

    // Replace the original file with the temporary file
    remove(filename);
    rename("temp_database.csv", filename);
    printf("Table '%s' deleted successfully.\n", statement->table_name);
}
