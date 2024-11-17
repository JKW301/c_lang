#ifndef CREATE_H
#define CREATE_H

#include "btree.h"
#include "repl.h"

int create_table(const char* table_name, ColumnNode* columns_head, int column_count);

#endif
