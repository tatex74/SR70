#ifndef ROBOT_H
#define ROBOT_H

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "structures.h"

void robot_init();
void is_alive(int signo, siginfo_t *info, void *context);
char *type_robot_to_string(TypeTache type);
#endif // ROBOT_H
