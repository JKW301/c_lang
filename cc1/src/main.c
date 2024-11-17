#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "repl.h"
#include "btree.h"


int main(int argc __attribute__((unused)), char* argv[] __attribute__((unused)), char* envp[] __attribute__((unused))) {
  
  /*
   * Pour tester les sanitizers :
   * décommentez la ligne ci-dessous 
   */

  //int array[5]; array[10] = 42;
  //BTreeNode* btree_root = NULL;

    // Load the B-tree state from disk
  //load_from_disk("database.csv", &btree_root);

  repl();
  //save_to_disk("database.csv", btree_root);
  return 0;
}
