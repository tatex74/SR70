#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <semaphore.h>


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

// Structure pour un nœud de la file de tâches
typedef struct Node {
    Tache tache;            // Tâche contenue dans le nœud
    struct Node *suivant;   // Pointeur vers le nœud suivant
} Node;

// Structure pour la file de tâches
typedef struct {
    Node *tete;             // Tête de la file
    Node *queue;            // Queue de la file
    sem_t sem_mutex;        // Sémaphore pour protéger l'accès à la file
    sem_t sem_taches;       // Sémaphore pour indiquer qu'il y a des tâches dans la file
} FileTaches;

// Structure pour les robots
typedef struct {
    int id;                 // Identifiant unique du robot
    int is_alive;           // Indicateur pour savoir si le robot est en vie (1) ou mort (0)
    TypeTache type_robot;   // Type du robot (assemblage, peinture, vérification)
    pid_t pid;              // PID du processus du robot
} Robot;

#endif // STRUCTURES_H