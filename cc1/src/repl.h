#ifndef REPL_H
#define REPL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_COLUMNS 10  // Nombre maximum de colonnes dans une table
#define MAX_ROWS 100    // Nombre maximum de lignes dans une table
#define MAX_VALUE_LEN 255  // Longueur maximale d'une valeur
#define MAX_LINE_LENGTH 255
#define DELIMITER "#### Table:"

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
    STATEMENT_DESCRIBE,
    //STATEMENT_USE,
    //STATEMENT_VIEWTABLES,
    STATEMENT_SELECT,
    STATEMENT_EXIT,
} StatementType;

/*
typedef struct {
    StatementType type;
    char table_name[255];  // Nom de la table
    char columns[MAX_COLUMNS][MAX_VALUE_LEN];  // Colonnes spécifiées pour l'insertion
    char values[MAX_COLUMNS][MAX_VALUE_LEN];   // Valeurs pour chaque colonne dans une ligne
    int num_columns;   // ajouté pour insert, erroné ? 
} Statement;
*/
typedef struct {
    StatementType type;
    char table_name[255];
    char columns[MAX_COLUMNS][MAX_VALUE_LEN];  // Specified columns in the INSERT statement
    char values[MAX_COLUMNS][MAX_VALUE_LEN];   // Values associated with the columns
    char formatted_values[MAX_LINE_LENGTH];    // String of ordered values to be inserted
} Statement;


typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

// Structure représentant une table
typedef struct {
    char table_name[255];                    // Nom de la table
    char columns[MAX_COLUMNS][255];          // Noms des colonnes
    int num_columns;                         // Nombre de colonnes
    char rows[MAX_ROWS][MAX_COLUMNS][255];   // Données des lignes (tableau de valeurs)
    int num_rows;                            // Nombre actuel de lignes dans la table
} Table;

// Liste de tables (chaînée)
typedef struct {
    Table tables[MAX_ROWS];    // Ensemble de tables
    int num_tables;            // Nombre de tables
} TableList;

// Structure for the linked list to store columns and their values
struct ColumnValueNode {
    char column_name[255];
    char value[255];
    struct ColumnValueNode* next;
};

// Fonctions de gestion des tables
TableList* create_table_list();
void print_table_list(TableList* list);
/*
news
*/

// crea
void execute_createtable(Statement* statement, const char* filename);
void create_table(const char* filename, const char* table_name, const char** columns, int num_columns);
int table_exists_in_file(const char* filename, const char* table_name);

// desc
void describe_table(const char* filename, const char* table_name);
void execute_describe(Statement* statement, const char* filename);

// select
void execute_select(Statement* statement, const char* filename);

//insert
void execute_insert(Statement* statement, const char* filename);

//
void trim_whitespace(char* str);
void add_column_value_node(struct ColumnValueNode** head, char* column_name, char* value);
struct ColumnValueNode* find_column_value_node(struct ColumnValueNode* head, char* column_name);
void trim_type(char* column_name);

#endif
