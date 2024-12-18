#ifndef REPL_H
#define REPL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "btree.h"
#define MAX_COLUMNS 10 
#define MAX_ROWS 100
#define MAX_VALUE_LEN 255
#define MAX_LINE_LENGTH 255
#define MAX_SELECTED_COLUMNS 255
#define DELIMITER "#### Table:"

#define TYPE_INT 0
#define TYPE_STRING 1


#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_RESET "\033[0m"

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
    STATEMENT_MAN,
    STATEMENT_DESCRIBE,
    STATEMENT_SHOWTABLES,
    STATEMENT_SEARCH,
    STATEMENT_DELETETABLE,
    STATEMENT_SELECT,
    STATEMENT_SHOW_TREE,
    STATEMENT_EXIT,
} StatementType;

typedef struct {
    char name[50];
    int type;
} Column;

typedef struct ColumnNode {
    char name[50];
    int type;
    struct ColumnNode* next;
} ColumnNode;

typedef struct {
    StatementType type;
    char table_name[255];
    char columns[MAX_COLUMNS][MAX_VALUE_LEN];  // Specified columns in the INSERT statement
    char column_name[255];
    char values[MAX_COLUMNS][MAX_VALUE_LEN];   // Values associated with the columns
    char formatted_values[MAX_LINE_LENGTH];    // String of ordered values to be inserted
    char selected_columns[MAX_SELECTED_COLUMNS][255];  // Array to store selected column names
    int num_selected_columns;  // Track the number of selected columns
    int search_key;
    int num_columns;
    int column_count;
    ColumnNode* columns_head;
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

void repl();
// crea
void execute_createtable(Statement* statement, const char* filename);
int create_table(const char* table_name, ColumnNode* columns_head, int column_count);
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
void add_column_value_node(struct ColumnValueNode** head, const char* column_name, const char* value);
struct ColumnValueNode* find_column_value_node(struct ColumnValueNode* head, char* column_name);
void trim_type(char* column_name);

//delete table

#endif
