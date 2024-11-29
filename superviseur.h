#ifndef SUPERVISEUR_H
#define SUPERVISEUR_H

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structures.h" // Inclut la définition de struct Robot

// Prototypes des fonctions
void create_robot(int robot_id, int type);
void create_all_robots();
void superviseur_init();
void init_signals();
void check_robots_alive();

#endif // SUPERVISEUR_H
