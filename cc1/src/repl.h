#ifndef REPL_H
#define REPL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#define MAX_VALUES 10
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_CREATETABLE,
    STATEMENT_VIEWTABLES,
    STATEMENT_SELECT,
    STATEMENT_EXIT,
} StatementType;

typedef struct {
    StatementType type;
    char table_name[255];  // Ajout pour stocker le nom de la table
    char values[MAX_VALUES][255];
} Statement;

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer();
void print_prompt();
void read_input(InputBuffer* input_buffer);
void close_input_buffer(InputBuffer* input_buffer);
MetaCommandResult do_meta_command(InputBuffer* input_buffer);
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);
void execute_statement(Statement* statement);
void repl();

// Structure pour une valeur dans une colonne
struct Valeur {
    char data[50];
    struct Valeur* suivant;
};

// Structure pour une colonne
struct Colonne {
    char nom[50];  // Nom de la colonne
    struct Valeur* valeurs;  // Liste chaînée des valeurs
    struct Colonne* suivante;  // Pointeur vers la colonne suivante
};

// Structure représentant une table
typedef struct Table {
    int table_number;        // Numéro de la table
    char table_name[255];    // Nom de la table
    struct Table* next;      // Pointeur vers la prochaine table (liste chaînée)
    struct Colonne* colonnes;  // Liste chaînée de colonnes
    struct Table* suivante;
} Table;

// Liste de tables (chaînée)
typedef struct {
    Table* head;  // Pointeur vers la première table
} TableList;

// Fonctions pour gérer la liste des tables
TableList* create_table_list();
void add_table(TableList* list, int table_number, const char* table_name);
void save_table_list_to_file(TableList* list, const char* filename);
void load_table_list_from_file(TableList* list, const char* filename);
void print_table_in_frame(const char* table_name);
void print_table_list(TableList* list);
void execute_insert(Statement* statement, TableList* table_list, char data[MAX_VALUES][255]);
void execute_select(Statement* statement, TableList* table_list);

#endif
