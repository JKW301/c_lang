#ifndef DESCRIBE_H
#define DESCRIBE_H
#include "repl.h"

void describe_table(const char* filename, const char* table_name);
void execute_describe(Statement* statement, const char* filename);


#endif