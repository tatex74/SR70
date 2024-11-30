// queue.h

#ifndef QUEUE_H
#define QUEUE_H

#include "structures.h"

int ajouter_tache(FileTaches *file, Tache tache);
int retirer_tache(FileTaches *file, Tache *tache);

#endif // QUEUE_H
