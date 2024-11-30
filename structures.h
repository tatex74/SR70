// structures.h

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <sys/types.h>
#include "constantes.h"

// Définition des types de tâches
typedef enum {
    ASSEMBLAGE,
    PEINTURE,
    VERIFICATION
} TypeTache;

// Structure pour les tâches
typedef struct {
    int id;                 // Identifiant unique de la tâche
    TypeTache type_actuel;  // Type actuel de la tâche (étape en cours)
} Tache;

// Structure pour la file de tâches, partagée entre les robots
typedef struct {
    Tache tasks[QUEUE_CAPACITY]; // Tableau fixe pour stocker les tâches
    int head;                     // Indice de la tête de la file
    int tail;                     // Indice de la queue de la file
    char mutex_name[64];          // Nom du sémaphore mutex, pour protéger l'accès à la file
    char items_name[64];          // Nom du sémaphore items, pour indiquer qu'un élément est disponible
} FileTaches;

// Structure pour les robots
typedef struct {
    int id;                          // Identifiant unique du robot
    TypeTache type_robot;            // Type du robot (type de tâche qu'il peut traiter)
    pid_t pid;                       // PID du processus du robot
    int exit_status; // Statut de sortie du robot
} Robot;

#endif // STRUCTURES_H
