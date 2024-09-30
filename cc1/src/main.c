#include <stdio.h>
#include <stdlib.h>
//#include "repl.c"
#include <stdbool.h>
#include <string.h>

#include "repl.h"

/* AJOUTS */

//#include "student.h"

/*FIN AJOUTS*/

/*
int main(int argc, char* argv[], char* envp[]){
  //repl();
  return 0;
  
}
*/


int main(int argc __attribute__((unused)), char* argv[] __attribute__((unused)), char* envp[] __attribute__((unused))) {
  repl();
    return 0;
}
