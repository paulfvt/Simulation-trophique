#ifndef GRAPHE_H
#define GRAPHE_H

#include <stdbool.h>

#define TAILLE_MAX_NOM_SOMMET 100
#define TAILLE_MAX_LIGNE 256

typedef struct Arrete {
    int indice_sommet_cible;        // Index du prédateur
    float poids_arrete;             // Coefficient de prédation
    struct Arrete *suivant_arrete;  // Pointeur vers l'arête suivante
} Arrete;

// Structure du graphe
typedef struct Graphe {
    int nombre_de_sommets;
    char **noms_des_sommets;          // Tableau des noms des espèces
    float *populations_des_especes;   // Tableau des populations actuelles
    float *populations_initiales;     // Tableau des populations initiales
    int *capacites_portage;           // Tableau des capacités de portage initiales
    float *rythmes_croissance;        // Tableau des rythmes de croissance
    Arrete **liste_d_adjacence;       // Tableau des listes d'adjacence (arêtes sortantes)
} Graphe;

int trouver_indice_sommet(Graphe *graphe, const char *nom_sommet);

void ajouter_arete(Graphe* graphe, int source, int destination, float poids_arrete);

Graphe* lireFichierTexte(const char *nom_fichier_txt);

void afficherInfoGraphe(Graphe *graphe_trophique);

void liberer_graphe(Graphe *graphe_trophique);

bool est_connexe(Graphe *graphe, bool *exclus);

void generer_combinaisons(int n, int k, int start, int depth, int *combinaison, int ***resultats, int *nombre_resultats, int *capacite_resultats);

int calculer_k_connexite(Graphe *graphe);

void afficher_k_connexite(Graphe* graphe);

void mode_manuel(Graphe *graphe);

void menu_recherche(Graphe *graphe_trophique);

void sousMenuSimulation(Graphe *graphe);

Graphe* lireFichierDot(const char *nom_fichier_dot);

void ecrireFichierDot(const Graphe *graphe, const char *nom_fichier_dot);

void afficherGraphiqueEvolution(const char *fichierImage);

void AffichageDesGraphes(Graphe *graphe_trophique, const char *fichierImage);

#endif // GRAPHE_H