#include "repl.h"
#include "insert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#include <stddef.h>

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_prompt() {
    printf("\n\033[35mdb > \033[0m");  // \033[35m pour magenta, \033[0m pour réinitialiser la couleur
}


void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = '\0';  // Remplacer le caractère de nouvelle ligne
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}



PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "create", 6) == 0) {
        statement->type = STATEMENT_CREATETABLE;
        sscanf(input_buffer->buffer, "create %s", statement->table_name);
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "describe", 8) == 0) {
        statement->type = STATEMENT_DESCRIBE;
        sscanf(input_buffer->buffer, "describe %s", statement->table_name);
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        struct ColumnValueNode* head = NULL;
        statement->type = STATEMENT_INSERT;

        // Extract the table name, columns, and values
        char columns[255], values[255];
        sscanf(input_buffer->buffer, "insert into %s (%[^)]) values (%[^)])", statement->table_name, columns, values);
        printf("Table name: %s\n", statement->table_name);  // Debug

        // Split and clean columns
        char* col_token = strtok(columns, ",");
        while (col_token != NULL) {
            trim_whitespace(col_token);
            trim_type(col_token); // Remove types like ::INT
            printf("Adding column: %s\n", col_token); // Debug
            add_column_value_node(&head, col_token, NULL);  // Add to linked list
            printf("Column added successfully.\n"); // Debug
            col_token = strtok(NULL, ",");
        }

        // Manual parsing of values to avoid issues with quotes
        char* val_start = values;
        char val_buffer[255];
        struct ColumnValueNode* current = head;

        while (*val_start && current != NULL) {
            char* val_end = NULL;

            // Handle quoted values
            if (*val_start == '\'') {
                val_start++;  // Move past the initial quote
                val_end = strchr(val_start, '\'');  // Find closing quote
            } else {
                val_end = strchr(val_start, ',');  // Find comma delimiter for unquoted values
            }

            // If no end delimiter was found, assume it's the end of the string
            if (val_end == NULL) {
                val_end = val_start + strlen(val_start);
            }

            // Copy value into buffer and trim it
            strncpy(val_buffer, val_start, val_end - val_start);
            val_buffer[val_end - val_start] = '\0';
            trim_whitespace(val_buffer);

            // Store the cleaned value in the linked list
            strcpy(current->value, val_buffer);
            printf("Assigned value '%s' to column '%s'\n", val_buffer, current->column_name); // Debug

            // Move to the next value and skip any trailing characters
            val_start = (*val_end == '\'' || *val_end == ',') ? val_end + 1 : val_end;
            current = current->next;
        }

        // Check if values correctly match the columns
        if (current != NULL) {
            printf("Error: Number of values does not match the number of columns.\n");
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }

        // Proceed to file matching and reordering
        FILE *file = fopen("database.csv", "r");
        if (file == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier database.csv.\n");
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }

        char line[MAX_LINE_LENGTH];
        int table_found = 0;
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "#### Table:", 11) == 0) {
                char table_name[255];
                sscanf(line, "#### Table: %s", table_name);
                if (strcmp(table_name, statement->table_name) == 0) {
                    table_found = 1;

                    // Read the next line to get the column order
                    fgets(line, sizeof(line), file);
                    char* csv_col_token = strtok(line, ",");
                    struct ColumnValueNode* ordered_list = NULL;

                    while (csv_col_token != NULL) {
                        trim_whitespace(csv_col_token);
                        trim_type(csv_col_token);
                        printf("Matching CSV column: %s\n", csv_col_token); // Debug

                        struct ColumnValueNode* node = find_column_value_node(head, csv_col_token);
                        if (node != NULL) {
                            printf("Found matching column: %s with value: %s\n", node->column_name, node->value); // Debug
                            add_column_value_node(&ordered_list, node->column_name, node->value);
                        } else {
                            printf("Erreur : Colonne '%s' manquante dans l'insertion.\n", csv_col_token);
                            fclose(file);
                            return PREPARE_UNRECOGNIZED_STATEMENT;
                        }

                        csv_col_token = strtok(NULL, ",");
                    }

                    // Create the final string of ordered values
                    char formatted_values[MAX_LINE_LENGTH] = "";
                    struct ColumnValueNode* current = ordered_list;
                    while (current != NULL) {
                        strcat(formatted_values, current->value);
                        if (current->next != NULL) {
                            strcat(formatted_values, ",");
                        }
                        current = current->next;
                    }

                    strcpy(statement->formatted_values, formatted_values);
                    fclose(file);
                    return PREPARE_SUCCESS;
                }
            }
        }

        fclose(file);
        if (!table_found) {
            printf("Erreur : Table '%s' non trouvée dans le fichier.\n", statement->table_name);
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }
    }

/* ########################################## */
/*!! Commande SELECT !!*/

/* SELECT * : OK
 * SELECT column_name : OK
 * SELECT multiple columns : OK
 */

    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
    statement->type = STATEMENT_SELECT;

    // Extract columns and table name
    char columns[255];
    char table_name[255];
    int args = sscanf(input_buffer->buffer, "select %[^ ] from %s", columns, table_name);

    if (args == 2) {
        // Remove any semicolon at the end of the table name
        char* semicolon_pos = strchr(table_name, ';');
        if (semicolon_pos != NULL) {
            *semicolon_pos = '\0';
        }

        // Split the columns by comma and store them in the Statement struct
        char* col_token = strtok(columns, ",");
        int col_index = 0;

        while (col_token != NULL && col_index < MAX_SELECTED_COLUMNS) {
            trim_whitespace(col_token);  // Remove any extra spaces around the column name
            strcpy(statement->selected_columns[col_index], col_token);
            col_index++;
            col_token = strtok(NULL, ",");
        }

        statement->num_selected_columns = col_index;
        strcpy(statement->table_name, table_name);
        printf("Parsed table name: %s\n", statement->table_name);  // Debug output
        for (int i = 0; i < col_index; i++) {
            printf("Parsed column %d: %s\n", i, statement->selected_columns[i]);  // Debug output
        }
        return PREPARE_SUCCESS;
    }

    printf("Error: Could not parse the SELECT statement.\n");
    return PREPARE_UNRECOGNIZED_STATEMENT;
}
    if (strncmp(input_buffer->buffer, "delete", 6) == 0) {
    statement->type = STATEMENT_DELETETABLE;
    sscanf(input_buffer->buffer, "delete table %s", statement->table_name);
    return PREPARE_SUCCESS;
}


    // Default case for unrecognized commands
    return PREPARE_UNRECOGNIZED_STATEMENT;
}


/* ########################################## */


void execute_statement(Statement* statement, InputBuffer* input_buffer, const char* filename) {
  switch (statement->type) {
    case (STATEMENT_EXIT):
        close_input_buffer(input_buffer);
      exit(EXIT_SUCCESS);
        break;
    case (STATEMENT_CREATETABLE):
            execute_createtable(statement, filename);
            break;
    case (STATEMENT_DESCRIBE):
            execute_describe(statement, filename);
            break;
    case (STATEMENT_SELECT):
            execute_select(statement, filename);
            //printf("select command detected");
            break;
    case (STATEMENT_INSERT):
            execute_insert(statement, filename);
            break;
    case (STATEMENT_DELETETABLE):
            execute_delete_table(statement, filename);
            break;

  }
}



// Add a column-value pair to the linked list
void add_column_value_node(struct ColumnValueNode** head, const char* column_name, const char* value) {
    // Allocate memory for a new node
    struct ColumnValueNode* new_node = (struct ColumnValueNode*)malloc(sizeof(struct ColumnValueNode));
    if (new_node == NULL) {
        printf("Error: Memory allocation failed for new node.\n");
        exit(1);  // Exit if allocation fails
    }

    // Ensure column name and value are copied safely
    strncpy(new_node->column_name, column_name, sizeof(new_node->column_name) - 1);
    new_node->column_name[sizeof(new_node->column_name) - 1] = '\0';  // Null-terminate to avoid overflow

    if (value != NULL) {
        strncpy(new_node->value, value, sizeof(new_node->value) - 1);
        new_node->value[sizeof(new_node->value) - 1] = '\0';  // Null-terminate to avoid overflow
    } else {
        new_node->value[0] = '\0';  // Initialize as empty string if no value is provided
    }

    // Set the next pointer to the current head and then update head to the new node
    new_node->next = *head;
    *head = new_node;
}


// Find a column by name in the linked list
struct ColumnValueNode* find_column_value_node(struct ColumnValueNode* head, char* column_name) {
    struct ColumnValueNode* current = head;
    while (current != NULL) {
        if (strcmp(current->column_name, column_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void trim_whitespace(char* str) {
    // Trim leading spaces
    while (isspace((unsigned char)*str)) str++;
    
    // Trim trailing spaces and quotes
    char* end = str + strlen(str) - 1;
    while (end > str && (isspace((unsigned char)*end) || *end == '\'' || *end == '\"')) end--;
    
    // Write new null terminator
    *(end + 1) = '\0';
}


// Trim the type information (e.g., ::INT) from the column name
void trim_type(char* column_name) {
    char* type_ptr = strstr(column_name, "::");
    if (type_ptr != NULL) {
        *type_ptr = '\0';  // Cut the string at the "::" to remove the type
    }
}



void print_table_in_frame(const char* table_name) {
    int length = strlen(table_name);
    
    // Affichage de la ligne supérieure
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");

    // Affichage du contenu (nom de la table)
    printf("| %s |\n", table_name);

    // Affichage de la ligne inférieure
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");
}

// Fonction pour vérifier si une table existe déjà dans le fichier CSV
int table_exists_in_file(const char* filename, const char* table_name) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0;  // Si le fichier n'existe pas, la table n'existe pas non plus
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0) {
            // Vérification si c'est la table recherchée
            char existing_table_name[255];
            sscanf(line, "#### Table: %254[^\n]", existing_table_name);
            if (strcmp(existing_table_name, table_name) == 0) {
                fclose(file);
                return 1;  // La table existe déjà
            }
        }
    }

    fclose(file);
    return 0;  // La table n'existe pas
}

void repl() {
    InputBuffer* input_buffer = new_input_buffer();
    while (1) {
        print_prompt();
        read_input(input_buffer);
        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }
        const char* filename = "database.csv";  // Define the filename
        execute_statement(&statement, input_buffer, filename);
        printf("Executed.\n");
    }
}

/*

void print_table_list(TableList* list) {
    Table* current = list->head;
    while (current != NULL) {
        print_table_in_frame(current->table_name);  // Encadrer chaque nom de table
        current = current->next;
    }
}

void print_table_in_frame(const char* table_name) {
    int length = strlen(table_name);
    
    // Affichage de la ligne supérieure
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");

    // Affichage du contenu (nom de la table)
    printf("| %s |\n", table_name);

    // Affichage de la ligne inférieure
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");
}
*/