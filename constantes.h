#ifndef CONSTANTES_H
#define CONSTANTES_H

#define PANNE_PROBABILITE 3 // 1 sur 10 chance de panne

#define SHM_FILES_TACHES "/shm_files_taches"
#define SHM_AFFECTATION "/shm_affectation"
#define SHM_TASKS_DONE "/shm_tasks_done"
#define SEM_MUTEX_TASKS_DONE "/sem_mutex_tasks_done"

#define QUEUE_CAPACITY 100  // Capacité maximale de la file de tâches

#define NB_ROBOTS_PAR_TYPE 10
#define NB_ROBOTS (NB_ROBOTS_PAR_TYPE * 3)
#define NB_FILES_TACHES 3 // Nombre de files de tâches (car 3 types de tâches)

#endif // CONSTANTES_H