#include <stdlib.h>
#include <stdio.h>

void collatz(int val) {
    while (val != 1) {
        if (val % 2 == 0) {
            printf("val est pair\n");
            val = val / 2;
            printf("val est maintenant égale à %d\n", val);
        } else {
            printf("val est impair\n");
            val = val * 3 + 1;
            printf("val est maintenant égale à %d\n", val);
        }
    }
    printf("val est maintenant égale à 1\n");
}

int main() {
    int val;

    printf("Entrez un entier : \n");
    if (scanf("%d", &val) != 1) {
        printf("Erreur de saisie.\n");
        return 1;
    }

    collatz(val);

    return 0;
}
