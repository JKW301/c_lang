#ifndef TABLEAU_H
#define TABLEAU_H

// Structure pour les étudiants
struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
};

// Déclaration des fonctions
int max_longueur_nom(struct Etudiant etudiants[], int taille);
void afficher_ligne(int largeur_nom, int largeur_age, int largeur_moyenne);

#endif
