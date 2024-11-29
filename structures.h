// structures.h

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <sys/types.h>

#define QUEUE_CAPACITY 100  // Capacité maximale de la file de tâches

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

// Structure pour la file de tâches (buffer circulaire)
typedef struct {
    Tache tasks[QUEUE_CAPACITY]; // Tableau fixe pour stocker les tâches
    int head;                     // Indice de la tête de la file
    int tail;                     // Indice de la queue de la file
    char mutex_name[64];          // Nom du sémaphore mutex
    char items_name[64];          // Nom du sémaphore items
} FileTaches;

// Structure pour les robots
typedef struct {
    int id;                          // Identifiant unique du robot
    TypeTache type_robot;            // Type du robot
    pid_t pid;                       // PID du processus du robot
    volatile sig_atomic_t is_alive;  // Indique si le robot est vivant
    volatile sig_atomic_t exit_status; // Statut de sortie du robot
} Robot;

#endif // STRUCTURES_H
