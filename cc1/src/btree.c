#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

// Déclare la racine globale du B-arbre
extern BTreeNode* btree_root;

BTreeNode* create_btree_node(int is_leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->num_keys = 0;
    node->is_leaf = is_leaf;

    for (int i = 0; i < MAX_KEYS + 1; i++) {
        node->children[i] = NULL;
    }
    return node;
}

/* Code INSERT TREE */
void insert_btree(BTreeNode** root, int key, const char* table_name) {
    if (*root == NULL) {
        *root = create_btree_node(1);  // Crée un nœud feuille
        (*root)->keys[0] = key;
        strcpy((*root)->table_names[0], table_name);
        (*root)->num_keys = 1;
    } else {
        BTreeNode* current = *root;

        if (current->is_leaf) {
            // Trouver la position d'insertion
            int i = current->num_keys - 1;
            while (i >= 0 && current->keys[i] > key) {
                current->keys[i + 1] = current->keys[i];
                strcpy(current->table_names[i + 1], current->table_names[i]);
                i--;
            }

            // Insérer la clé
            current->keys[i + 1] = key;
            strcpy(current->table_names[i + 1], table_name);
            current->num_keys++;

            // Diviser le nœud si nécessaire
            if (current->num_keys == MAX_KEYS) {
                split_btree_node(root, current);
            }
        } else {
            // Trouver le bon enfant pour l'insertion
            int i = current->num_keys - 1;
            while (i >= 0 && current->keys[i] > key) {
                i--;
            }
            i++;

            // Diviser l'enfant si nécessaire
            if (current->children[i]->num_keys == MAX_KEYS) {
                split_btree_node(root, current->children[i]);
                if (key > current->keys[i]) {
                    i++;
                }
            }

            // Insérer dans l'enfant approprié
            insert_btree(&current->children[i], key, table_name);
        }
    }
}



// Fonction pour diviser un nœud plein
void split_btree_node(BTreeNode** root, BTreeNode* node) {
    int mid_index = MAX_KEYS / 2;
    int mid_key = node->keys[mid_index];
    char mid_table_name[255];
    strcpy(mid_table_name, node->table_names[mid_index]);

    BTreeNode* new_node = create_btree_node(node->is_leaf);
    new_node->num_keys = MAX_KEYS - mid_index - 1;

    // Transférer les clés après la médiane
    for (int j = 0; j < new_node->num_keys; j++) {
        new_node->keys[j] = node->keys[mid_index + 1 + j];
        strcpy(new_node->table_names[j], node->table_names[mid_index + 1 + j]);
    }

    // Transférer les enfants si ce n'est pas une feuille
    if (!node->is_leaf) {
        for (int j = 0; j <= new_node->num_keys; j++) {
            new_node->children[j] = node->children[mid_index + 1 + j];
        }
    }

    node->num_keys = mid_index;

    // Si le nœud divisé est la racine
    if (*root == node) {
        BTreeNode* new_root = create_btree_node(0);
        new_root->keys[0] = mid_key;
        strcpy(new_root->table_names[0], mid_table_name);
        new_root->children[0] = *root;
        new_root->children[1] = new_node;
        new_root->num_keys = 1;
        *root = new_root;
    } else {
        insert_into_parent(*root, mid_key, mid_table_name, new_node);
    }
}

void insert_into_parent(BTreeNode* parent, int key, const char* table_name, BTreeNode* new_child) {
    int i = parent->num_keys - 1;
    while (i >= 0 && parent->keys[i] > key) {
        parent->keys[i + 1] = parent->keys[i];
        strcpy(parent->table_names[i + 1], parent->table_names[i]);
        parent->children[i + 2] = parent->children[i + 1];
        i--;
    }

    parent->keys[i + 1] = key;
    strcpy(parent->table_names[i + 1], table_name);
    parent->children[i + 2] = new_child;
    parent->num_keys++;

    // Diviser le parent si nécessaire
    if (parent->num_keys == MAX_KEYS) {
        split_btree_node(&parent, parent);
    }
}

/* Code INSERT TREE */

void build_btree_from_csv(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        int table_id;
        char table_name[255];

        if (sscanf(line, "#### Table::%d: %s", &table_id, table_name) == 2) {
            insert_btree(&btree_root, table_id, table_name);
            printf("Inserting Table ID: %d, Name: %s\n", table_id, table_name);  // Debug
        }
    }

    fclose(file);
}


void print_btree_level(BTreeNode* root, int depth) {
    if (root != NULL) {
        for (int i = 0; i < depth; i++) {
            printf("|   ");
        }

        printf("|-- [");
        for (int i = 0; i < root->num_keys; i++) {
            printf("%d (%s)", root->keys[i], root->table_names[i]);
            if (i < root->num_keys - 1) {
                printf(", ");
            }
        }
        printf("]\n");

        for (int i = 0; i <= root->num_keys; i++) {
            if (root->children[i] != NULL) {
                print_btree_level(root->children[i], depth + 1);
            }
        }
    }
}


void traverse_btree(BTreeNode* root) {
    print_btree_level(root, 0);
}

BTreeNode* search_btree(BTreeNode* root, int key) {
    if (root == NULL) {
        printf("Erreur : B-tree root is NULL.\n");
        return NULL;
    }

    int i = 0;
    while (i < root->num_keys && key > root->keys[i]) {
        i++;
    }

    if (i < root->num_keys && key == root->keys[i]) {
        printf("Table found: ID %d, Name %s\n", root->keys[i], root->table_names[i]);
        return root;
    }

    if (root->is_leaf) {
        printf("Key %d not found in leaf node.\n", key);
        return NULL;
    }

    printf("Descending to child %d of node with keys: ", i);
    for (int j = 0; j < root->num_keys; j++) {
        printf("%d ", root->keys[j]);
    }
    printf("\n");

    return search_btree(root->children[i], key);
}


