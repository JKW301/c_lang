#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

void menu() {
    int choix;

    do {
        printf("\n====== MENU ======\n");
        printf("1. Option 1 (N/A)\n");
        printf("2. Option 2 (N/A)\n");
        printf("3. Option 3 (N/A)\n");
        printf("4. QUITTER\n");
        printf("===================\n");
        printf("Votre choix : ");
        
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                printf("Option 1 sélectionnée (N/A)\n");
                break;
            case 2:
                printf("Option 2 sélectionnée (N/A)\n");
                break;
            case 3:
                printf("Option 3 sélectionnée (N/A)\n");
                break;
            case 4:
                printf("Quitter le programme...\n");
                break;
            default:
                printf("Choix invalide, veuillez réessayer.\n");
        }
    } while (choix != 4);
    
    printf("Programme terminé.\n");
}
