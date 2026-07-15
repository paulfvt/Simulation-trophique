    #ifndef PROJET_RESEAUX_TROPHIQUES_EQUIPE_2E_SIMULATION_H
#define PROJET_RESEAUX_TROPHIQUES_EQUIPE_2E_SIMULATION_H

#include "graphe.h"
#define nombre_choix_menu_parametres 4
#define nombre_choix_menu_simulation 4

void modifier_parametres(Graphe *graphe);

void modifier_parametres_simulation(Graphe *graphe, float *populations_temp, int *capacites_temp, float *rythmes_temp);

void mode_automatique(Graphe *graphe);

void mode_manuel(Graphe *graphe);

void setColor(int color);

void resetColor();

int deplacementMenu(const char* menuItems[], int maxItems, const char* titre_menu);

void sousMenuSimulation(Graphe *graphe);

void generer_et_afficher_graphique_evolution(Graphe *graphe);

#endif //PROJET_RESEAUX_TROPHIQUES_EQUIPE_2E_SIMULATION_H
