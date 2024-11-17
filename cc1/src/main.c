#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "repl.h"
#include "btree.h"

extern BTreeNode* btree_root;

int main(int argc __attribute__((unused)), char* argv[] __attribute__((unused)), char* envp[] __attribute__((unused))) {
    // Charger les données depuis le disque dans l'arbre B
    load_from_disk("database.csv", &btree_root);

    // Lancer REPL
    repl();

    // Sauvegarder les données à la fin
    save_to_disk("database.csv", btree_root);

    return 0;
}