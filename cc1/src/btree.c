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
        *root = create_btree_node(1);
        (*root)->keys[0] = key;
        strcpy((*root)->table_names[0], table_name);
        (*root)->num_keys = 1;
    } else {
        BTreeNode* current = *root;

        // Si le nœud actuel est une feuille, insérer directement
        if (current->is_leaf) {
            // Trouver la position pour insérer la nouvelle clé
            int i = current->num_keys - 1;
            while (i >= 0 && current->keys[i] > key) {
                current->keys[i + 1] = current->keys[i];
                strcpy(current->table_names[i + 1], current->table_names[i]);
                i--;
            }

            // Insérer la nouvelle clé
            current->keys[i + 1] = key;
            strcpy(current->table_names[i + 1], table_name);
            current->num_keys++;

            printf("Inserted Table ID: %d, Name: %s at leaf\n", key, table_name);

            // Si le nœud est plein, gérer la division
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

            // Vérifier si l'enfant est plein
            if (current->children[i]->num_keys == MAX_KEYS) {
                split_btree_node(root, current->children[i]);
                if (key > current->keys[i]) {
                    i++;
                }
            }

            // Insérer dans l'enfant trouvé
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

    // Transférer les clés et les enfants après le milieu au nouveau nœud
    for (int j = 0; j < new_node->num_keys; j++) {
        new_node->keys[j] = node->keys[mid_index + 1 + j];
        strcpy(new_node->table_names[j], node->table_names[mid_index + 1 + j]);
    }
    if (!node->is_leaf) {
        for (int j = 0; j <= new_node->num_keys; j++) {
            new_node->children[j] = node->children[mid_index + 1 + j];
        }
    }

    node->num_keys = mid_index;

    // Si le nœud à diviser est la racine, créer une nouvelle racine
    if (*root == node) {
        BTreeNode* new_root = create_btree_node(0);
        new_root->keys[0] = mid_key;
        strcpy(new_root->table_names[0], mid_table_name);
        new_root->children[0] = *root;
        new_root->children[1] = new_node;
        new_root->num_keys = 1;
        *root = new_root;
    } else {
        // Propager la clé médiane au parent
        insert_into_parent(*root, mid_key, mid_table_name, new_node);
    }
}

// Fonction pour insérer la clé médiane dans le parent
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

    // Si le parent est plein, le diviser également
    if (parent->num_keys == MAX_KEYS) {
        split_btree_node(&parent, parent);
    }
}

/* Code INSERT TREE */

void build_btree_from_csv(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int table_id;
    char table_name[255];

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "#### Table::", 12) == 0) {
            sscanf(line, "#### Table::%d: %s", &table_id, table_name);
            insert_btree(&btree_root, table_id, table_name);
            printf("Inserting Table ID: %d, Name: %s\n", table_id, table_name);  // Debug
        }
    }

    fclose(file);
}

void print_btree_level(BTreeNode* root, int depth, int is_last) {
    if (root != NULL) {
        for (int i = 0; i < depth; i++) {
            printf(is_last ? "    " : "|   ");
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
                print_btree_level(root->children[i], depth + 1, i == root->num_keys);
            }
        }
    }
}


void traverse_btree(BTreeNode* root) {
    print_btree_level(root, 0, 1);
}
