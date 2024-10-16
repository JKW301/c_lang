#include "repl.h"
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
    return PREPARE_UNRECOGNIZED_STATEMENT;


  if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;

        // Extraire le nom de la colonne et de la table
        char column_name[255];
        char table_name[255];
        
        // Exemple d'entrée : "select nom from marshall"
        int args_parsed = sscanf(input_buffer->buffer, "select %s from %s", column_name, table_name);
        
        if (args_parsed < 2) {
            return PREPARE_UNRECOGNIZED_STATEMENT;  // Format incorrect
        }
        
        // Stocker les valeurs dans la structure statement
        strcpy(statement->values[0], column_name);  // Stocke le nom de la colonne dans values[0]
        strcpy(statement->table_name, table_name);  // Stocke le nom de la table

        return PREPARE_SUCCESS;
    }


  return PREPARE_UNRECOGNIZED_STATEMENT;
}



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
            break;
    case (STATEMENT_INSERT):
            execute_insert(statement, filename);
            break;
  }
}

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

// Fonction pour créer une table dans le fichier CSV
void create_table(const char* filename, const char* table_name, const char** columns, int num_columns) {
    // Vérification si la table existe déjà
    if (table_exists_in_file(filename, table_name)) {
        printf("Erreur : La table '%s' existe déjà dans le fichier %s.\n", table_name, filename);
        return;
    }

    // Ouverture du fichier en mode append
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    // Écriture de la nouvelle table
    fprintf(file, "#### Table: %s\n", table_name);
    for (int i = 0; i < num_columns; i++) {
        fprintf(file, "%s", columns[i]);
        if (i < num_columns - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n\n");

    fclose(file);
    printf("Table '%s' créée avec succès dans le fichier %s.\n", table_name, filename);
}

// Exemple d'utilisation dans le main ou la boucle REPL
void execute_createtable(Statement* statement, const char* filename) {
    // Exemple de colonnes pour la nouvelle table
    const char* columns[] = {"id::INT", "nom::string", "email::string"};
    int num_columns = 3;

    create_table(filename, statement->table_name, columns, num_columns);
}


// Fonction pour décrire les colonnes d'une table
void describe_table(const char* filename, const char* table_name) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;

    // Parcourir le fichier pour trouver la table
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0) {
            // Vérifier si c'est la table recherchée
            char existing_table_name[255];
            sscanf(line, "#### Table: %254[^\n]", existing_table_name);
            if (strcmp(existing_table_name, table_name) == 0) {
                table_found = 1;
                // Lire la ligne suivante contenant les colonnes
                if (fgets(line, sizeof(line), file)) {
                    printf("Colonnes de la table '%s':\n", table_name);
                    char* token = strtok(line, ",");
                    while (token != NULL) {
                        printf("  %s\n", token);  // Affiche chaque colonne
                        token = strtok(NULL, ",");
                    }
                }
                break;
            }
        }
    }

    fclose(file);

    if (!table_found) {
        printf("Erreur : Table '%s' non trouvée dans le fichier %s.\n", table_name, filename);
    }
}

// Exemple d'utilisation dans le REPL
void execute_describe(Statement* statement, const char* filename) {
    describe_table(filename, statement->table_name);
}

// Fonction pour sélectionner et afficher les valeurs d'une colonne donnée dans une table
void select_from_table(const char* filename, const char* table_name, const char* column_name) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_found = 0;
    int column_index = -1;
    char columns[MAX_LINE_LENGTH][255];  // Stockage des noms de colonnes
    int num_columns = 0;

    // Étape 1 : Trouver la table
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0) {
            char existing_table_name[255];
            sscanf(line, "#### Table: %254[^\n]", existing_table_name);

            if (strcmp(existing_table_name, table_name) == 0) {
                table_found = 1;

                // Étape 2 : Lire les colonnes et trouver l'index de la colonne demandée
                if (fgets(line, sizeof(line), file)) {
                    char* token = strtok(line, ",");
                    while (token != NULL) {
                        strcpy(columns[num_columns], token);  // Stocker le nom de chaque colonne
                        if (strncmp(token, column_name, strlen(column_name)) == 0) {
                            column_index = num_columns;  // Trouver l'index de la colonne demandée
                        }
                        num_columns++;
                        token = strtok(NULL, ",");
                    }

                    if (column_index == -1) {
                        printf("Erreur : Colonne '%s' non trouvée dans la table '%s'.\n", column_name, table_name);
                        fclose(file);
                        return;
                    }

                    // Étape 3 : Lire les lignes de données et afficher la valeur de la colonne demandée
                    printf("Valeurs de la colonne '%s' dans la table '%s':\n", column_name, table_name);
                    while (fgets(line, sizeof(line), file)) {
                        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0 || line[0] == '\n') {
                            break;  // Fin de la table
                        }
                        
                        // Extraire les valeurs de chaque colonne dans la ligne
                        token = strtok(line, ",");
                        int current_column = 0;
                        while (token != NULL) {
                            if (current_column == column_index) {
                                printf("  %s\n", token);  // Afficher la valeur de la colonne sélectionnée
                            }
                            current_column++;
                            token = strtok(NULL, ",");
                        }
                    }
                }
                break;
            }
        }
    }

    fclose(file);

    if (!table_found) {
        printf("Erreur : Table '%s' non trouvée dans le fichier %s.\n", table_name, filename);
    }
}

// Exemple d'utilisation dans le REPL
void execute_select(Statement* statement, const char* filename) {
    select_from_table(filename, statement->table_name, statement->values[0]);  // Le nom de la colonne est dans values[0]
}

/*



*/
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