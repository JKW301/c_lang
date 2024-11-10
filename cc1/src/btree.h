#ifndef BTREE_H
#define BTREE_H
#include "repl.h"

#define MAX_KEYS 3  // L'ordre du B-arbre (choisissez un ordre en fonction des besoins)

typedef struct BTreeNode {
    int keys[MAX_KEYS];              // Les ID des tables
    char table_names[MAX_KEYS][255]; // Les noms des tables associés
    struct BTreeNode* children[MAX_KEYS + 1]; // Pointeurs vers les enfants
    int num_keys;                    // Nombre actuel de clés dans le nœud
    int is_leaf;                     // 1 si le nœud est une feuille, 0 sinon
} BTreeNode;



// Fonctions prototypes
BTreeNode* create_btree_node(int is_leaf);
//void insert_btree(BTreeNode** root, int key, const char* table_name);

void insert_btree(BTreeNode** root, int key, const char* table_name);
void split_btree_node(BTreeNode** root, BTreeNode* node);
void insert_into_parent(BTreeNode* parent, int key, const char* table_name, BTreeNode* new_child);

void traverse_btree(BTreeNode* root);
void print_btree_level(BTreeNode* root, int depth);
void build_btree_from_csv(const char* filename);
BTreeNode* search_btree(BTreeNode* root, int key);


#endif // BTREE_H
