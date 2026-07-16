# 🌍 Trophic Networks — Modélisation et Simulation de Réseaux Écologiques

> **Un projet d'ingénierie système et d'algorithmique de niveau ING2, axé sur la Théorie des Graphes et la modélisation dynamique de populations, développé en Langage C.**

Ce projet, réalisé dans le cadre du cursus d'ingénieur à l'ECE Paris, propose un outil complet d'analyse de structures écologiques (chaînes alimentaires, interactions prédateurs-proies) et de simulation de l'évolution dynamique de populations de réseaux trophiques (Savane, Arctique, Forêt).

---

## 🚀 Fonctionnalités Clés du Programme

Le programme s'exécute en mode console sous forme de menu interactif et propose les modules d'analyses algorithmiques et physiques suivants :

- **📊 Analyse Structurelle du Graphe** :
  - **Niveaux Trophiques** : Classification hiérarchique des espèces (Producteurs primaires, Herbivores, Prédateurs intermédiaires, Superprédateurs).
  - **Robustesse & Cohésion** : Recherche des composantes connexes et calcul de la $k$-sommet-connexité pour analyser la résistance du réseau face à l'extinction d'espèces.
  - **Détection de Cycles** : Analyse de la cohérence biologique du réseau (absence de boucles de rétroaction infinies directes).
  - **Parcours de Graphes (DFS)** : Exploration en profondeur de la structure des flux énergétiques.
  - **Calcul des Plus Courts Chemins** : Identification de la distance minimale de transfert d'énergie entre deux sommets.
- **📈 Simulation Dynamique de Populations** :
  - Intégration d'un modèle d'équations différentielles de type **Verhulst / Lotka-Volterra** pour simuler les variations démographiques au fil des générations (prise en compte du taux de croissance $r$, de la capacité de portage $K$ et des coefficients de prédation).
  - Outil de modification interactive des paramètres biologiques (ajustement de $N$, $K$, $r$ d'une espèce) hors simulation.
- **🖼️ Visualisation Graphique Avancée** :
  - Génération automatique de diagrammes de graphes orientés via **Graphviz** (fichiers `.dot` compilés en `.png`).
  - Tracé de courbes d'évolution des populations générées en temps réel via **Gnuplot** (exports `.csv` et rendus `.png`).

---

## 🛠️ Architecture Algorithmie & Choix Techniques

### 1. Structure de Données du Graphe
Le graphe trophique est modélisé de manière optimisée pour allier flexibilité de modification et rapidité de parcours :
- **Représentation par Listes d'Adjacence** : Chaque sommet (espèce) pointe vers une liste chaînée d'arêtes sortantes (relations de prédation), assurant une complexité en espace de $O(\vert{}S\vert{} + \vert{}A\vert{})$.
- **Modélisation d'une Arête** :
```c
typedef struct Arrete {
    int destination;           // Indice du sommet cible
    float poids;               // Coefficient d'interaction (force de la prédation)
    struct Arrete *suivant;    // Pointeur vers l'arête suivante
} Arrete;
