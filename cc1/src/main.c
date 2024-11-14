#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "repl.h"


int main(int argc __attribute__((unused)), char* argv[] __attribute__((unused)), char* envp[] __attribute__((unused))) {
  
  /*
   * Pour tester les sanitizers :
   * décommentez la ligne ci-dessous 
   */

  //int array[5]; array[10] = 42;
  

  repl();
  return 0;
}
