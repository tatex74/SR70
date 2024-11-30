#ifndef ROBOT_H
#define ROBOT_H

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "structures.h"

char *type_robot_to_string(TypeTache type);
void handle_sigterm(int signo);
char *type_robot_to_string(TypeTache type);

#endif // ROBOT_H
