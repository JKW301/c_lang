#include <stdio.h>
#include <string.h>

// Définition d'une structure pour représenter un étudiant
struct Etudiant {
    char nom[50];
    int age;
    float moyenne;
};

// Fonction pour calculer la longueur maximale d'un champ dans un tableau d'étudiants
int max_longueur_nom(struct Etudiant etudiants[], int taille) {
    int max = strlen("Nom");  // "Nom" est le titre de la colonne
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

int main() {
    // Création d'un tableau d'étudiants
    struct Etudiant etudiants[3];
    
    // Initialisation manuelle des valeurs pour 3 étudiants
    for (int i = 0; i < 3; i++) {
        printf("Entrez le nom de l'étudiant %d: ", i+1);
        scanf("%s", etudiants[i].nom);
        printf("Entrez l'âge de l'étudiant %d: ", i+1);
        scanf("%d", &etudiants[i].age);
        printf("Entrez la moyenne de l'étudiant %d: ", i+1);
        scanf("%f", &etudiants[i].moyenne);
    }

    // Calcul de la largeur maximale de chaque colonne
    int largeur_nom = max_longueur_nom(etudiants, 3); // Largeur dynamique pour le nom
    int largeur_age = strlen("Age");  // Utilisation du titre "Age" car il est toujours plus large que l'âge lui-même
    int largeur_moyenne = strlen("Moyenne");  // Utilisation du titre "Moyenne"

    // Affichage du tableau avec en-têtes
    afficher_ligne(largeur_nom, largeur_age, largeur_moyenne);
    printf("| %-*s | %-*s | %-*s |\n", largeur_nom, "Nom", largeur_age, "Age", largeur_moyenne, "Moyenne");
    afficher_ligne(largeur_nom, largeur_age, largeur_moyenne);

    // Affichage des données de chaque étudiant
    for (int i = 0; i < 3; i++) {
        printf("| %-*s | %-*d | %-*.2f |\n", largeur_nom, etudiants[i].nom, largeur_age, etudiants[i].age, largeur_moyenne, etudiants[i].moyenne);
        afficher_ligne(largeur_nom, largeur_age, largeur_moyenne);
    }

    return 0;
}
