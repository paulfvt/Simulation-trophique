#ifndef ALGO_H
#define ALGO_H

#include "graphe.h"

// Structure de la queue
typedef struct Queue {
    int *data;
    int debut;
    int fin;
    int capacity;
} Queue;

Queue *creer_queue(int capacity);
void enfiler(Queue *queue, int value);
int defiler(Queue *queue);
bool est_vide(Queue *queue);
void liberer_queue(Queue *queue);

void afficher_liste_sommets(Graphe *graphe);

void DFS(Graphe *graphe);

void detectCycle(Graphe *graphe);

void afficher_dependances(Graphe *graphe, int sommet, bool *visite, int niveau);

void afficher_chaines_alimentaires(Graphe *graphe, int sommet_selectionne);

void selectionner_et_afficher_chaines(Graphe *graphe_trophique);

void niveau_trophique(Graphe *graphe_trophique);

void trouver_composantes_connexes(Graphe *graphe);

void lancerCalculPlusCourtsChemin(Graphe *graphe_ecosysteme);

void afficher_chemin(Graphe *graphe, int *precedent, int sommet_final);

void afficher_chaine_descendants_recursif(Graphe *graphe, int indice_espece, bool *visites, int niveau);

void afficher_degrees(Graphe *graphe);

void afficher_relations_interactif(Graphe *graphe);

void afficher_relations_recursif(Graphe *graphe, int indice_espece, bool *visites, int niveau, bool afficher_descendants);

void modifier_parametres(Graphe *graphe);

void modifier_parametres_simulation(Graphe *graphe, float *populations_temp, int *capacites_temp, float *rythmes_temp);

void setColor(int color);

void resetColor();

int deplacementMenu(const char* menuItems[], int maxItems, const char* titre_menu);

void sousMenuSimulation(Graphe *graphe);

void afficher_especes_sans_predateurs(Graphe *graphe);

void afficher_especes_source_unique(Graphe *graphe);

void afficher_superpredateurs(Graphe *graphe);

void afficher_especes_population_seuil(Graphe *graphe);

void menu_recherche(Graphe *graphe_trophique);

#endif // ALGO_H
