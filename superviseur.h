#ifndef SUPERVISEUR_H
#define SUPERVISEUR_H

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>


// Prototypes des fonctions
void create_robot(int robot_id, int type);
void create_all_robots();
void superviseur_init();
void init_signals();
void check_robots_alive();

#endif // SUPERVISEUR_H
