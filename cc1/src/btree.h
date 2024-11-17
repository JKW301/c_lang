#ifndef BTREE_H
#define BTREE_H
#include "repl.h"

#define MAX_KEYS 3  

typedef struct BTreeNode {
    int keys[MAX_KEYS];              // Les ID des tables
    char table_names[MAX_KEYS][255]; // Les noms des tables associés
    struct BTreeNode* children[MAX_KEYS + 1]; // Pointeurs vers les enfants
    int num_keys;                    // Nombre actuel de clés dans le nœud
    int is_leaf;                     // 1 si le nœud est une feuille, 0 sinon
} BTreeNode;



BTreeNode* create_btree_node(int is_leaf);

void insert_btree(BTreeNode** root, int key, const char* table_name);
void split_btree_node(BTreeNode** root, BTreeNode* node);
void insert_into_parent(BTreeNode* parent, int key, const char* table_name, BTreeNode* new_child);

void traverse_btree(BTreeNode* root);
void print_btree_level(BTreeNode* root, int depth);
void build_btree_from_csv(const char* filename);
BTreeNode* search_btree(BTreeNode* root, int key);


void save_to_disk(const char* filename, const BTreeNode* root);
void load_from_disk(const char* filename, BTreeNode** root);
void serialize_btree(FILE* file, const BTreeNode* node);
void deserialize_btree(FILE* file, BTreeNode** node);
#endif
