#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "graphe.h"
#include "algo.h"
#include "windows.h"
#include <conio.h>

#define nombre_choix_menu_simulation 4
#define nombre_choix_menu_recherche 5
#define nombre_choix_menu_parametres 4

#ifdef _WIN32
#include <direct.h>
#define GETCWD _getcwd
#else
#include <unistd.h>
#define GETCWD getcwd
#endif


// Fonction pour modifier les paramètres hors simulation
void modifier_parametres(Graphe *graphe) {
    const char *menuparametres[] = {
            "Modifier la population (N)",
            "Modifier la capacite de portage (K)",
            "Modifier le rythme de croissance (R)",
            "Retour au menu simulation"
    };

    while (1) {

        int choixParametres = deplacementMenu(menuparametres, nombre_choix_menu_parametres, "===Modifier parametres===");

        if (choixParametres == 4) break;

        int sommet_index;
        printf("\nListe des especes disponibles :\n");
        for (int i = 0; i < graphe->nombre_de_sommets; i++) {
            printf("%d. %s\n", i + 1, graphe->noms_des_sommets[i]);
        }

        printf("\nEntrez le numero de l'espece a modifier : ");
        if (scanf("%d", &sommet_index) != 1 || sommet_index < 1 || sommet_index > graphe->nombre_de_sommets) {
            printf("Entree invalide.\n");
            continue;
        }
        sommet_index -= 1;

        switch (choixParametres) {
            case 1: {
                printf("Entrez la nouvelle population (N) : ");
                int nouvelle_population;
                if (scanf("%d", &nouvelle_population) == 1 && nouvelle_population >= 0) {
                    graphe->populations_des_especes[sommet_index] = nouvelle_population;
                    graphe->populations_initiales[sommet_index] = nouvelle_population;
                    printf("Population modifiee avec succes.\n");
                } else {
                    printf("Entree invalide.\n");
                }
                break;
            }
            case 2: {
                printf("Entrez la nouvelle capacite de portage (K) : ");
                int nouvelle_capacite;
                if (scanf("%d", &nouvelle_capacite) == 1 && nouvelle_capacite > 0) {
                    graphe->capacites_portage[sommet_index] = nouvelle_capacite;
                    printf("Capacite modifiee avec succes.\n");
                } else {
                    printf("Entree invalide.\n");
                }
                break;
            }
            case 3: {
                printf("Entrez le nouveau rythme de croissance (R) : ");
                float nouveau_rythme;
                if (scanf("%f", &nouveau_rythme) == 1 && nouveau_rythme >= 0) {
                    graphe->rythmes_croissance[sommet_index] = nouveau_rythme;
                    printf("Rythme modifie avec succes.\n");
                } else {
                    printf("Entree invalide.\n");
                }
                break;
            }
        }
    }
}


void modifier_parametres_simulation(Graphe *graphe, float *populations_temp, int *capacites_temp, float *rythmes_temp) {
    const char *menuparametres[] = {
            "Modifier la population (N)",
            "Modifier la capacite de portage (K)",
            "Modifier le rythme de croissance (R)",
            "Retour a la simulation"
    };

    if (!graphe) {
        printf("Graphe non initialise.\n");
        return;
    }

    while (1) {
        system("cls");

        int choixParametres = deplacementMenu(menuparametres, nombre_choix_menu_parametres,"===Modifier parametres (Temporaire)===");

        if (choixParametres == 4) break;

        int sommet_index;
        printf("\nListe des especes disponibles :\n");
        for (int i = 0; i < graphe->nombre_de_sommets; i++) {
            printf("%d. %s\n", i + 1, graphe->noms_des_sommets[i]);
        }

        printf("\nEntrez le numero de l'espece a modifier : ");
        if (scanf("%d", &sommet_index) != 1 || sommet_index < 1 || sommet_index > graphe->nombre_de_sommets) {
            printf("Entree invalide.\n");
            continue;
        }
        sommet_index -= 1;

        switch (choixParametres) {
            case 1: {
                printf("Entrez la nouvelle population (N) : ");
                float nouvelle_population;
                if (scanf("%f", &nouvelle_population) == 1 && nouvelle_population >= 0) {
                    populations_temp[sommet_index] = nouvelle_population;
                    printf("Population temporaire modifiee avec succes.\n");
                } else {
                    printf("Entree invalide.\n");
                }
                break;
            }
            case 2: {
                printf("Entrez la nouvelle capacite de portage (K) : ");
                int nouvelle_capacite;
                if (scanf("%d", &nouvelle_capacite) == 1 && nouvelle_capacite > 0) {
                    capacites_temp[sommet_index] = nouvelle_capacite;
                    printf("Capacite temporaire modifiee avec succes.\n");
                } else {
                    printf("Entree invalide.\n");
                }
                break;
            }
            case 3: {
                printf("Entrez le nouveau rythme de croissance (R) : ");
                float nouveau_rythme;
                if (scanf("%f", &nouveau_rythme) == 1 && nouveau_rythme >= 0) {
                    rythmes_temp[sommet_index] = nouveau_rythme;
                    printf("Rythme temporaire modifie avec succes.\n");
                } else {
                    printf("Entree invalide.\n");
                }
                break;
            }
        }
    }
}

void mode_automatique(Graphe *graphe) {
    if (!graphe) {
        printf("Graphe non initialise.\n");
        return;
    }

    float *populations_temp = malloc(graphe->nombre_de_sommets * sizeof(float));
    int *capacites_temp = malloc(graphe->nombre_de_sommets * sizeof(int));
    float *rythmes_temp = malloc(graphe->nombre_de_sommets * sizeof(float));

    if (!populations_temp || !capacites_temp || !rythmes_temp) {
        printf("Erreur d'allocation memoire pour les parametres temporaires.\n");
        free(populations_temp);
        free(capacites_temp);
        free(rythmes_temp);
        return;
    }

    memcpy(populations_temp, graphe->populations_initiales, graphe->nombre_de_sommets * sizeof(float));
    memcpy(capacites_temp, graphe->capacites_portage, graphe->nombre_de_sommets * sizeof(int));
    memcpy(rythmes_temp, graphe->rythmes_croissance, graphe->nombre_de_sommets * sizeof(float));

    int iteration = 0;
    bool pause_simulation = false;

    FILE *fichier = fopen("..//simulation_automatique.csv", "w");
    if (!fichier) {
        printf("Erreur d'ouverture du fichier de simulation.\n");
        free(populations_temp);
        free(capacites_temp);
        free(rythmes_temp);
        return;
    } else {
        printf("Fichier simulation_automatique.csv ouvert avec succes.\n");
    }

    fprintf(fichier, "Iteration");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        fprintf(fichier, ",%s", graphe->noms_des_sommets[i]);
    }
    fprintf(fichier, "\n");
    fflush(fichier);
    printf("En-tête du CSV écrit.\n");

    fprintf(fichier, "0");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        fprintf(fichier, ",%.2f", populations_temp[i]);
    }
    fprintf(fichier, "\n");
    fflush(fichier);
    printf("Populations initiales ecrites dans le CSV.\n");

    system("cls");
    printf("Mode Automatique - Appuyez sur [Espace] pour pause/reprendre, [M] pour modifier les parametres, [Q] pour quitter.\n\n");
    printf("=== Resultats après l'iteration 0 ===\n");

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        printf("  %s : Population = %.2f\n",
               graphe->noms_des_sommets[i],
               populations_temp[i]);
    }

    while (1) {
        if (!pause_simulation) {
            iteration++;
            printf("\n=== Iteration %d ===\n", iteration);

            float *N_new = malloc(graphe->nombre_de_sommets * sizeof(float));
            if (!N_new) {
                printf("Erreur d'allocation mémoire pour N_new.\n");
                free(populations_temp);
                free(capacites_temp);
                free(rythmes_temp);
                fclose(fichier);
                return;
            }

            const float DECLINE_RATE = 0.1f;

            for (int E = 0; E < graphe->nombre_de_sommets; E++) {
                float Nt = populations_temp[E]; // Population actuelle de l'espèce E
                float rE = rythmes_temp[E];      // Taux de croissance de l'espèce E
                int KE = capacites_temp[E];      // Capacité de portage de l'espèce E
                bool dependencies_present = true; // Flag pour vérifier les dépendances

                if (KE <= 0) {
                    printf("  %s : Capacite de portage non valide (K = %d).\n", graphe->noms_des_sommets[E], KE);
                    N_new[E] = Nt;
                    continue;
                }

                for (int j = 0; j < graphe->nombre_de_sommets; j++) {
                    Arrete *arrete = graphe->liste_d_adjacence[j];
                    while (arrete != NULL) {
                        if (arrete->indice_sommet_cible == E) {
                            if (populations_temp[j] <= 0.0f) {
                                dependencies_present = false;
                                break;
                            }
                        }
                        arrete = arrete->suivant_arrete;
                    }
                    if (!dependencies_present) {
                        break;
                    }
                }

                if (dependencies_present) {

                    float croissance_logistique = rE * Nt * (1.0f - (Nt / (float)KE));
                    float NE_new_float = Nt + croissance_logistique;

                    if (NE_new_float > (float)KE) {
                        NE_new_float = (float)KE;
                    }

                    if (NE_new_float < 0.0f) {
                        NE_new_float = 0.0f;
                    }

                    N_new[E] = NE_new_float;

                    printf("  %s : NE(t) = %.2f, Croissance = %.4f, NE(t+1) = %.2f\n",
                           graphe->noms_des_sommets[E],
                           Nt,
                           croissance_logistique,
                           NE_new_float);
                }
                else {

                    float declin = DECLINE_RATE * Nt;
                    float NE_new_float = Nt - declin;
                    if (NE_new_float < 0.0f) NE_new_float = 0.0f;

                    N_new[E] = NE_new_float;

                    printf("  %s : NE(t) = %.2f, Declin = %.4f, NE(t+1) = %.2f\n",
                           graphe->noms_des_sommets[E],
                           Nt,
                           declin,
                           NE_new_float);
                }
            }

/// CSV
            printf("Pret a ecrire l'iteration %d dans le CSV.\n", iteration);
            fprintf(fichier, "%d", iteration);
            for (int i = 0; i < graphe->nombre_de_sommets; i++) {
                populations_temp[i] = N_new[i];
                fprintf(fichier, ",%.2f", populations_temp[i]);
            }
            fprintf(fichier, "\n");
            fflush(fichier);
            printf("Iteration %d ecrite dans le CSV.\n", iteration);
            free(N_new);

            printf("\n=== Populations apres l'Iteration %d ===\n", iteration);
            for (int i = 0; i < graphe->nombre_de_sommets; i++) {
                printf("  %s : Population = %.2f\n",
                       graphe->noms_des_sommets[i],
                       populations_temp[i]);
            }
            printf("\nAppuyez sur [Espace] pour pause/reprendre, [M] pour modifier les parametres, [Q] pour quitter.\n");

            Sleep(1000);
        }

        if (_kbhit()) {
            char touche = _getch();
            if (touche == ' ') {
                pause_simulation = !pause_simulation;
                if (pause_simulation) {
                    printf("\nSimulation en pause.\n");
                }
                else {
                    printf("\nSimulation reprise.\n");
                }
            }
            else if (touche == 'q' || touche == 'Q') {
                printf("\nFin du mode automatique.\n");
                break;
            }
            else if ((touche == 'm' || touche == 'M') && pause_simulation) {
                printf("\nModification des parametres...\n");
                modifier_parametres_simulation(graphe, populations_temp, capacites_temp, rythmes_temp);
                printf("Retour à la simulation. Appuyez sur [Espace] pour pause/reprendre, [M] pour modifier les parametres, [Q] pour quitter.\n");
            }
        }
    }

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        graphe->populations_des_especes[i] = populations_temp[i];
        graphe->capacites_portage[i] = capacites_temp[i];
        graphe->rythmes_croissance[i] = rythmes_temp[i];
    }

    ecrireFichierDot(graphe, "../graphe.dot");

    fclose(fichier);
    printf("Fichier simulation_automatique.csv fermé.\n");

    free(populations_temp);
    free(capacites_temp);
    free(rythmes_temp);
}


void afficher_repertoire_courant() {
    char chemin_courant[FILENAME_MAX];
    if (GETCWD(chemin_courant, sizeof(chemin_courant)) != NULL) {
        printf("Repertoire de travail actuel : %s\n", chemin_courant);
    } else {
        perror("getcwd");
    }
}

void mode_manuel(Graphe *graphe) {
    if (!graphe) {
        printf("Graphe non initialise.\n");
        return;
    }

    afficher_repertoire_courant();

    float *populations_temp = malloc(graphe->nombre_de_sommets * sizeof(float));
    int *capacites_temp = malloc(graphe->nombre_de_sommets * sizeof(int));
    float *rythmes_temp = malloc(graphe->nombre_de_sommets * sizeof(float));

    if (!populations_temp || !capacites_temp || !rythmes_temp) {
        printf("Erreur d'allocation memoire pour les paramètres temporaires.\n");
        free(populations_temp);
        free(capacites_temp);
        free(rythmes_temp);
        return;
    }

    memcpy(populations_temp, graphe->populations_initiales, graphe->nombre_de_sommets * sizeof(float));
    memcpy(capacites_temp, graphe->capacites_portage, graphe->nombre_de_sommets * sizeof(int));
    memcpy(rythmes_temp, graphe->rythmes_croissance, graphe->nombre_de_sommets * sizeof(float));

    int iteration = 0;

    printf("Utilisation des valeurs initiales definies dans le fichier texte.\n");

    FILE *fichier = fopen("..//simulation_automatique.csv", "w");
    if (!fichier) {
        printf("Erreur d'ouverture du fichier de simulation.\n");
        free(populations_temp);
        free(capacites_temp);
        free(rythmes_temp);
        return;
    } else {
        printf("Fichier simulation_automatique.csv ouvert avec succès.\n");
    }

    fprintf(fichier, "Iteration");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        fprintf(fichier, ",%s", graphe->noms_des_sommets[i]);
    }
    fprintf(fichier, "\n");
    fflush(fichier);
    printf("En-tête du CSV écrit.\n");

    fprintf(fichier, "0");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        fprintf(fichier, ",%.2f", populations_temp[i]);
    }
    fprintf(fichier, "\n");
    fflush(fichier);
    printf("Populations initiales ecrites dans le CSV.\n");

    system("cls");

    printf("Mode Manuel - Appuyez sur [D] pour effectuer une iteration, [M] pour modifier les parametres, [Q] pour quitter.\n\n");
    printf("=== Resultats apres l'iteration 0 ===\n");

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        printf("  %s : Population = %.2f\n",
               graphe->noms_des_sommets[i],
               populations_temp[i]);
    }

    while (1) {
        char touche = _getch();

        if (touche == 'q' || touche == 'Q') {
            printf("\nFin du mode manuel.\n");
            break;
        }

        if (touche == 'd' || touche == 'D') {
            printf("Bouton [D] detecte. Début de l'iteration.\n");
            iteration++;
            printf("\n=== Itération %d ===\n", iteration);

            float *N_new = malloc(graphe->nombre_de_sommets * sizeof(float));
            if (!N_new) {
                printf("Erreur d'allocation mémoire pour N_new.\n");
                free(populations_temp);
                free(capacites_temp);
                free(rythmes_temp);
                fclose(fichier);
                return;
            }

            const float DECLINE_RATE = 0.1f;

            for (int E = 0; E < graphe->nombre_de_sommets; E++) {
                float Nt = populations_temp[E]; // Population actuelle de l'espèce E
                float rE = rythmes_temp[E];      // Taux de croissance de l'espèce E
                int KE = capacites_temp[E];      // Capacité de portage de l'espèce E
                bool dependencies_present = true; // Flag pour vérifier les dépendances

                if (KE <= 0) {
                    printf("  %s : Capacite de portage non valide (K = %d).\n", graphe->noms_des_sommets[E], KE);
                    N_new[E] = Nt;
                    continue;
                }

                for (int j = 0; j < graphe->nombre_de_sommets; j++) {
                    Arrete *arrete = graphe->liste_d_adjacence[j];
                    while (arrete != NULL) {
                        if (arrete->indice_sommet_cible == E) {
                            if (populations_temp[j] <= 0.0f) {
                                dependencies_present = false;
                                break;
                            }
                        }
                        arrete = arrete->suivant_arrete;
                    }
                    if (!dependencies_present) {
                        break;
                    }
                }

                if (dependencies_present) {

                    float croissance_logistique = rE * Nt * (1.0f - (Nt / (float)KE));
                    float NE_new_float = Nt + croissance_logistique;

                    if (NE_new_float > (float)KE) {
                        NE_new_float = (float)KE;
                    }

                    if (NE_new_float < 0.0f) {
                        NE_new_float = 0.0f;
                    }

                    N_new[E] = NE_new_float;

                    printf("  %s : NE(t) = %.2f, Croissance = %.4f, NE(t+1) = %.2f\n",
                           graphe->noms_des_sommets[E],
                           Nt,
                           croissance_logistique,
                           NE_new_float);
                }
                else {

                    float declin = DECLINE_RATE * Nt;
                    float NE_new_float = Nt - declin;
                    if (NE_new_float < 0.0f) NE_new_float = 0.0f;

                    N_new[E] = NE_new_float;

                    printf("  %s : NE(t) = %.2f, Declin = %.4f, NE(t+1) = %.2f\n",
                           graphe->noms_des_sommets[E],
                           Nt,
                           declin,
                           NE_new_float);
                }
            }

            printf("Pret à écrire l'iteration %d dans le CSV.\n", iteration);
            fprintf(fichier, "%d", iteration);
            for (int i = 0; i < graphe->nombre_de_sommets; i++) {
                populations_temp[i] = N_new[i];
                fprintf(fichier, ",%.2f", populations_temp[i]);
            }
            fprintf(fichier, "\n");
            fflush(fichier);
            printf("Itération %d ecrite dans le CSV.\n", iteration);
            free(N_new);

            printf("\n=== Populations apres l'iteration %d ===\n", iteration);
            for (int i = 0; i < graphe->nombre_de_sommets; i++) {
                printf("  %s : Population = %.2f\n",
                       graphe->noms_des_sommets[i],
                       populations_temp[i]);
            }
            printf("\nAppuyez sur [D] pour continuer, [M] pour modifier les parametres ou [Q] pour quitter.\n");
        }
        else if (touche == 'm' || touche == 'M') { // Modifier
            printf("\nModification des parametres...\n");
            modifier_parametres_simulation(graphe, populations_temp, capacites_temp, rythmes_temp);
            printf("Retour a la simulation. Appuyez sur [D] pour continuer ou [Q] pour quitter.\n");
        }
    }

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        graphe->populations_des_especes[i] = populations_temp[i];
        graphe->capacites_portage[i] = capacites_temp[i];
        graphe->rythmes_croissance[i] = rythmes_temp[i];
    }

    ecrireFichierDot(graphe, "../graphe.dot");

    fclose(fichier);
    printf("Fichier simulation_automatique.csv fermé.\n");

    free(populations_temp);
    free(capacites_temp);
    free(rythmes_temp);
}
void restaurer_populations_initiales(Graphe *graphe) {
    if (!graphe || !graphe->populations_initiales) return;

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        graphe->populations_des_especes[i] = graphe->populations_initiales[i];
    }
    printf("Les populations ont ete reinitialisees a leurs valeurs initiales.\n");
}

void restaurer_parametres_initiaux(Graphe *graphe) {
    if (!graphe || !graphe->populations_initiales) return;

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        graphe->populations_des_especes[i] = graphe->populations_initiales[i];
        /* graphe->capacites_portage[i] = graphe->capacites_portage_initiales[i];
         graphe->rythmes_croissance[i] = graphe->rythmes_croissance_initiales[i];*/
    }

    printf("Tous les parametres ont ete reinitialises aux valeurs initiales.\n");
}


void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}


void resetColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

int deplacementMenu(const char* menuItems[], int maxItems, const char* titre_menu) {
    int select = 0;
    int key;

    while (1) {

        system("cls");


        if (titre_menu) {
            setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            printf("%s\n\n", titre_menu);
            resetColor();
        }


        for (int i = 0; i < maxItems; i++) {
            if (i == select) {
                setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);

            } else {
                resetColor();
            }
            printf("%s\n", menuItems[i]);
        }

        resetColor();
        key = _getch();

        switch (key) {
            case 72:  // flêche du haut
                if (select > 0) {
                    select--;
                }
                break;

            case 80:  // flêche du bas
                if (select < maxItems - 1) {
                    select++;
                }
                break;

            case 13:  // Validate selection
                return select + 1;
        }
    }
}


// Simulation dynamique avec sous-menu
void sousMenuSimulation(Graphe *graphe) {
    const char* menuSimulation[] = {
            "Mode Automatique",
            "Mode Manuel",
            "Modifier les Parametres",
            "Retour au menu principal"
    };


    while (1) {
        int choixSimulation = deplacementMenu(menuSimulation, nombre_choix_menu_simulation,"===Simulation===");
        system("cls");

        if (choixSimulation == 4) break;

        switch (choixSimulation) {
            case 1:
                mode_automatique(graphe);
                break;
            case 2:
                mode_manuel(graphe);
                break;
            case 3:
                modifier_parametres(graphe);
                break;
        }
    }
}


void generer_et_afficher_graphique_evolution(Graphe *graphe) {
    const char *fichier_csv = "..//simulation_automatique.csv";
    const char *fichier_gnuplot = "plot_population_evolution.gp";
    const char *fichier_png = "population_evolution.png";

    FILE *fichier = fopen(fichier_csv, "r");
    if (!fichier) {
        printf("Erreur : le fichier %s n'existe pas. Veuillez effectuer une simulation d'abord.\n", fichier_csv);
        return;
    }
    fclose(fichier);

    // Créer le script Gnuplot
    FILE *gp = fopen(fichier_gnuplot, "w");
    if (!gp) {
        printf("Erreur : impossible de créer le script Gnuplot.\n");
        return;
    }

    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key autotitle columnheader\n");
    fprintf(gp, "set terminal png size 800,600\n");
    fprintf(gp, "set output '%s'\n", fichier_png);
    fprintf(gp, "set title 'Evolution des Populations'\n");
    fprintf(gp, "set xlabel 'Itérations'\n");
    fprintf(gp, "set ylabel 'Population'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key outside\n");

    // Construire la commande plot
    fprintf(gp, "plot ");
    for (int i = 2; i <= graphe->nombre_de_sommets + 1; i++) { // Colonnes 2 à N+1
        fprintf(gp, "'%s' using 1:%d with lines title columnheader(%d)%s ",
                fichier_csv,
                i,
                i,
                (i < graphe->nombre_de_sommets + 1) ? ", \\\n    " : "\n");
    }

    fclose(gp);

    char commande_gnuplot[512];
    snprintf(commande_gnuplot, sizeof(commande_gnuplot), "gnuplot \"%s\"", fichier_gnuplot);
    int resultat = system(commande_gnuplot);
    if (resultat != 0) {
        printf("Erreur : impossible d'executer Gnuplot.\n");
        return;
    }

    printf("Affichage du graphique d'evolution des populations : %s\n", fichier_png);
    char commande_afficher[512];
    snprintf(commande_afficher, sizeof(commande_afficher), "start \"\" \"%s\"", fichier_png);
    resultat = system(commande_afficher);
    if (resultat == -1) {
        fprintf(stderr, "Erreur : impossible d'exécuter la commande pour afficher le graphique d'evolution des populations.\n");
    }
}