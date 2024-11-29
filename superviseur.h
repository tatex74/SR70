#ifndef SUPERVISEUR_H
#define SUPERVISEUR_H

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structures.h" // Inclut la définition de struct Robot

// Prototypes des fonctions
void create_robot(int i);
void superviseur_init();
void is_alive_received(int signo, siginfo_t *info, void *context);
void send_signal_to_check_alive();
void init_signals();

#endif // SUPERVISEUR_H
