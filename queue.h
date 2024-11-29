// queue.h

#ifndef QUEUE_H
#define QUEUE_H

#include "structures.h"

// Fonctions pour manipuler les files de tâches
void ajouter_tache(FileTaches *file, Tache tache);
int retirer_tache(FileTaches *file, Tache *tache);

#endif // QUEUE_H
