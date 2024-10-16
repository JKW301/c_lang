#ifndef SELECT_H
#define SELECT_H
#include repl.h

void select_from_table(const char* filename, const char* table_name, const char* column_name);
void execute_select(Statement* statement, const char* filename);


#endif