// queue.h

#ifndef QUEUE_H
#define QUEUE_H
#include "structures.h"

// Déclaration des fonctions pour manipuler les files de tâches
void init_files_taches(FileTaches *files_taches);
void ajouter_tache(FileTaches *file, Tache tache);
int retirer_tache(FileTaches *file, Tache *tache);

#endif // QUEUE_H
