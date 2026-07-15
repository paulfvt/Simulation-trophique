#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include "graphe.h"
#include "algo.h"
#include <conio.h>

#define nombre_choix_menu_recherche 5

// Création d'une queue
Queue *creer_queue(int capacity) {
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->data = (int *) malloc(capacity * sizeof(int));
    queue->debut = 0;
    queue->fin = -1;
    queue->capacity = capacity;
    return queue;
}

// Enfiler un élément dans la queue
void enfiler(Queue *queue, int value) {
    queue->data[++queue->fin] = value;
}

// Défiler un élément de la queue
int defiler(Queue *queue) {
    return queue->data[queue->debut++];
}

// Vérifier si la queue est vide
bool est_vide(Queue *queue) {
    return queue->debut > queue->fin;
}

void liberer_queue(Queue *queue) {
    if (!queue) return;
    free(queue->data);
    free(queue);
}


void afficher_liste_sommets(Graphe *graphe) {

    printf("\n=== Liste des sommets disponibles ===\n");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        printf("%d. %s\n", i + 1, graphe->noms_des_sommets[i]);
    }
    printf("\n");
}


// Fonction utilitaire pour DFS
void DFS_util(Graphe *graphe, int sommet, int *visited) {
    visited[sommet] = 1;
    printf(" -> %s ", graphe->noms_des_sommets[sommet]);
    Arrete *arrete = graphe->liste_d_adjacence[sommet];
    while (arrete != NULL) {
        if (!visited[arrete->indice_sommet_cible]) {
            DFS_util(graphe, arrete->indice_sommet_cible, visited);
        }
        arrete = arrete->suivant_arrete;
    }
}

// Fonction DFS
void DFS(Graphe *graphe) {
    int sommet_debut;
    printf("Entrez l'ID de l'espece pour le parcours DFS (1-%d) : ", graphe->nombre_de_sommets);

    // Lire l'entrée directement et vérifier sa validité
    if (scanf("%d", &sommet_debut) != 1 || sommet_debut < 1 || sommet_debut > graphe->nombre_de_sommets) {
        printf("Entree invalide ou ID de sommet hors limites.\n");
        return;
    }

    // Ajuster l'ID
    sommet_debut -= 1;

    int *visited = calloc(graphe->nombre_de_sommets, sizeof(int));

    printf("Parcours DFS a partir de '%s' : ", graphe->noms_des_sommets[sommet_debut]);
    DFS_util(graphe, sommet_debut, visited);
    printf("\n");

    free(visited);
}


bool detectCycle_util(Graphe *graphe, int sommet, int *visited, int *chemin, int *index_chemin, int sommet_origine, int *cycles_trouves) {
    if (*cycles_trouves >= 2) return true; // Arrêter après 2 cycles trouvés

    visited[sommet] = 1;
    chemin[*index_chemin] = sommet;
    (*index_chemin)++;

    Arrete *arrete = graphe->liste_d_adjacence[sommet];
    while (arrete) {
        int voisin = arrete->indice_sommet_cible;

        if (voisin == sommet_origine) {
            // Cycle circulaire détecté
            printf("Cycle circulaire detecte : ");
            for (int i = 0; i < *index_chemin; i++) {
                printf("%s -> ", graphe->noms_des_sommets[chemin[i]]);
            }
            printf("%s\n", graphe->noms_des_sommets[chemin[0]]);
            (*cycles_trouves)++;
            if (*cycles_trouves >= 2) return true; // Arrêter après 2 cycles trouvés
        }

        if (!visited[voisin]) {
            if (detectCycle_util(graphe, voisin, visited, chemin, index_chemin, sommet_origine, cycles_trouves)) {
                return true;
            }
        }

        arrete = arrete->suivant_arrete;
    }

    // Retour arrière
    (*index_chemin)--;
    visited[sommet] = 0;

    return false;
}

// Fonction principale pour détecter des cycles circulaires dans le graphe
void detectCycle(Graphe *graphe) {

    int *visited = calloc(graphe->nombre_de_sommets, sizeof(int));
    int *chemin = malloc(graphe->nombre_de_sommets * sizeof(int));
    int index_chemin = 0;
    int cycles_trouves = 0; // Compteur de cycles trouvés


    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (cycles_trouves >= 2) break; // Arrêter après 2 cycles trouvés
        if (detectCycle_util(graphe, i, visited, chemin, &index_chemin, i, &cycles_trouves)) {
            break; // Arrêter dès que 2 cycles sont détectés
        }
    }

    if (cycles_trouves == 0) {
        printf("Aucun cycle circulaire detecte dans le graphe.\n");
    }

    free(visited);
    free(chemin);
}


void calculer_niveaux_trophiques(Graphe *graphe, int *trophic_level) {
    if (!graphe || !trophic_level) return;

    int *in_degree = calloc(graphe->nombre_de_sommets, sizeof(int));

    // Calculer le degré d'entrée de chaque sommet
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        Arrete *arrete = graphe->liste_d_adjacence[i];
        while (arrete != NULL) {
            in_degree[arrete->indice_sommet_cible]++;
            arrete = arrete->suivant_arrete;
        }
    }

    // Initialiser la queue avec les sommets de base (in_degree == 0)
    Queue *queue = creer_queue(graphe->nombre_de_sommets);
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (in_degree[i] == 0) {
            enfiler(queue, i);
            trophic_level[i] = 1; // Niveau trophique de base
        } else {
            trophic_level[i] = 0; // Non encore assigné
        }
    }

    // BFS pour assigner les niveaux trophiques
    while (!est_vide(queue)) {
        int sommet = defiler(queue);
        Arrete *arrete = graphe->liste_d_adjacence[sommet];
        while (arrete != NULL) {
            int voisin = arrete->indice_sommet_cible;
            in_degree[voisin]--;
            if (in_degree[voisin] == 0) {
                enfiler(queue, voisin);
                trophic_level[voisin] = trophic_level[sommet] + 1;
            }
            arrete = arrete->suivant_arrete;
        }
    }

    // Assignation d'un niveau trophique par défaut pour les cycles
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (trophic_level[i] == 0) {
            trophic_level[i] = -1; // Indique un cycle ou une dépendance non résolue
        }
    }
    free(in_degree);
    liberer_queue(queue);
}

void afficher_niveaux_trophiques(Graphe *graphe, int *trophic_level) {

    printf("\n=== Niveaux Trophiques ===\n");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (trophic_level[i] == -1) {
            printf("Sommet %s : Niveau Trophique Indetermine (Cycle Present)\n", graphe->noms_des_sommets[i]);
        } else {
            printf("Sommet %s : Niveau %d\n", graphe->noms_des_sommets[i], trophic_level[i]);
        }
    }
}


void afficher_dependances(Graphe *graphe, int sommet, bool *visite, int niveau) {
    if (visite[sommet]) return; // Éviter de revisiter un sommet déjà exploré

    visite[sommet] = true;

    // Indentation pour visualiser la hiérarchie des dépendances
    for (int i = 0; i < niveau; i++) printf("  ");
    printf("- %s\n", graphe->noms_des_sommets[sommet]);

    // Trouver le premier prédécesseur direct
    int premier_predecesseur = -1;

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        Arrete *arrete = graphe->liste_d_adjacence[i];
        while (arrete != NULL) {
            if (arrete->indice_sommet_cible == sommet) {
                premier_predecesseur = i; // Sélectionner ce prédécesseur
                break; // Sortir dès qu'on a trouvé un prédécesseur
            }
            arrete = arrete->suivant_arrete;
        }
        if (premier_predecesseur != -1) break; // Sortir une fois un prédécesseur trouvé
    }

    // Appel récursif pour le premier prédécesseur trouvé
    if (premier_predecesseur != -1) {
        afficher_dependances(graphe, premier_predecesseur, visite, niveau + 1);
    }
}


void afficher_chaines_alimentaires(Graphe *graphe, int sommet_selectionne) {

    bool *visite = calloc(graphe->nombre_de_sommets, sizeof(bool));

    printf("\nChaines alimentaires dont depend '%s' :\n", graphe->noms_des_sommets[sommet_selectionne]);
    afficher_dependances(graphe, sommet_selectionne, visite, 0);

    free(visite);
}

void selectionner_et_afficher_chaines(Graphe *graphe_trophique) {

    printf("\n=== Liste des especes ===\n");
    for (int i = 0; i < graphe_trophique->nombre_de_sommets; i++) {
        printf("%d. %s\n", i + 1, graphe_trophique->noms_des_sommets[i]);
    }

    int choix;
    printf("\nEntrez le numero de l'espece pour afficher ses chaines alimentaires : ");

    if (scanf("%d", &choix) != 1 || choix < 1 || choix > graphe_trophique->nombre_de_sommets) {
        printf("Entree invalide ou numero hors limites.\n");
        return;
    }

    int sommet_selectionne = choix - 1; // Conversion en index

    // Appel pour afficher les chaînes alimentaires
    afficher_chaines_alimentaires(graphe_trophique, sommet_selectionne);
}



void niveau_trophique(Graphe *graphe_trophique) {

    int *trophic_level = malloc(graphe_trophique->nombre_de_sommets * sizeof(int));

    calculer_niveaux_trophiques(graphe_trophique, trophic_level);
    afficher_niveaux_trophiques(graphe_trophique, trophic_level);
    selectionner_et_afficher_chaines(graphe_trophique);
    free(trophic_level);
}


void bfs_composante_connexe(Graphe *graphe, int sommet_depart, bool *visite) {
    if (!graphe || !visite) return;

    Queue *queue = creer_queue(graphe->nombre_de_sommets);
    enfiler(queue, sommet_depart);
    visite[sommet_depart] = true;
    printf("%s ", graphe->noms_des_sommets[sommet_depart]);

    while (!est_vide(queue)) {
        int sommet_courant = defiler(queue);
        Arrete *arrete = graphe->liste_d_adjacence[sommet_courant];
        while (arrete != NULL) {
            int voisin = arrete->indice_sommet_cible;
            if (!visite[voisin]) {
                visite[voisin] = true;
                enfiler(queue, voisin);
                printf("-> %s ", graphe->noms_des_sommets[voisin]);
            }
            arrete = arrete->suivant_arrete;
        }
    }

    liberer_queue(queue);
}


void trouver_composantes_connexes(Graphe *graphe) {

    bool *visite = calloc(graphe->nombre_de_sommets, sizeof(bool));

    int composante_id = 1;
    printf("\n=== Composantes Connexes du Graphe ===\n");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (!visite[i]) {
            printf("Composante %d : ", composante_id++);
            bfs_composante_connexe(graphe, i, visite);
            printf("\n");
        }
    }
    free(visite);
}


void calculerPlusCourtsChemin(Graphe *graphe_ecosysteme, int id_espece_source) {

    // Allocation des tableaux pour les distances et les chemins
    float *distance = malloc(graphe_ecosysteme->nombre_de_sommets * sizeof(float));
    int *chemins_predecesseurs = malloc(graphe_ecosysteme->nombre_de_sommets * sizeof(int));
    int *sommets_traites = calloc(graphe_ecosysteme->nombre_de_sommets, sizeof(int));

    // Initialisation des distances et chemins
    for (int i = 0; i < graphe_ecosysteme->nombre_de_sommets; i++) {
        distance[i] = FLT_MAX;
        chemins_predecesseurs[i] = -1;      // Aucun prédécesseur au début
    }
    distance[id_espece_source] = 0.0f;

    // Algorithme Dijkstra
    for (int i = 0; i < graphe_ecosysteme->nombre_de_sommets - 1; i++) {
        // Trouver le sommet non traité avec la distance minimale
        float distance_minimale = FLT_MAX;
        int sommet_courant = -1;

        for (int j = 0; j < graphe_ecosysteme->nombre_de_sommets; j++) {
            if (!sommets_traites[j] && distance[j] <= distance_minimale) {
                distance_minimale = distance[j];
                sommet_courant = j;
            }
        }

        // Si tous les sommets restants sont inaccessibles, on arrête
        if (sommet_courant == -1) break;

        // Marquer le sommet comme traité
        sommets_traites[sommet_courant] = 1;

        // Mettre a jour les distances des sommets adjacents
        Arrete *arrete_adjacente = graphe_ecosysteme->liste_d_adjacence[sommet_courant];
        while (arrete_adjacente != NULL) {
            int sommet_voisin = arrete_adjacente->indice_sommet_cible;
            float poids_interaction = arrete_adjacente->poids_arrete;

            if (!sommets_traites[sommet_voisin] &&
                distance[sommet_courant] + poids_interaction < distance[sommet_voisin]) {
                distance[sommet_voisin] = distance[sommet_courant] + poids_interaction;
                chemins_predecesseurs[sommet_voisin] = sommet_courant;
            }
            arrete_adjacente = arrete_adjacente->suivant_arrete;
        }
    }

    // Affichage des résultats
    printf("\n=== Chemin et Distance minimal ===\n");
    for (int i = 0; i < graphe_ecosysteme->nombre_de_sommets; i++) {
        if (i == id_espece_source) continue;  // Ignorer l'espèce source

        if (distance[i] == FLT_MAX) {
            printf("\nEspece %s : Inaccessible\n", graphe_ecosysteme->noms_des_sommets[i]);
        } else {
            printf("\nEspece %s :\n", graphe_ecosysteme->noms_des_sommets[i]);
            printf("\tDistance minimal : %.2f\n", distance[i]);

            // Afficher le chemin entre l'espèce source et l'espèce actuelle
            afficher_chemin(graphe_ecosysteme, chemins_predecesseurs, i);
        }
    }
    free(distance);
    free(chemins_predecesseurs);
    free(sommets_traites);
}

void lancerCalculPlusCourtsChemin(Graphe *graphe) {

    int sommet_debut;
    printf("Entrez l'ID de l'espece pour le parcours DFS (1-%d) : ", graphe->nombre_de_sommets);

    // Lire l'entrée directement et vérifier sa validité
    if (scanf("%d", &sommet_debut) != 1 || sommet_debut < 1 || sommet_debut > graphe->nombre_de_sommets) {
        printf("Entree invalide ou ID de sommet hors limites.\n");
        return;
    }

    sommet_debut -= 1;
    system("cls");

    calculerPlusCourtsChemin(graphe, sommet_debut);
}

void afficher_chemin(Graphe *graphe, int *precedent, int sommet_final) {
    if (precedent[sommet_final] == -1) {
        printf("Aucun chemin vers %s\n", graphe->noms_des_sommets[sommet_final]);
        return;
    }

    // Utiliser une pile pour inverser le chemin
    int *chemin = malloc(graphe->nombre_de_sommets * sizeof(int));

    int taille = 0;
    int sommet = sommet_final;
    while (sommet != -1) {
        chemin[taille++] = sommet;
        sommet = precedent[sommet];
    }

    printf("\tChemin : ");
    for (int i = taille - 1; i >= 0; i--) {
        printf("%s ->  ", graphe->noms_des_sommets[chemin[i]]);
    }
    printf("\n");

    free(chemin);
}

void afficher_chaine_descendants_recursif(Graphe *graphe, int indice_espece, bool *visites, int niveau) {
    if (!graphe || visites[indice_espece]) {
        return; // Éviter de revisiter une espèce
    }

    visites[indice_espece] = true; // Marquer l'espèce comme visitée

    // Vérifier si l'espèce a des descendants
    Arrete *arrete = graphe->liste_d_adjacence[indice_espece];
    if (!arrete) {
        if (niveau == 0) {
            printf("Cette espece est le superpredateur.\n");
        }
        return;
    }

    // Parcourir les arêtes sortantes de l'espèce donnée
    while (arrete != NULL) {
        // Afficher la relation
        for (int i = 0; i < niveau; i++) printf("  ");
        printf("%s -> %s (Poids: %.2f)\n",
               graphe->noms_des_sommets[indice_espece],  // Source
               graphe->noms_des_sommets[arrete->indice_sommet_cible], // Cible
               arrete->poids_arrete);

        // Appel récursif pour explorer les descendants de la cible
        afficher_chaine_descendants_recursif(graphe, arrete->indice_sommet_cible, visites, niveau + 1);

        arrete = arrete->suivant_arrete;
    }
}


void afficher_degrees(Graphe *graphe) {
    int *in_degree = malloc(graphe->nombre_de_sommets * sizeof(int));
    int *out_degree = malloc(graphe->nombre_de_sommets * sizeof(int));

    // Calculer les degrés
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        in_degree[i] = 0;
        out_degree[i] = 0;
    }

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        Arrete *arrete = graphe->liste_d_adjacence[i];
        while (arrete != NULL) {
            out_degree[i]++;
            in_degree[arrete->indice_sommet_cible]++;
            arrete = arrete->suivant_arrete;
        }
    }

    // Afficher les degrés
    printf("\nDegres des sommets :\n\n");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        printf("Sommet %s :\n"
               "   -> Degree entrant = %d\n"
               "   -> Degree sortant = %d\n\n",
               graphe->noms_des_sommets[i],
               in_degree[i],
               out_degree[i]);
    }

    free(in_degree);
    free(out_degree);
}


void afficher_relations_recursif(Graphe *graphe, int indice_espece, bool *visites, int niveau, bool afficher_descendants) {
    if (!graphe || visites[indice_espece]) {
        return;
    }

    visites[indice_espece] = true; // Marquer l'espèce comme visitée

    if (afficher_descendants) {
        // Parcourir les descendants (arêtes sortantes)
        Arrete *arrete = graphe->liste_d_adjacence[indice_espece];
        while (arrete != NULL) {
            for (int i = 0; i < niveau; i++) printf("  ");
            printf("%s -> %s (Poids: %.2f)\n",
                   graphe->noms_des_sommets[indice_espece],  // Source
                   graphe->noms_des_sommets[arrete->indice_sommet_cible], // Cible
                   arrete->poids_arrete);

            // Appel récursif pour explorer les descendants
            afficher_relations_recursif(graphe, arrete->indice_sommet_cible, visites, niveau + 1, afficher_descendants);

            arrete = arrete->suivant_arrete;
        }
    } else {
        // Parcourir les ascendants (arêtes entrantes)
        for (int i = 0; i < graphe->nombre_de_sommets; i++) {
            Arrete *arrete = graphe->liste_d_adjacence[i];
            while (arrete != NULL) {
                if (arrete->indice_sommet_cible == indice_espece) {
                    for (int j = 0; j < niveau; j++) printf("  ");
                    printf("%s -> %s (Poids: %.2f)\n",
                           graphe->noms_des_sommets[i],  // Source
                           graphe->noms_des_sommets[indice_espece], // Cible
                           arrete->poids_arrete);

                    // Appel récursif pour explorer les ascendants
                    afficher_relations_recursif(graphe, i, visites, niveau + 1, afficher_descendants);
                }
                arrete = arrete->suivant_arrete;
            }
        }
    }
}

void afficher_relations_interactif(Graphe *graphe) {

    int choix_relation;
    int numero_espece;

    afficher_liste_sommets(graphe);

    printf("Entrez le numero de l'espece : ");
    if (scanf("%d", &numero_espece) != 1 || numero_espece < 1 || numero_espece > graphe->nombre_de_sommets) {
        printf("Numero invalide.\n");
        return;
    }

    // Convertir le numéro en index
    int indice_espece = numero_espece - 1;

    printf("Voulez-vous afficher les predateurs ou les proies directes ?\n");
    printf("1. Proies\n");
    printf("2. Predateurs\n");
    printf("Votre choix : ");
    if (scanf("%d", &choix_relation) != 1 || (choix_relation != 1 && choix_relation != 2)) {
        printf("Choix invalide.\n");
        return;
    }

    printf("\n=== Relations directes pour l'espece '%s' ===\n", graphe->noms_des_sommets[indice_espece]);

    if (choix_relation == 1) {
        printf("--- Proies directs ---\n");
        for (int i = 0; i < graphe->nombre_de_sommets; i++) {
            Arrete *arrete = graphe->liste_d_adjacence[i];
            while (arrete != NULL) {
                if (arrete->indice_sommet_cible == indice_espece) {
                    printf(" - %s (Poids : %.2f)\n", graphe->noms_des_sommets[i], arrete->poids_arrete);
                }
                arrete = arrete->suivant_arrete;
            }
        }
    } else if (choix_relation == 2) {
        printf("--- Predateurs directes ---\n");
        Arrete *arrete = graphe->liste_d_adjacence[indice_espece];
        while (arrete != NULL) {
            printf(" - %s (Poids : %.2f)\n", graphe->noms_des_sommets[arrete->indice_sommet_cible], arrete->poids_arrete);
            arrete = arrete->suivant_arrete;
        }
    }
}


void afficher_especes_sans_predateurs(Graphe *graphe) {
    system("cls");
    printf("\n=== Producteurs primaires ===\n");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        bool sans_predateurs = true;
        for (int j = 0; j < graphe->nombre_de_sommets; j++) {
            Arrete *arrete = graphe->liste_d_adjacence[j];
            while (arrete) {
                if (arrete->indice_sommet_cible == i) {
                    sans_predateurs = false;
                    break;
                }
                arrete = arrete->suivant_arrete;
            }
            if (!sans_predateurs) break;
        }
        if (sans_predateurs) {
            printf(" - %s\n", graphe->noms_des_sommets[i]);
        }
    }
    printf("\nAppuyez sur une touche pour revenir au menu...");
    _getch();
}

void afficher_especes_source_unique(Graphe *graphe) {
    system("cls"); // Nettoyer la console pour une meilleure lisibilité
    printf("\n=== Especes avec une seule source d'alimentation ===\n");

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        int compteur = 0;

        for (int j = 0; j < graphe->nombre_de_sommets; j++) {
            Arrete *arrete = graphe->liste_d_adjacence[j];
            while (arrete) {
                if (arrete->indice_sommet_cible == i) {
                    compteur++;
                }
                arrete = arrete->suivant_arrete;
            }
        }

        if (compteur == 1) {
            printf(" - %s\n", graphe->noms_des_sommets[i]);
        }
    }

    printf("\nAppuyez sur une touche pour revenir au menu...\n");
    _getch();
}


void afficher_superpredateurs(Graphe *graphe) {
    system("cls");
    printf("\n=== Superpredateurs ===\n");
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (!graphe->liste_d_adjacence[i]) {
            printf(" - %s\n", graphe->noms_des_sommets[i]);
        }
    }
    printf("\nAppuyez sur une touche pour revenir au menu...");
    _getch();
}

void afficher_especes_population_seuil(Graphe *graphe) {
    float seuil;
    int aucune_espece = 1; // Drapeau pour vérifier si une espèce est trouvée

    printf("Entrez le seuil de population : ");
    if (scanf("%f", &seuil) != 1 || seuil < 0) {
        printf("\nEntree invalide. Veuillez reessayer.\n");
        return;
    }

    printf("\n=== Especes avec une population superieure a %.0f ===\n", seuil);
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        // Vérifie si la population actuelle est supérieure au seuil
        if (graphe->populations_des_especes[i] > seuil) {
            printf(" - %s (Population : %.0f)\n",
                   graphe->noms_des_sommets[i],
                   graphe->populations_des_especes[i]);
            aucune_espece = 0;
        }
    }

    if (aucune_espece) {
        printf("Aucune espece n'a une population superieure a %.0f.\n", seuil);
    }

    printf("\nAppuyez sur une touche pour revenir au menu...");
    _getch();
}




void menu_recherche(Graphe *graphe_trophique) {
    const char* menuRecherche[] = {
            "Producteurs primaires",
            "Especes avec une seule source d'alimentation",
            "Superpredateurs",
            "Especes avec une population superieure a un seuil",
            "Retour au menu principal"
    };

    while (1) {
        int choixRecherche = deplacementMenu(menuRecherche, nombre_choix_menu_recherche, "===Recherche===");
        system("cls");

        if (choixRecherche == 5) break;

        switch (choixRecherche) {
            case 1:
                afficher_especes_sans_predateurs(graphe_trophique);
                break;

            case 2:
                afficher_especes_source_unique(graphe_trophique);
                break;

            case 3:
                afficher_superpredateurs(graphe_trophique);
                break;

            case 4: {
                afficher_especes_population_seuil(graphe_trophique);
                break;
            }

            default:
                printf("Choix invalide. Veuillez reessayer.\n");
                break;
        }
    }
}