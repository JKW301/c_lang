#include <stdio.h>
#include <string.h>
#include "tableau.h"

// Fonction pour calculer la longueur maximale d'un champ dans un tableau d'étudiants
int max_longueur_nom(struct Etudiant etudiants[], int taille) {
    int max = strlen("Nom");
    for (int i = 0; i < taille; i++) {
        int longueur = strlen(etudiants[i].nom);
        if (longueur > max) {
            max = longueur;
        }
    }
    return max;
}

// Fonction pour afficher une ligne délimitée par des tirets
void afficher_ligne(int largeur_nom, int largeur_age, int largeur_moyenne) {
    printf("+");
    for (int i = 0; i < largeur_nom + 2; i++) printf("-");
    printf("+");
    for (int i = 0; i < largeur_age + 2; i++) printf("-");
    printf("+");
    for (int i = 0; i < largeur_moyenne + 2; i++) printf("-");
    printf("+\n");
}
