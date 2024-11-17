#include "repl.h"
#include "delete.h"
#include "insert.h"
#include "btree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <stddef.h>


BTreeNode* btree_root = NULL;

void execute_show_tree() {
    
    btree_root = NULL;  
    build_btree_from_csv("database.csv");
    printf("B-Tree Structure:\n");
    traverse_btree(btree_root);
}
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_prompt() {
    printf("\n\033[35mdb > \033[0m");  
}


void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = '\0';  
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
        printf("Commande 'create' détectée.\n"); 
        statement->type = STATEMENT_CREATETABLE;

        char columns_definition[256];
        if (sscanf(input_buffer->buffer, "create %s (%[^)])", statement->table_name, columns_definition) == 2) {
            printf("Nom de la table : %s\n", statement->table_name);  
            printf("Définition des colonnes : %s\n", columns_definition);  

            char* token = strtok(columns_definition, ",");
            ColumnNode* last_node = NULL;

            while (token != NULL) {
                ColumnNode* new_node = malloc(sizeof(ColumnNode));
                if (!new_node) {
                    perror("Erreur d'allocation mémoire pour ColumnNode");
                    return PREPARE_UNRECOGNIZED_STATEMENT;
                }

                char col_type_str[10];
                
                if (sscanf(token, "%[^:]::%s", new_node->name, col_type_str) == 2 ||
                    sscanf(token, "%[^:]:%s", new_node->name, col_type_str) == 2) {
                    
                    
                    if (strcmp(col_type_str, "INT") == 0) {
                        new_node->type = TYPE_INT;
                    } else if (strcmp(col_type_str, "STRING") == 0 || strcmp(col_type_str, "string") == 0) {
                        new_node->type = TYPE_STRING;
                    } else {
                        printf("Type de colonne inconnu : %s\n", col_type_str);
                        free(new_node);
                        return PREPARE_UNRECOGNIZED_STATEMENT;
                    }
                    new_node->next = NULL;

                    
                    if (last_node == NULL) {
                        statement->columns_head = new_node;
                    } else {
                        last_node->next = new_node;
                    }
                    last_node = new_node;
                } else {
                    printf("Erreur de parsing pour la colonne : %s\n", token);
                    free(new_node);
                    return PREPARE_UNRECOGNIZED_STATEMENT;
                }
                token = strtok(NULL, ",");
            }
            return PREPARE_SUCCESS;
        }
    }

    if (strncmp(input_buffer->buffer, "man", 3) == 0 || strncmp(input_buffer->buffer, "help", 4) ==0) {
        statement->type = STATEMENT_MAN;
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "show tables", 11) == 0) {
    statement->type = STATEMENT_SHOWTABLES;
    return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "exit", 4) == 0 || strncmp(input_buffer->buffer, "ex", 2) == 0) {
        statement->type = STATEMENT_EXIT;
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "describe", 8) == 0) {
        statement->type = STATEMENT_DESCRIBE;
        sscanf(input_buffer->buffer, "describe %s", statement->table_name);
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "search", 6) == 0) {
        statement->type = STATEMENT_SEARCH;
        sscanf(input_buffer->buffer, "search %d", &statement->search_key);
        printf("Recherche de la clé : %d\n", statement->search_key);  
        return PREPARE_SUCCESS;
    }


    if (strncmp(input_buffer->buffer, "show tree", 9) == 0) {
        statement->type = STATEMENT_SHOW_TREE;
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        struct ColumnValueNode* head = NULL;
        statement->type = STATEMENT_INSERT;

        
        char columns[255], values[255];
        sscanf(input_buffer->buffer, "insert into %s (%[^)]) values (%[^)])", statement->table_name, columns, values);
        printf("Table name: %s\n", statement->table_name);  

        
        char* col_token = strtok(columns, ",");
        while (col_token != NULL) {
            trim_whitespace(col_token);
            trim_type(col_token); 
            printf("Adding column: %s\n", col_token); 
            add_column_value_node(&head, col_token, NULL);  
            printf("Column added successfully.\n"); 
            col_token = strtok(NULL, ",");
        }

        
        char* val_start = values;
        char val_buffer[255];
        struct ColumnValueNode* current = head;

        while (*val_start && current != NULL) {
            char* val_end = NULL;

            
            if (*val_start == '\'') {
                val_start++;  
                val_end = strchr(val_start, '\'');  
            } else {
                val_end = strchr(val_start, ',');  
            }

            
            if (val_end == NULL) {
                val_end = val_start + strlen(val_start);
            }

            
            strncpy(val_buffer, val_start, val_end - val_start);
            val_buffer[val_end - val_start] = '\0';
            trim_whitespace(val_buffer);

            
            strcpy(current->value, val_buffer);
            printf("Assigned value '%s' to column '%s'\n", val_buffer, current->column_name); 

            
            val_start = (*val_end == '\'' || *val_end == ',') ? val_end + 1 : val_end;
            current = current->next;
        }

        
        if (current != NULL) {
            printf("Error: Number of values does not match the number of columns.\n");
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }

        
        FILE *file = fopen("database.csv", "r");
        if (file == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier database.csv.\n");
            return PREPARE_UNRECOGNIZED_STATEMENT;
        }

        char line[MAX_LINE_LENGTH];
        int table_found = 0;
        while (fgets(line, sizeof(line), file)) {
            int tableid;
            if (strncmp(line, "#### Table::", 11) == 0) {
                char table_name[255];
                sscanf(line, "#### Table::%d: %s", &tableid, table_name);
                if (strcmp(table_name, statement->table_name) == 0) {
                    table_found = 1;

                    
                    fgets(line, sizeof(line), file);
                    char* csv_col_token = strtok(line, ",");
                    struct ColumnValueNode* ordered_list = NULL;

                    while (csv_col_token != NULL) {
                        trim_whitespace(csv_col_token);
                        trim_type(csv_col_token);
                        printf("Matching CSV column: %s\n", csv_col_token); 

                        struct ColumnValueNode* node = find_column_value_node(head, csv_col_token);
                        if (node != NULL) {
                            printf("Found matching column: %s with value: %s\n", node->column_name, node->value); 
                            add_column_value_node(&ordered_list, node->column_name, node->value);
                        } else {
                            printf("Erreur : Colonne '%s' manquante dans l'insertion.\n", csv_col_token);
                            fclose(file);
                            return PREPARE_UNRECOGNIZED_STATEMENT;
                        }

                        csv_col_token = strtok(NULL, ",");
                    }

                    
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



    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
    statement->type = STATEMENT_SELECT;

    
    char columns[255];
    char table_name[255];
    int args = sscanf(input_buffer->buffer, "select %[^ ] from %s", columns, table_name);

    if (args == 2) {
        
        char* semicolon_pos = strchr(table_name, ';');
        if (semicolon_pos != NULL) {
            *semicolon_pos = '\0';
        }

        
        char* col_token = strtok(columns, ",");
        int col_index = 0;

        while (col_token != NULL && col_index < MAX_SELECTED_COLUMNS) {
            trim_whitespace(col_token);  
            strcpy(statement->selected_columns[col_index], col_token);
            col_index++;
            col_token = strtok(NULL, ",");
        }

        statement->num_selected_columns = col_index;
        strcpy(statement->table_name, table_name);
        printf("Parsed table name: %s\n", statement->table_name);  
        for (int i = 0; i < col_index; i++) {
            printf("Parsed column %d: %s\n", i, statement->selected_columns[i]);  
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
    
    return PREPARE_UNRECOGNIZED_STATEMENT;
}




void execute_statement(Statement* statement, InputBuffer* input_buffer, const char* filename) {
  switch (statement->type) {
    case (STATEMENT_EXIT):
        save_to_disk(filename, btree_root);
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
        break;
    case (STATEMENT_CREATETABLE):
            if (create_table(statement->table_name, statement->columns_head, statement->column_count) == 0) {
                printf(COLOR_GREEN "Table '%s' créée avec succès.\n" COLOR_RESET, statement->table_name);

                save_to_disk(filename, btree_root);
            } else {
                printf(COLOR_RED "Erreur lors de la création de la table '%s'.\n" COLOR_RESET, statement->table_name);
            }
            break;
    case (STATEMENT_DESCRIBE):
            execute_describe(statement, filename);
            break;
    case (STATEMENT_SELECT):
            execute_select(statement, filename);
            
            break;
    case (STATEMENT_SHOW_TREE):
            execute_show_tree();
            break;
    case STATEMENT_SEARCH:
            if (btree_root == NULL) {
                printf("Initialisation du B-arbre à partir de 'database.csv'...\n");
                build_btree_from_csv("database.csv"); 
            }
            BTreeNode* result = search_btree(btree_root, statement->search_key);
            if (result == NULL) {
                printf(COLOR_RED "Table with ID %d not found.\n" COLOR_RESET, statement->search_key);
            }
            break;
    case (STATEMENT_INSERT):
            execute_insert(statement, filename);
            save_to_disk(filename, btree_root);
            break;
    case (STATEMENT_DELETETABLE):
            execute_delete_table(statement, filename);
            save_to_disk(filename, btree_root);
            break;
    case (STATEMENT_SHOWTABLES):
            printf("Liste des tables disponibles :\n");

            
            FILE* file = fopen("database.csv", "r");
            if (file == NULL) {
                perror(COLOR_RED "Erreur lors de l'ouverture du fichier database.csv" COLOR_RESET);
                break;
            }

            char line[MAX_LINE_LENGTH];
            while (fgets(line, sizeof(line), file)) {
                if (strncmp(line, "#### Table::", 12) == 0) {
                    
                    char* table_name = strrchr(line, ':') + 1; 
                    if (table_name != NULL) {
                        printf("- %s", table_name); 
                    }
                }
            }

            fclose(file);
            break;
    case STATEMENT_MAN:
            printf("\033[1mGuide des commandes disponibles :\033[0m\n");
            printf("\033[36m1. CREATE TABLE\033[0m\n");
            printf("   Syntaxe : create <table_name> (<column1>::<type>, <column2>::<type>, ...)\n");
            printf("   Description : Crée une nouvelle table avec les colonnes spécifiées.\n");
            printf("\n");
            
            printf("\033[36m2. INSERT\033[0m\n");
            printf("   Syntaxe : insert into <table_name> (<column1>, <column2>, ...) values (<value1>, <value2>, ...)\n");
            printf("   Description : Insère une ligne dans une table existante.\n");
            printf("\n");
            
            printf("\033[36m3. SELECT\033[0m\n");
            printf("   Syntaxe : select <columns> from <table_name>\n");
            printf("   Description : Sélectionne des colonnes spécifiques dans une table.\n");
            printf("\n");
            
            printf("\033[36m4. SHOW TABLES\033[0m\n");
            printf("   Syntaxe : show tables\n");
            printf("   Description : Affiche toutes les tables disponibles dans la base de données.\n");
            printf("\n");
            
            printf("\033[36m5. DELETE TABLE\033[0m\n");
            printf("   Syntaxe : delete <table_name>\n");
            printf("   Description : Supprime une table existante.\n");
            printf("\n");

            printf("\033[36m6. SHOW TREE\033[0m\n");
            printf("    Description : Affiche la structure de l'arbre B.\n");

            printf("\033[36m7. SEARCH\033[0m\n");
            printf("    Syntaxe : search <table ID>\n");
            printf("    Description : A l'aide du bTree, recherche et retourne la table correspondant à l'ID demandée.\n");
            
            printf("\033[36m8. EXIT\033[0m\n");
            printf("   Syntaxe : 'exit' ou 'ex'\n");
            printf("   Description : Quitte le programme.\n");
            printf("\n");
            break;

        default:
            printf("\033[31mCommande non reconnue.\033[0m\n");
            break;
    
    }
}
void free_columns_list(ColumnNode* head) {
    while (head != NULL) {
        ColumnNode* temp = head;
        head = head->next;
        free(temp);
    }
}




void add_column_value_node(struct ColumnValueNode** head, const char* column_name, const char* value) {
    
    struct ColumnValueNode* new_node = (struct ColumnValueNode*)malloc(sizeof(struct ColumnValueNode));
    if (new_node == NULL) {
        printf(COLOR_RED "Error: Memory allocation failed for new node.\n" COLOR_RESET);
        exit(1);  
    }

    
    strncpy(new_node->column_name, column_name, sizeof(new_node->column_name) - 1);
    new_node->column_name[sizeof(new_node->column_name) - 1] = '\0';  

    if (value != NULL) {
        strncpy(new_node->value, value, sizeof(new_node->value) - 1);
        new_node->value[sizeof(new_node->value) - 1] = '\0';  
    } else {
        new_node->value[0] = '\0';  
    }

    
    new_node->next = *head;
    *head = new_node;
}



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
    
    while (isspace((unsigned char)*str)) str++;
    
    
    char* end = str + strlen(str) - 1;
    while (end > str && (isspace((unsigned char)*end) || *end == '\'' || *end == '\"')) end--;
    
    
    *(end + 1) = '\0';
}



void trim_type(char* column_name) {
    char* type_ptr = strstr(column_name, "::");
    if (type_ptr != NULL) {
        *type_ptr = '\0';  
    }
}



void print_table_in_frame(const char* table_name) {
    int length = strlen(table_name);
    
    
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");

    
    printf("| %s |\n", table_name);

    
    printf("+");
    for (int i = 0; i < length + 2; i++) {
        printf("-");
    }
    printf("+\n");
}


int table_exists_in_file(const char* filename, const char* table_name) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0;  
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, DELIMITER, strlen(DELIMITER)) == 0) {
            
            char existing_table_name[255];
            sscanf(line, "#### Table: %254[^\n]", existing_table_name);
            if (strcmp(existing_table_name, table_name) == 0) {
                fclose(file);
                return 1;  
            }
        }
    }

    fclose(file);
    return 0;  
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
                printf("\033[31mCommande non reconnue : '%s'.\033[0m\n", input_buffer->buffer);
                printf("Tapez 'man' ou 'help' pour voir les commandes disponibles.\n");
                continue;
        }
        const char* filename = "database.csv";  
        execute_statement(&statement, input_buffer, filename);
        printf("Executed.\n");
    }
}
