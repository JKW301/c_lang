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
    printf("db > ");
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
    if (strncmp(input_buffer->buffer, "create_table", 12) == 0) {
        statement->type = STATEMENT_CREATETABLE;
        // Extraire le nom de la table après "create table"
        sscanf(input_buffer->buffer + 13, "%s", statement->table_name);
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "view_tables") == 0) {
        statement->type = STATEMENT_VIEWTABLES;
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "exit") == 0) {
        statement->type = STATEMENT_EXIT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
    static TableList* table_list = NULL;
    static int table_counter = 1; // Compteur pour les tables

    if (table_list == NULL) {
        table_list = create_table_list();
        load_table_list_from_file(table_list, "tables.txt");

        // Mettre à jour le compteur pour éviter les doublons
        Table* current = table_list->head;
        while (current != NULL) {
            if (current->table_number >= table_counter) {
                table_counter = current->table_number + 1;
            }
            current = current->next;
        }
    }

    switch (statement->type) {
        case STATEMENT_CREATETABLE:
            add_table(table_list, table_counter++, statement->table_name);
            printf("Table '%s' created.\n", statement->table_name);
            save_table_list_to_file(table_list, "tables.txt"); // Sauvegarder après création
            break;

        case STATEMENT_VIEWTABLES:
            if (table_list->head == NULL) {
                printf("No tables created yet.\n");
            } else {
                print_table_list(table_list);
            }
            break;

        case STATEMENT_INSERT:
            printf("This is where we would do an insert.\n");
            break;

        case STATEMENT_SELECT:
            printf("This is where we would do a select.\n");
            break;

        case STATEMENT_EXIT:
            printf("Goodbye!\n");
            close_input_buffer(new_input_buffer());
            exit(EXIT_SUCCESS);
            break;

        default:
            printf("Unrecognized command.\n");
            break;
    }
}




// Créer une nouvelle liste de tables
TableList* create_table_list() {
    TableList* list = (TableList*)malloc(sizeof(TableList));
    list->head = NULL;
    return list;
}

// Ajouter une nouvelle table à la liste
void add_table(TableList* list, int table_number, const char* table_name) {
    Table* new_table = (Table*)malloc(sizeof(Table));
    new_table->table_number = table_number;
    strncpy(new_table->table_name, table_name, sizeof(new_table->table_name) - 1);
    new_table->table_name[sizeof(new_table->table_name) - 1] = '\0';
    new_table->next = list->head;
    list->head = new_table;
}

// Sauvegarder la liste de tables dans un fichier
void save_table_list_to_file(TableList* list, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return;
    }
    Table* current = list->head;
    while (current != NULL) {
        fprintf(file, "%d %s\n", current->table_number, current->table_name);
        current = current->next;
    }
    fclose(file);
    printf("Table list saved to %s\n", filename);
}

void load_table_list_from_file(TableList* list, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("No existing table file found, starting with an empty list.\n");
        return;
    }
    int table_number;
    char table_name[255];
    while (fscanf(file, "%d %s", &table_number, table_name) != EOF) {
        add_table(list, table_number, table_name);
    }
    fclose(file);
}


void print_table_list(TableList* list) {
    Table* current = list->head;
    while (current != NULL) {
        printf("Table %d: %s\n", current->table_number, current->table_name);
        current = current->next;
    }
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
        execute_statement(&statement);
        printf("Executed.\n");
    }
}
