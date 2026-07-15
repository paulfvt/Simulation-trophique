#include <stdio.h>
#include <stdlib.h>
#include "graphe.h"
#include "algo.h"
#include "simulation.h"
#include <conio.h>

#define nombre_choix_menu_choix 13
#define nombre_choix_menu_reseau 3

int main() {
    // Variables pour gérer le nom du fichier
    Graphe *graphe_trophique = NULL;
    const char* fichierImage = NULL;

    // Définition des réseaux
    const char* menuReseau[] = {
            "Reseau Arctique \n     -> Ecosysteme polaire, climat froid extreme, autour de l'ocean Arctique",
            "Reseau Foret    \n     -> Ecosysteme boise, climat tempere, regions diversifiees",
            "Reseau Savane   \n     -> Ecosysteme herbace, climat chaud saisonnier, Afrique subsaharienne"
    };

    // Affichge Menu principal
    const char* menuChoix[] = {
            "Afficher info Graphe",
            "Afficher degrees",
            "Afficher relations",
            "Afficher niveaux trophiques",
            "Trouver composantes connexes",
            "Parcours DFS",
            "Detecter cycles",
            "Distance minimal",
            "Lancer la simulation dynamique",
            "Robustesse du reseau (k-sommet-connexite)",
            "Lancer le menu de recherche",
            "Afficher graphe",
            "Quitter"
    };

    int choixgraphe = deplacementMenu(menuReseau, nombre_choix_menu_reseau, "    ____   __                            \n"
                                                                            "   / __ \\_/_/ ________  ____ ___  ___  __\n"
                                                                            "  / /_/ / _ \\/ ___/ _ \\/ __ `/ / / / |/_/\n"
                                                                            " / _, _/  __(__  )  __/ /_/ / /_/ />  <  \n"
                                                                            "/_/ |_|\\___/____/\\___/\\____/\\____/_/|_|  \n"
                                                                            "                                         ");

    // Charger le graphe en fonction du réseau choisi
    switch (choixgraphe) {
        case 1:
            graphe_trophique = lireFichierTexte("..//arctique.txt");
            fichierImage = "..//arctique.png";
            break;
        case 2:
            graphe_trophique = lireFichierTexte("..//foret.txt");
            fichierImage = "..//foret.png";
            break;
        case 3:
            graphe_trophique = lireFichierTexte("..//savane.txt");
            fichierImage = "..//savane.png";
            break;
        default:
            return EXIT_FAILURE;
    }

    // Boucle du Menu principal
    while (1) {
        int choix = deplacementMenu(menuChoix, nombre_choix_menu_choix, "    __  ___                \n"
                                                                        "   /  |/  /__  ____  __  __\n"
                                                                        "  / /|_/ / _ \\/ __ \\/ / / /\n"
                                                                        " / /  / /  __/ / / / /_/ / \n"
                                                                        "/_/  /_/\\___/_/ /_/\\____/  \n"
                                                                        "                           ");

        system("cls");

        switch (choix) {
            case 1:
                afficherInfoGraphe(graphe_trophique);
                break;

            case 2:
                afficher_degrees(graphe_trophique);
                break;

            case 3: {
                afficher_relations_interactif(graphe_trophique);
                break;
            }

            case 4:
                niveau_trophique(graphe_trophique);
                break;

            case 5:
                trouver_composantes_connexes(graphe_trophique);
                break;

            case 6:
                afficher_liste_sommets(graphe_trophique);
                DFS(graphe_trophique);
                break;

            case 7:
                detectCycle(graphe_trophique);
                break;

            case 8:
                afficher_liste_sommets(graphe_trophique);
                lancerCalculPlusCourtsChemin(graphe_trophique);
                break;

            case 9:
                sousMenuSimulation(graphe_trophique);
                break;

            case 10:
                afficher_k_connexite(graphe_trophique);
                break;

            case 11:
                menu_recherche(graphe_trophique);
                break;

            case 12:
                AffichageDesGraphes(graphe_trophique,fichierImage);
                break;

            case 13:
                // Quitter le programme
                printf("Quitter le programme.\n");
                liberer_graphe(graphe_trophique);
                return EXIT_SUCCESS;

            default:
                printf("Choix invalide.\n");
                break;
        }
        printf("\nAppuyez sur une touche pour continuer...");
        _getch();
    }
}
