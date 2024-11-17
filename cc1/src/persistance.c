#include "repl.h"
#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // pour ssize_t
#include <stddef.h>

void save_to_disk(const char* filename, const BTreeNode* root) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Erreur d'ouverture du fichier pour la sauvegarde");
        return;
    }
    serialize_btree(file, root);
    fclose(file);
}

void load_from_disk(const char* filename, BTreeNode** root) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Aucun fichier trouvé. Démarrage avec une base vide.\n");
        return;
    }
    deserialize_btree(file, root);
    fclose(file);
}

void serialize_btree(FILE* file, const BTreeNode* node) {
    if (!node) return;
    fprintf(file, "%d ", node->num_keys);
    for (int i = 0; i < node->num_keys; i++) {
        fprintf(file, "%d ", node->keys[i]);
    }
    fprintf(file, "\n");
    for (int i = 0; i <= node->num_keys; i++) {
        serialize_btree(file, node->children[i]);
    }
}

void deserialize_btree(FILE* file, BTreeNode** node) {
    int num_keys;
    if (fscanf(file, "%d", &num_keys) != 1) return;

    *node = create_btree_node(1); // Assurez-vous que cette fonction existe
    (*node)->num_keys = num_keys;
    for (int i = 0; i < num_keys; i++) {
        fscanf(file, "%d", &((*node)->keys[i]));
    }
    for (int i = 0; i <= num_keys; i++) {
        deserialize_btree(file, &((*node)->children[i]));
    }
}
