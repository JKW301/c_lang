#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

// Fonction pour obtenir un entier depuis l'utilisateur
int obtenir_entier(char* prompt) {
    int valeur;
    char buffer[50];

    while (1) {
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &valeur) == 1) {
            break;  // Entrée valide
        } else {
            printf("Erreur : veuillez entrer un entier valide.\n");
        }
    }

    return valeur;
}

// Fonction pour obtenir un flottant depuis l'utilisateur
float obtenir_flottant(char* prompt) {
    float valeur;
    char buffer[50];

    while (1) {
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%f", &valeur) == 1) {
            break;  // Entrée valide
        } else {
            printf("Erreur : veuillez entrer une valeur décimale valide.\n");
        }
    }

    return valeur;
}

int main() {
    // Création d'un tableau d'étudiants
    struct Etudiant etudiants[3];
    
    // Initialisation manuelle des valeurs pour 3 étudiants
    for (int i = 0; i < 3; i++) {
        printf("Entrez le nom de l'étudiant %d: ", i + 1);
        fgets(etudiants[i].nom, sizeof(etudiants[i].nom), stdin);
        etudiants[i].nom[strcspn(etudiants[i].nom, "\n")] = 0;  // Enlève le saut de ligne

        etudiants[i].age = obtenir_entier("Entrez l'âge de l'étudiant : ");
        etudiants[i].moyenne = obtenir_flottant("Entrez la moyenne de l'étudiant : ");
    }

    // Calcul de la largeur maximale de chaque colonne
    int largeur_nom = max_longueur_nom(etudiants, 3); // Largeur dynamique pour le nom
    int largeur_age = strlen("Age");  // Utilisation du titre "Age"
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
