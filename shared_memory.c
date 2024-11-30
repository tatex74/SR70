#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include "shared_memory.h"
#include "constantes.h"

/**
 * Ouvre une mémoire partagée existante.
 * @param key Clé de la mémoire partagée.
 * @param size Taille de la mémoire partagée.
 * @return Pointeur vers la mémoire partagée.
 */
void *open_shared_memory(key_t key, size_t size)
{
    // Ajouter le PID du processus parent pour éviter les collisions de clés
    // On met getppid() car cette fonction est appelée par les enfants
    key = key + getppid();

    int shm_id = shmget(key, size, 0666);
    if (shm_id == -1)
    {
        perror("shmget failed in open_shared_memory");
        exit(EXIT_FAILURE);
    }

    void *addr = shmat(shm_id, NULL, 0);
    if (addr == (void *) -1)
    {
        perror("shmat failed in open_shared_memory");
        exit(EXIT_FAILURE);
    }

    return addr;
}

/**
 * Crée une mémoire partagée.
 * @param key Clé de la mémoire partagée.
 * @param size Taille de la mémoire partagée.
 * @return Pointeur vers la mémoire partagée.
 */
void* shared_memory_create(key_t key, size_t size)
{
    // Ajouter le PID du processus pour éviter les collisions de clés
    // On met getpid() car cette fonction est appelée par le parent
    key = key + getpid();
    if (key == -1)
    {
        perror("ftok failed in shared_memory_create");
        exit(EXIT_FAILURE);
    }

    int shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1)
    {
        if (errno == EEXIST)
        {
            // La mémoire partagée existe déjà, essayer de l'ouvrir
            shm_id = shmget(key, size, 0666);
            if (shm_id == -1)
            {
                perror("shmget failed to open existing shared memory in shared_memory_create");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            perror("shmget failed in shared_memory_create");
            exit(EXIT_FAILURE);
        }
    }

    void *addr = shmat(shm_id, NULL, 0);
    if (addr == (void *) -1)
    {
        perror("shmat failed in shared_memory_create");
        exit(EXIT_FAILURE);
    }

    // Si créé nouvellement, initialiser la mémoire à zéro
    if (errno != EEXIST)
    {
        memset(addr, 0, size);
    }

    return addr;
}

/**
 * Ouvre un sémaphore nommé existant.
 * @param name Nom du sémaphore.
 * @return Pointeur vers le sémaphore.
 */
sem_t *open_semaphore(const char *name)
{
    sem_t *sem = sem_open(name, 0);
    if (sem == SEM_FAILED)
    {
        perror("Erreur lors de l'ouverture du sémaphore");
        exit(EXIT_FAILURE);
    }
    return sem;
}

/**
 * Supprime une mémoire partagée.
 * @param key Clé de la mémoire partagée.
 */
void remove_shared_memory(key_t key)
{
    // Ajouter le PID du processus pour éviter les collisions de clés
    // On met getpid() car cette fonction est appelée par le parent
    key = key + getpid();

    int shm_id = shmget(key, 0, 0);
    if (shm_id == -1)
    {
        perror("shmget failed in remove_shared_memory");
        return;
    }

    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
        perror("shmctl IPC_RMID failed in remove_shared_memory");
    }
}
