#ifndef SUPERVISEUR_H
#define SUPERVISEUR_H

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>

void superviseur_init(int argc, char *argv[]);
void init_shared_memory();
void init_files_taches();
void cleanup_resources();
void init_signals();
void init_taches(int nombre_de_taches);
void superviseur_loop();
void create_all_robots();
void create_robot(int robot_id, int type);
void sigchld_handler();
void init_semaphores();

#endif // SUPERVISEUR_H
