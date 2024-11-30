// queue.c

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include "queue.h"
#include "constantes.h"

/**
 * Ajoute une tâche à la file de tâches.
 * @param file File de tâches à modifier.
 * @param tache Tâche à ajouter.
 */
void ajouter_tache(FileTaches *file, Tache tache) {
    // Ouvrir les sémaphores nommés
    sem_t *mutex = open_semaphore(file->mutex_name);
    sem_t *items = open_semaphore(file->items_name);

    // Verrouiller la file
    sem_wait(mutex);
    int next_tail = (file->tail + 1) % QUEUE_CAPACITY;
    if (next_tail == file->head) {
        // Queue pleine
        printf("Erreur: File de tâches pleine, tâche %d non ajoutée.\n", tache.id);
        sem_post(mutex);
        sem_close(mutex);
        sem_close(items);
        return;
    }

    // Ajouter la tâche au tail
    file->tasks[file->tail] = tache;
    file->tail = next_tail;
    sem_post(mutex);

    // Indiquer qu'un nouvel élément est disponible
    sem_post(items);

    // Fermer les sémaphores
    sem_close(mutex);
    sem_close(items);
}

/**
 * Retire une tâche de la file de tâches.
 * @param file File de tâches à modifier.
 * @param tache Pointeur vers la tâche à remplir.
 * @return 1 si une tâche a été retirée, 0 sinon.
 */
int retirer_tache(FileTaches *file, Tache *tache) {
    // Ouvrir les sémaphores nommés
    sem_t *mutex = open_semaphore(file->mutex_name);
    sem_t *items = open_semaphore(file->items_name);

    // Attendre qu'un élément soit disponible, sinon bloquer
    sem_wait(items);

    // Verrouiller la file
    sem_wait(mutex);
    if (file->head == file->tail) {
        // File vide, ceci ne devrait pas arriver etant donné que items est bloqué
        sem_post(mutex);
        sem_close(mutex);
        sem_close(items);
        return 0;
    }

    // Retirer la tâche de la tête
    *tache = file->tasks[file->head];
    file->head = (file->head + 1) % QUEUE_CAPACITY;
    sem_post(mutex);

    // Fermer les sémaphores
    sem_close(mutex);
    sem_close(items);
    return 1;
}
