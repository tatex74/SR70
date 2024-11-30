#ifndef CONSTANTES_H
#define CONSTANTES_H

#define PANNE_PROBABILITE 10 // 1 sur 10 chance de panne

#define SHM_FILES_TACHES 25000 // Clé pour la mémoire partagée des files de tâches
#define SHM_AFFECTATION 25001 // Clé pour la mémoire partagée de l'affectation des robots
#define SHM_TASKS_DONE 25002 // Clé pour la mémoire partagée du compteur de tâches terminées
#define SEM_MUTEX_TASKS_DONE "/sem_mutex_tasks_done" // Nom du sémaphore pour protéger le compteur de tâches terminées

#define QUEUE_CAPACITY 10000  // Capacité maximale de la file de tâches

#define NB_ROBOTS_PAR_TYPE 30 // Le nombre de robot par type (10 d'assemblage, 10 de peinture, 10 de vérification)
#define NB_FILES_TACHES 3 // Nombre de files de tâches (car 3 types de tâches)
#define NB_ROBOTS (NB_ROBOTS_PAR_TYPE * NB_FILES_TACHES) // Le nombre total de robot

#endif // CONSTANTES_H
