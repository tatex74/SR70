#ifndef ROBOT_H
#define ROBOT_H

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "structures.h"

char *type_robot_to_string(TypeTache type);
void handle_sigterm(int signo);
void traiter_tache(Tache *tache, int *tasks_done);

#endif // ROBOT_H
