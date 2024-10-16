#ifndef CREATE_H
#define CREATE_H

#include repl.h

void create_table(const char* filename, const char* table_name, const char** columns, int num_columns);
void execute_createtable(Statement* statement, const char* filename);


#endif