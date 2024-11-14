/*
 * Ce programme est un exercice de swap de deux variable
 */

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
int main() {
	int16_t a = 0;
	int16_t b = 1;


	printf("a = %"PRIu16" \nb = %d\n\nAfter swapping :\n", a, b);
	// %"PRIu16" est mieux que %d; nécessite 'inttypes.h' 	

	int16_t c = a;
	a = b;
	b = c;
	printf("a = %d\nb = %d\n", a, b);
exit(EXIT_SUCCESS);	
}

