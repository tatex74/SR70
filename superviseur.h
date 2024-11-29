#ifndef SUPERVISEUR_H
#define SUPERVISEUR_H

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>


// Prototypes des fonctions
void superviseur_init(int argc, char *argv[]);
void create_all_robots();
void create_robot(int robot_id, int type);
void sigchld_handler(int signo);
void init_signals();
void init_taches(int nombre_de_taches);
void superviseur_loop();


#endif // SUPERVISEUR_H
