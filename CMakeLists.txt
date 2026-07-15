cmake_minimum_required(VERSION 3.28)
project(projet_reseaux_trophiques_equipe_2e C)

set(CMAKE_C_STANDARD 11)

add_executable(projet_reseaux_trophiques_equipe_2e
        main.c
        algo.c
        algo.h
        graphe.c
        graphe.h
        simulation.h
        simulation.c
)
