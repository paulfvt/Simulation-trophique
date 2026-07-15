#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphe.h"
#include "simulation.h"


///nbre sommmet
///nbre aretes
/// arete, poids ( coeff )
///id sommet, population ( N ), capacite de portage ( K ), nom, rythme de croissance ( r )


// Fonction pour trouver l'indice d'un sommet donné son nom
int trouver_indice_sommet(Graphe *graphe, const char *nom_sommet) {
    if (!graphe || !nom_sommet) {
        return -1; // Vérification des pointeurs valides
    }

    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (graphe->noms_des_sommets[i] && strcmp(graphe->noms_des_sommets[i], nom_sommet) == 0) {
            return i; // Sommet trouvé, retourner l'indice
        }
    }

    return -1; // Sommet non trouvé
}

void ajouter_arete(Graphe* graphe, int source, int destination, float poids_arrete) {
    if (!graphe || source < 0 || source >= graphe->nombre_de_sommets ||
        destination < 0 || destination >= graphe->nombre_de_sommets) {
        return;
    }
    Arrete *nouvelle_arrete = malloc(sizeof(Arrete));
    if (!nouvelle_arrete) {
        printf("Erreur d'allocation mémoire pour une arête.\n");
        return;
    }
    nouvelle_arrete->indice_sommet_cible = destination;
    nouvelle_arrete->poids_arrete = poids_arrete;
    nouvelle_arrete->suivant_arrete = graphe->liste_d_adjacence[source];
    graphe->liste_d_adjacence[source] = nouvelle_arrete;
}

void ecrireFichierDot(const Graphe *graphe, const char *nom_fichier_dot) {
    if (!graphe || !nom_fichier_dot) {
        printf("Graphe ou nom de fichier invalide.\n");
        return;
    }

    FILE *fichier_dot = fopen(nom_fichier_dot, "w");
    if (!fichier_dot) {
        printf("Erreur d'ouverture du fichier %s pour l'ecriture.\n", nom_fichier_dot);
        return;
    }

    fprintf(fichier_dot, "digraph G {\n");

    // Écrire les sommets avec la population, K et r dans le label
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        fprintf(fichier_dot, "    \"%s\" [label=\"%s\\nPopulation: %.0f\\nr: %.4f\"];\n",
                graphe->noms_des_sommets[i],
                graphe->noms_des_sommets[i],
                graphe->populations_des_especes[i],
                graphe->rythmes_croissance[i]);
    }

    // Écrire les arêtes avec le coefficient dans le label
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        Arrete *arrete = graphe->liste_d_adjacence[i];
        while (arrete != NULL) {
            fprintf(fichier_dot, "    \"%s\" -> \"%s\" [label=\"%.2f\"];\n",
                    graphe->noms_des_sommets[i],
                    graphe->noms_des_sommets[arrete->indice_sommet_cible],
                    arrete->poids_arrete);
            arrete = arrete->suivant_arrete;
        }
    }

    fprintf(fichier_dot, "}\n");

    fclose(fichier_dot);
    printf("Graphe sauvegardé dans %s avec succès.\n", nom_fichier_dot);
}


Graphe* lireFichierDot(const char *nom_fichier_dot) {
    FILE *fichier_dot = fopen(nom_fichier_dot, "r");
    if (!fichier_dot) {
        printf("Erreur d'ouverture du fichier %s pour la lecture.\n", nom_fichier_dot);
        return NULL;
    }

    Graphe *graphe = malloc(sizeof(Graphe));
    if (!graphe) {
        printf("Erreur d'allocation mémoire pour le graphe.\n");
        fclose(fichier_dot);
        return NULL;
    }

    // Initialiser les champs
    graphe->nombre_de_sommets = 0;
    graphe->noms_des_sommets = NULL;
    graphe->populations_des_especes = NULL;
    graphe->populations_initiales = NULL;
    graphe->capacites_portage = NULL;
    graphe->rythmes_croissance = NULL;
    graphe->liste_d_adjacence = NULL;

    // Temporary storage for edges
    typedef struct EdgeTemp {
        char source[TAILLE_MAX_NOM_SOMMET];
        char target[TAILLE_MAX_NOM_SOMMET];
        float weight;
    } EdgeTemp;

    EdgeTemp *edges_temp = NULL;
    int edges_capacity = 100;
    int edges_count = 0;
    edges_temp = malloc(edges_capacity * sizeof(EdgeTemp));
    if (!edges_temp) {
        printf("Erreur d'allocation mémoire pour les arêtes temporaires.\n");
        fclose(fichier_dot);
        free(graphe);
        return NULL;
    }

    char ligne[TAILLE_MAX_LIGNE];
    // Read file line by line
    while (fgets(ligne, sizeof(ligne), fichier_dot)) {
        // Remove newline character
        ligne[strcspn(ligne, "\n")] = 0;

        // Skip empty lines or lines with braces or digraph declaration
        if (strlen(ligne) == 0 ||
            strstr(ligne, "digraph") != NULL ||
            strchr(ligne, '{') != NULL ||
            strchr(ligne, '}') != NULL) {
            continue;
        }

        // Check if it's an edge
        if (strstr(ligne, "->") != NULL) {
            char source[TAILLE_MAX_NOM_SOMMET], target[TAILLE_MAX_NOM_SOMMET];
            float weight;
            // Example line: "Lapin" -> "Renard" [weight=0.6];
            int matched = sscanf(ligne, " \"%[^\"]\" -> \"%[^\"]\" [weight=%f];", source, target, &weight);
            if (matched == 3) {
                // Store the edge temporarily
                if (edges_count >= edges_capacity) {
                    edges_capacity *= 2;
                    edges_temp = realloc(edges_temp, edges_capacity * sizeof(EdgeTemp));
                    if (!edges_temp) {
                        printf("Erreur d'allocation mémoire lors de la reallocation des arêtes temporaires.\n");
                        fclose(fichier_dot);
                        liberer_graphe(graphe);
                        return NULL;
                    }
                }
                strcpy(edges_temp[edges_count].source, source);
                strcpy(edges_temp[edges_count].target, target);
                edges_temp[edges_count].weight = weight;
                edges_count++;
            }
            continue;
        }

        // It's a node with attributes
        // Example line: "Lapin" [population=100, capacity=200, growth_rate=0.05];
        char nom_sommet[TAILLE_MAX_NOM_SOMMET];
        float population, growth_rate;
        int capacity;
        int matched = sscanf(ligne, " \"%[^\"]\" [population=%f, capacity=%d, growth_rate=%f];", nom_sommet, &population, &capacity, &growth_rate);
        if (matched == 4) {
            // Add the node to the graphe
            graphe->nombre_de_sommets++;
            graphe->noms_des_sommets = realloc(graphe->noms_des_sommets, graphe->nombre_de_sommets * sizeof(char *));
            graphe->noms_des_sommets[graphe->nombre_de_sommets -1] = strdup(nom_sommet);
            graphe->populations_des_especes = realloc(graphe->populations_des_especes, graphe->nombre_de_sommets * sizeof(float));
            graphe->populations_des_especes[graphe->nombre_de_sommets -1] = population;
            graphe->populations_initiales = realloc(graphe->populations_initiales, graphe->nombre_de_sommets * sizeof(float));
            graphe->populations_initiales[graphe->nombre_de_sommets -1] = population;
            graphe->capacites_portage = realloc(graphe->capacites_portage, graphe->nombre_de_sommets * sizeof(int));
            graphe->capacites_portage[graphe->nombre_de_sommets -1] = capacity;
            graphe->rythmes_croissance = realloc(graphe->rythmes_croissance, graphe->nombre_de_sommets * sizeof(float));
            graphe->rythmes_croissance[graphe->nombre_de_sommets -1] = growth_rate;
            graphe->liste_d_adjacence = realloc(graphe->liste_d_adjacence, graphe->nombre_de_sommets * sizeof(Arrete *));
            graphe->liste_d_adjacence[graphe->nombre_de_sommets -1] = NULL;
        }
    }

    // Now, process all edges
    for (int i = 0; i < edges_count; i++) {
        // Find indices of source and target
        int source_idx = trouver_indice_sommet(graphe, edges_temp[i].source);
        int target_idx = trouver_indice_sommet(graphe, edges_temp[i].target);
        if (source_idx == -1 || target_idx == -1) {
            printf("Erreur: Sommet source ou cible introuvable pour l'arête: %s -> %s\n",
                   edges_temp[i].source, edges_temp[i].target);
            continue;
        }
        // Ajouter l'arête au graphe
        ajouter_arete(graphe, source_idx, target_idx, edges_temp[i].weight);
    }

    free(edges_temp);
    fclose(fichier_dot);
    printf("Graphe lu depuis %s avec succes.\n", nom_fichier_dot);
    return graphe;
}


Graphe *lireFichierTexte(const char *nom_fichier_txt) {
    FILE *fichier_txt = fopen(nom_fichier_txt, "r");

    Graphe *graphe_trophique = malloc(sizeof(Graphe));

    graphe_trophique->nombre_de_sommets = 0;
    graphe_trophique->noms_des_sommets = NULL;
    graphe_trophique->populations_des_especes = NULL;
    graphe_trophique->capacites_portage = NULL;
    graphe_trophique->rythmes_croissance = NULL;
    graphe_trophique->liste_d_adjacence = NULL;
    graphe_trophique->populations_initiales = NULL; // Pour sauvegarder les populations initiales

    int nombre_de_sommets, nombre_d_aretes;

    // Lire le nombre de sommets et d'arêtes
    if (fscanf(fichier_txt, "%d %d\n", &nombre_de_sommets, &nombre_d_aretes) != 2) {
        fclose(fichier_txt);
        free(graphe_trophique);
        return NULL;
    }

    // Initialiser le graphe avec le nombre de sommets
    graphe_trophique->nombre_de_sommets = nombre_de_sommets;

    // Allocation mémoire pour les différentes propriétés du graphe
    graphe_trophique->noms_des_sommets = calloc(nombre_de_sommets, sizeof(char *));
    graphe_trophique->populations_des_especes = malloc(nombre_de_sommets * sizeof(int));
    graphe_trophique->populations_initiales = malloc(nombre_de_sommets * sizeof(int)); // Allouer les populations initiales
    graphe_trophique->capacites_portage = malloc(nombre_de_sommets * sizeof(int));
    graphe_trophique->rythmes_croissance = malloc(nombre_de_sommets * sizeof(float));
    graphe_trophique->liste_d_adjacence = calloc(nombre_de_sommets, sizeof(Arrete *));

    // Lire les arêtes
    for (int i = 0; i < nombre_d_aretes; i++) {
        int id_source, id_cible;
        float poids;
        if (fscanf(fichier_txt, "%d, %d, %f\n", &id_source, &id_cible, &poids) != 3) {
            fclose(fichier_txt);
            liberer_graphe(graphe_trophique);
            return NULL;
        }
        id_source -= 1;
        id_cible -= 1;

        // Vérifier les limites des IDs
        if (id_source < 0 || id_source >= nombre_de_sommets || id_cible < 0 || id_cible >= nombre_de_sommets) {
            fclose(fichier_txt);
            liberer_graphe(graphe_trophique);
            return NULL;
        }

        // Ajouter l'arête
        Arrete *nouvelle_arrete = malloc(sizeof(Arrete));
        if (!nouvelle_arrete) {
            fclose(fichier_txt);
            liberer_graphe(graphe_trophique);
            return NULL;
        }
        nouvelle_arrete->indice_sommet_cible = id_cible;
        nouvelle_arrete->poids_arrete = poids;
        nouvelle_arrete->suivant_arrete = graphe_trophique->liste_d_adjacence[id_source];
        graphe_trophique->liste_d_adjacence[id_source] = nouvelle_arrete;
    }

    // Lire les sommets avec leurs informations
    for (int i = 0; i < nombre_de_sommets; i++) {
        int id_sommet, population, capacite;
        char nom_sommet[TAILLE_MAX_NOM_SOMMET];
        float rythme_croissance;

        if (fscanf(fichier_txt, "%d, %d, %d, %[^,], %f\n", &id_sommet, &population, &capacite, nom_sommet, &rythme_croissance) != 5) {
            fclose(fichier_txt);
            liberer_graphe(graphe_trophique);
            return NULL;
        }

        id_sommet -= 1;
        graphe_trophique->noms_des_sommets[id_sommet] = strdup(nom_sommet);
        graphe_trophique->populations_des_especes[id_sommet] = population;
        graphe_trophique->populations_initiales[id_sommet] = population; // Sauvegarde initiale
        graphe_trophique->capacites_portage[id_sommet] = capacite;
        graphe_trophique->rythmes_croissance[id_sommet] = rythme_croissance;
    }

    fclose(fichier_txt);
    return graphe_trophique;
}

// Fonction pour afficher les informations du graphe
void afficherInfoGraphe(Graphe *graphe_trophique) {
    if (!graphe_trophique) {
        printf("Le graphe est vide.\n");
        return;
    }

    // Afficher les informations générales du graphe
    printf("\n=== Informations du Graphe ===\n\n");
    printf("Nombre de sommets : %d\n\n", graphe_trophique->nombre_de_sommets);

    // Parcourir chaque sommet pour afficher ses informations
    for (int indice_sommet = 0; indice_sommet < graphe_trophique->nombre_de_sommets; indice_sommet++) {
        printf("\n\nSommet %d: %s\n", indice_sommet + 1, graphe_trophique->noms_des_sommets[indice_sommet]);
        printf("  Population: %.0f\n", graphe_trophique->populations_des_especes[indice_sommet]);
        printf("  Capacite de Portage: %d\n", graphe_trophique->capacites_portage[indice_sommet]);
        printf("  Rythme de Croissance: %.4f\n", graphe_trophique->rythmes_croissance[indice_sommet]);

        // Afficher les arêtes entrantes (sommets précédents)
        printf("  Il mange :\n");
        bool trouve_precedent = false;
        for (int i = 0; i < graphe_trophique->nombre_de_sommets; i++) {
            Arrete *arrete_precedente = graphe_trophique->liste_d_adjacence[i];
            while (arrete_precedente != NULL) {
                if (arrete_precedente->indice_sommet_cible == indice_sommet) {
                    printf("     -> %s (Poids: %.2f)\n",
                           graphe_trophique->noms_des_sommets[i],
                           arrete_precedente->poids_arrete);
                    trouve_precedent = true;
                }
                arrete_precedente = arrete_precedente->suivant_arrete;
            }
        }
        if (!trouve_precedent) {
            printf("    Aucun sommet precedent.\n");
        }

        // Afficher les arêtes sortantes (sommets prédateurs)
        Arrete *arrete = graphe_trophique->liste_d_adjacence[indice_sommet];
        if (arrete) {
            printf("  Il se fait manger par :\n");
            while (arrete != NULL) {
                printf("     -> %s (Poids: %.2f)\n",
                       graphe_trophique->noms_des_sommets[arrete->indice_sommet_cible],
                       arrete->poids_arrete);
                arrete = arrete->suivant_arrete;
            }
        } else {
            printf("  C'est le SuperPredateur.\n");
        }
    }
}
// Fonction utilitaire pour libérer la mémoire allouée au graphe
void liberer_graphe(Graphe *graphe_trophique) {
    if (!graphe_trophique) return;
    for (int indice_sommet = 0; indice_sommet < graphe_trophique->nombre_de_sommets; indice_sommet++) {
        free(graphe_trophique->noms_des_sommets[indice_sommet]);
        Arrete *arrete_courante = graphe_trophique->liste_d_adjacence[indice_sommet];
        while (arrete_courante != NULL) {
            Arrete *temp = arrete_courante;
            arrete_courante = arrete_courante->suivant_arrete;
            free(temp);
        }
    }
    free(graphe_trophique->noms_des_sommets);
    free(graphe_trophique->populations_des_especes);
    free(graphe_trophique->capacites_portage);
    free(graphe_trophique->rythmes_croissance);
    free(graphe_trophique->liste_d_adjacence);
    free(graphe_trophique->populations_initiales);
    free(graphe_trophique);
}

// Fonction pour vérifier si le graphe est connecté, en excluant certains sommets
void dfs(int sommet, Graphe *graphe, bool *exclus, bool *visite) {
    visite[sommet] = true;

    // Parcourir les arêtes sortantes
    Arrete *arrete = graphe->liste_d_adjacence[sommet];
    while (arrete != NULL) {
        int cible = arrete->indice_sommet_cible;
        if (!exclus[cible] && !visite[cible]) {
            dfs(cible, graphe, exclus, visite);
        }
        arrete = arrete->suivant_arrete;
    }

    // Parcourir les arêtes entrantes pour traiter le graphe comme non orienté
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (i != sommet && !exclus[i]) {
            Arrete *a = graphe->liste_d_adjacence[i];
            while (a != NULL) {
                if (a->indice_sommet_cible == sommet && !visite[i]) {
                    dfs(i, graphe, exclus, visite);
                }
                a = a->suivant_arrete;
            }
        }
    }
}

// Fonction pour vérifier si le graphe est connecté, en excluant certains sommets
bool est_connexe(Graphe *graphe, bool *exclus) {
    // Trouver le premier sommet non exclu pour démarrer la DFS
    int debut = -1;
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (!exclus[i]) {
            debut = i;
            break;
        }
    }

    if (debut == -1) {
        // Tous les sommets sont exclus
        return false;
    }

    // Initialiser un tableau de visite
    bool *visite = calloc(graphe->nombre_de_sommets, sizeof(bool));

    // Démarrer la DFS
    dfs(debut, graphe, exclus, visite);

    // Vérifier si tous les sommets non exclus sont visités
    bool connexe = true;
    for (int i = 0; i < graphe->nombre_de_sommets; i++) {
        if (!exclus[i] && !visite[i]) {
            connexe = false;
            break;
        }
    }

    free(visite);
    return connexe;
}

// Fonction pour générer toutes les combinaisons de k sommets à exclure
void generer_combinaisons(int n, int k, int start, int depth, int *combinaison, int ***resultats, int *nombre_resultats, int *capacite_resultats) {
    if (depth == k) {
        // Vérifier et augmenter la capacité si nécessaire
        if (*nombre_resultats >= *capacite_resultats) {
            *capacite_resultats *= 2;
            *resultats = realloc(*resultats, (*capacite_resultats) * sizeof(int*));
        }
        // Allouer et copier la combinaison actuelle
        (*resultats)[*nombre_resultats] = malloc(k * sizeof(int));
        memcpy((*resultats)[*nombre_resultats], combinaison, k * sizeof(int));
        (*nombre_resultats)++;
        return;
    }

    for (int i = start; i <= n - (k - depth); i++) {
        combinaison[depth] = i;
        generer_combinaisons(n, k, i + 1, depth + 1, combinaison, resultats, nombre_resultats, capacite_resultats);
    }
}

// Fonction pour calculer la k-connexité du graphe
int calculer_k_connexite(Graphe *graphe) {
    int k_connexite = 0;
    int nombre_sommets = graphe->nombre_de_sommets;

    for (int k = 1; k < nombre_sommets; k++) {
        // Calcul du nombre de combinaisons (C(n, k))
        int C = 1;
        for (int i = 1; i <= k; i++) {
            C *= (nombre_sommets - i + 1);
            C /= i;
        }

        // Allouer de l'espace pour les combinaisons
        int capacite_resultats = (C > 1000) ? C : 1000;
        int **resultats = malloc(capacite_resultats * sizeof(int*));
        int nombre_resultats = 0;

        // Allouer un tableau pour la combinaison courante
        int *combinaison = malloc(k * sizeof(int));

        // Générer toutes les combinaisons de k sommets
        generer_combinaisons(nombre_sommets, k, 0, 0, combinaison, &resultats, &nombre_resultats, &capacite_resultats);
        free(combinaison);

        bool deconnecte = false;

        // Vérifier chaque combinaison
        for (int i = 0; i < nombre_resultats; i++) {
            bool *exclus = calloc(nombre_sommets, sizeof(bool));

            for (int j = 0; j < k; j++) {
                exclus[resultats[i][j]] = true;
            }

            if (!est_connexe(graphe, exclus)) {
                deconnecte = true;
                free(exclus);
                break;
            }

            free(exclus);
        }

        // Libérer les combinaisons générées
        for (int i = 0; i < nombre_resultats; i++) {
            free(resultats[i]);
        }
        free(resultats);

        if (deconnecte) {
            k_connexite = k - 1;
            break;
        } else {
            k_connexite = k;
        }
    }

    return k_connexite;
}


void afficher_k_connexite(Graphe* graphe) {
    int k_connexite = calculer_k_connexite(graphe);
    if (k_connexite == -1) {
        printf("Erreur lors du calcul de la k-connexite.\n");
        return;
    }
    printf("K-connexite du graphe : %d\n", k_connexite);
    if (k_connexite > 0) {
        printf("Le graphe est %d-connexe, signifiant qu'il reste connexe après la suppression de %d sommets.\n",
               k_connexite, k_connexite);
    }
    else {
        printf("Le graphe n'est pas k-connexe.\n");
    }
}

void afficherGraphiqueEvolution(const char *fichierImage) {
    printf("Choisissez le graphe a afficher:\n\n");
    printf("1. Graphe de base (%s)\n", fichierImage);
    printf("2. Dernier graphe de simulation \n\n");
    printf("Entrez votre choix (1 ou 2): ");

    int choix_affichage;
// Lire le choix de l'utilisateur
    if (scanf("%d", &choix_affichage) != 1) {
        printf("Entrée invalide. Retour au menu principal.\n");
        // Nettoyer l'entrée en cas d'erreur
        while (getchar() != '\n');
    }

// Nettoyer le tampon d'entrée
    while (getchar() != '\n');

    if (choix_affichage == 1) {
        // Afficher le graphe de base
        if (fichierImage) {
            printf("Affichage du graphe de base : %s\n", fichierImage);
            // Préparer la commande pour ouvrir l'image du graphe avec un titre vide
            char commande[512];
            snprintf(commande, sizeof(commande), "start \"\" \"%s\"", fichierImage);
            // Exécuter la commande système pour afficher le fichier
            int status = system(commande);
            // Vérification du statut de la commande
            if (status == -1) {
                fprintf(stderr, "Erreur : impossible d'exécuter la commande pour afficher le graphe.\n");
            }
        }
        else {
            fprintf(stderr, "Erreur : aucun fichier image spécifié pour l'affichage du graphe de base.\n");
        }
    } else if (choix_affichage == 2) {
        // Générer et afficher le dernier graphe de simulation
        printf("Generation du dernier graphe de simulation...\n");

        const char *fichier_dot = "../graphe.dot";
        const char *fichier_png = "../graphe.png";

        FILE *fichier = fopen(fichier_dot, "r");
        if (!fichier) {
            printf("Erreur : le fichier %s n'existe pas. Veuillez effectuer une simulation d'abord.\n", fichier_dot);
            return;
        }
        fclose(fichier);

        char commande_dot_to_png[512];
        snprintf(commande_dot_to_png, sizeof(commande_dot_to_png), "dot -Tpng \"%s\" -o \"%s\"", fichier_dot, fichier_png);

        if (system(commande_dot_to_png) != 0) {
            printf("Erreur lors de la generation de %s a partir de %s.\n", fichier_png, fichier_dot);
        } else {
            printf("Affichage du dernier graphe de simulation : %s\n", fichier_png);
            char commande_afficher[512];
            snprintf(commande_afficher, sizeof(commande_afficher), "start \"\" \"%s\"", fichier_png);
            if (system(commande_afficher) == -1) {
                fprintf(stderr, "Erreur : impossible d'executer la commande pour afficher le graphe de simulation.\n");
            }
        }
    } else {
        printf("Choix invalide. Retour au menu principal.\n");
    }
}


void AffichageDesGraphes(Graphe *graphe_trophique, const char *fichierImage){
    {
        printf("Choisissez le graphe a afficher:\n\n");
        printf("1. Graphe de base (%s)\n", fichierImage);
        printf("2. Dernier graphe de simulation \n");
        printf("3. Graphique d'evolution des populations\n\n");
        printf("Entrez votre choix (1, 2 ou 3): ");

        int choix_affichage;
        if (scanf("%d", &choix_affichage) != 1) {
            printf("Entrée invalide. Retour au menu principal.\n");
            // Nettoyer l'entrée en cas d'erreur
            while (getchar() != '\n');
        }

        while (getchar() != '\n');

        if (choix_affichage == 1) {
            if (fichierImage) {
                printf("Affichage du graphe de base : %s\n", fichierImage);
                char commande[512];
                snprintf(commande, sizeof(commande), "start \"\" \"%s\"", fichierImage);
                int status = system(commande);
                if (status == -1) {
                    fprintf(stderr, "Erreur : impossible d'exécuter la commande pour afficher le graphe.\n");
                }
            }
        }
        else if (choix_affichage == 2) {
            printf("Generation du dernier graphe de simulation...\n");

            const char *fichier_dot = "../graphe.dot";
            const char *fichier_png = "../graphe.png";

            FILE *fichier = fopen(fichier_dot, "r");
            if (!fichier) {
                printf("Erreur : le fichier %s n'existe pas. Veuillez effectuer une simulation d'abord.\n", fichier_dot);
            }
            fclose(fichier);

            char commande_dot_to_png[512];
            snprintf(commande_dot_to_png, sizeof(commande_dot_to_png), "dot -Tpng \"%s\" -o \"%s\"", fichier_dot, fichier_png);

            int result = system(commande_dot_to_png);
            if (result != 0) {
                printf("Erreur lors de la génération de %s a partir de %s.\n", fichier_png, fichier_dot);
            }
            else {
                printf("Affichage du dernier graphe de simulation : %s\n", fichier_png);
                char commande_afficher[512];
                snprintf(commande_afficher, sizeof(commande_afficher), "start \"\" \"%s\"", fichier_png);
                int status = system(commande_afficher);
                if (status == -1) {
                    fprintf(stderr, "Erreur : impossible d'exécuter la commande pour afficher le graphe de simulation.\n");
                }
            }
        }
        else if (choix_affichage == 3) {
            printf("Affichage du graphique d'evolution des populations...\n");
            generer_et_afficher_graphique_evolution(graphe_trophique);
        }
        else {
            printf("Choix invalide. Retour au menu principal.\n");
        }
    }
}